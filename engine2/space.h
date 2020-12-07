#ifndef ENGINE2_SPACE_H_
#define ENGINE2_SPACE_H_

#include <list>
#include <queue>
#include <random>
#include <variant>
#include <vector>

#include "engine2/impl/rect_search_tree.h"
#include "engine2/physics_object.h"

namespace engine2 {

template <int N, class... ObjectTypes>
class Space {
 private:
  struct Motion;

 public:
  Space(const Rect<int64_t, N>& rect);

  using Variant = std::variant<ObjectTypes*...>;
  struct Iterator {
    Variant& operator*();
    Iterator& operator++();
    bool operator==(const Iterator& other) const;

    typename std::list<Motion>::iterator list_iterator;
  };

  template <class T>
  Iterator Add(T* object);

  void Remove(Iterator iterator);

  // TODO instead use time delta??
  void AdvanceTime(double new_time_seconds);

 private:
  friend class Iterator;

  struct Motion {
    Variant object;
    PhysicsObject<N>* physics;
    Rect<int64_t, N + 1> enclosing_rect{};
    typename RectSearchTree<N + 1, Motion*>::Iterator tree_iterator;
    bool marked_for_removal = false;

    double Time() const { return enclosing_rect.pos[N] / 1000.; }

    void UpdateEnclosingRect(RectSearchTree<N + 1, Motion*>* tree,
                             double start_time,
                             double finish_time) {
      Rect<int64_t, N> start_rect = physics->GetRect();
      Rect<int64_t, N> finish_rect = physics->GetRectAfterTime(finish_time);
      for (int i = 0; i < N; ++i) {
        enclosing_rect.pos[i] = std::min(start_rect.pos[i], finish_rect.pos[i]);
        enclosing_rect.size[i] =
            std::max(start_rect.pos[i] + start_rect.size[i],
                     finish_rect.pos[i] + finish_rect.size[i]) -
            enclosing_rect.pos[i];
      }

      // TODO offset by reference time which is set at start of update?
      // TODO time units??
      enclosing_rect.pos[N] = start_time * 1000;
      enclosing_rect.size[N] = (finish_time - start_time) * 1000;

      // Update tree storage
      tree_iterator = tree->Move(std::move(tree_iterator), enclosing_rect);
    }
  };

  struct Collision {
    Motion* motion_a;
    Motion* motion_b;
    double time;
    int dimension;
    bool operator>(const Collision& other) const { return time > other.time; }
    bool IsValid() const {
      return motion_a->physics->GetRectAfterTime(time - motion_a->Time())
          .Touches(
              motion_b->physics->GetRectAfterTime(time - motion_b->Time()));
    }
  };

  struct MultiCollision {
    std::vector<Motion*> motions;
    double time;
    double mass_sum;
    Point<double, N> velocity_sum;
    int dimension;

    MultiCollision(double time, int dimension)
        : time(time), mass_sum(0.), velocity_sum{}, dimension(dimension) {
      motions.reserve(2);
    }

    void Add(Motion* motion) {
      motions.push_back(motion);
      mass_sum += motion->physics->mass_kg;
      velocity_sum += motion->physics->velocity;
    }

    void Merge(const MultiCollision& other) {
      for (Motion* other_motion : other.motions) {
        bool found = false;
        for (Motion* motion : motions) {
          if (motion == other_motion) {
            found = true;
            break;
          }
        }
        if (!found)
          Add(other_motion);
      }
    }
  };

  static typename std::vector<MultiCollision>::iterator
  FindMultiCollisionContainingMotion(std::vector<MultiCollision>* mcs,
                                     Motion* motion,
                                     int dimension) {
    for (auto iter = mcs->begin(); iter != mcs->end(); ++iter) {
      if ((*iter).dimension != dimension)
        continue;

      for (Motion* mc_motion : (*iter).motions) {
        if (mc_motion == motion)
          return iter;
      }
    }
    return mcs->end();
  }

  using CollisionQueue = std::priority_queue<Collision,
                                             std::vector<Collision>,
                                             std::greater<Collision>>;

  void FindCollisions(CollisionQueue* queue, Motion* motion_a);

  typename std::list<Motion>::iterator RemoveInternal(
      typename std::list<Motion>::iterator iterator) {
    (*iterator).tree_iterator.Erase();
    return motions_.erase(iterator);
  }

  std::list<Motion> motions_;
  std::unique_ptr<RectSearchTree<N + 1, Motion*>> tree_;
  int advance_time_call_depth_ = 0;

  // TODO set in constructor
  double time_seconds_ = 0.;
};

template <int N, class... ObjectTypes>
Space<N, ObjectTypes...>::Space(const Rect<int64_t, N>& rect) {
  Rect<int64_t, N + 1> rect_with_time;
  for (int i = 0; i < N; ++i) {
    rect_with_time.pos[i] = rect.pos[i];
    rect_with_time.size[i] = rect.size[i];
  }

  // TODO time scale is temporary
  rect_with_time.pos[N] = 0;
  rect_with_time.size[N] = 1'000'000;

  // TODO want custom breakdown?
  tree_ = RectSearchTree<N + 1, Motion*>::Create(rect_with_time, N * 2);
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Variant&
Space<N, ObjectTypes...>::Iterator::operator*() {
  return (*list_iterator).object;
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Iterator&
Space<N, ObjectTypes...>::Iterator::operator++() {
  ++list_iterator;
  return *this;
}

template <int N, class... ObjectTypes>
bool Space<N, ObjectTypes...>::Iterator::operator==(
    const Iterator& other) const {
  return list_iterator == other.list_iterator;
}

template <int N, class... ObjectTypes>
template <class T>
typename Space<N, ObjectTypes...>::Iterator Space<N, ObjectTypes...>::Add(
    T* object) {
  motions_.emplace_front();
  Motion& motion = *(motions_.begin());

  motion.object = object;

  // TODO static assert
  motion.physics = object->physics();
  motion.tree_iterator = tree_->Insert(motion.enclosing_rect, &motion);
  return Iterator{motions_.begin()};
}

template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::Remove(Iterator iterator) {
  if (advance_time_call_depth_ > 0) {
    (*(iterator.list_iterator)).marked_for_removal = true;
  } else {
    RemoveInternal(iterator.list_iterator);
  }
}

template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::FindCollisions(CollisionQueue* queue,
                                              Motion* motion_a) {
  for (Motion* motion_b : tree_->Near(motion_a->enclosing_rect)) {
    if (motion_a == motion_b)
      continue;

    // If there's a collision, calculate dt and enqueue, otherwise skip
    // TODO time unit?
    auto [ab_collision_time, dimension] =
        GetCollisionTime(*(motion_a->physics), motion_a->Time(),
                         *(motion_b->physics), motion_b->Time());

    if (ab_collision_time < 0)
      continue;

    queue->push({motion_a, motion_b, ab_collision_time, dimension});
  }
}

// TODO collect requirements for objects
template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::AdvanceTime(double new_time_seconds) {
  ++advance_time_call_depth_;
  // TODO use custom breakdown for search tree

  // Find object final positions ignoring collisions.
  for (auto motion_iterator = motions_.begin();
       motion_iterator != motions_.end(); ++motion_iterator) {
    // Handle pending removals
    while (motion_iterator != motions_.end() &&
           (*motion_iterator).marked_for_removal) {
      motion_iterator = RemoveInternal(motion_iterator);
    }
    if (motion_iterator == motions_.end())
      break;

    Motion& motion = *motion_iterator;
    motion.UpdateEnclosingRect(tree_.get(), time_seconds_, new_time_seconds);
  }

  // Find first collisions and enqueue by earliest time.
  CollisionQueue queue;
  for (Motion& motion : motions_)
    FindCollisions(&queue, &motion);

  // Process collisions and motion until all objects have reached the end
  // time.
  while (!queue.empty()) {
    // Collect all collisions happening at the same time
    std::vector<MultiCollision> multicollisions;
    double current_mc_time = queue.top().time;
    do {
      Collision collision = queue.top();

      // Skip if the collision isn't actually happening.
      if (!collision.IsValid()) {
        queue.pop();
        if (queue.empty() || queue.top().time != current_mc_time)
          break;
        continue;
      }

      auto mc_a = FindMultiCollisionContainingMotion(
          &multicollisions, collision.motion_a, collision.dimension);
      auto mc_b = FindMultiCollisionContainingMotion(
          &multicollisions, collision.motion_b, collision.dimension);

      if (mc_a != multicollisions.end()) {
        if (mc_b != multicollisions.end()) {
          // Do nothing if both a and b are included in the same mc already.
          if (mc_a != mc_b) {
            // If a and b are in different mcs, merge them.
            (*mc_a).Merge(*mc_b);
            multicollisions.erase(mc_b);
          }
        } else {
          (*mc_a).Add(collision.motion_b);
        }
      } else {
        if (mc_b != multicollisions.end()) {
          (*mc_b).Add(collision.motion_a);
        } else {
          // Neither motion is part of a mc so create a new one
          multicollisions.emplace_back(collision.time, collision.dimension);
          multicollisions.back().Add(collision.motion_a);
          multicollisions.back().Add(collision.motion_b);
        }
      }

      queue.pop();
    } while (!queue.empty() && queue.top().time == current_mc_time);

    if (multicollisions.empty())
      continue;

    // 1. Update positions to time of collision
    for (MultiCollision& mc : multicollisions) {
      for (Motion* motion : mc.motions)
        motion->physics->Update(mc.time - motion->Time());
    }

    // 2. Run handlers
    for (MultiCollision& mc : multicollisions) {
      for (auto iter_a = mc.motions.begin(); iter_a != mc.motions.end();
           ++iter_a) {
        Motion* motion_a = *iter_a;
        std::visit(
            [&mc, motion_a, &iter_a](auto* object_a) {
              for (auto iter_b = iter_a + 1; iter_b != mc.motions.end();
                   ++iter_b) {
                Motion* motion_b = *iter_b;
                if (motion_a == motion_b)
                  continue;
                std::visit(
                    [&mc, object_a](auto* object_b) {
                      object_a->OnCollideWith(*object_b, mc.time);
                      object_b->OnCollideWith(*object_a, mc.time);
                    },
                    motion_b->object);
              }
            },
            motion_a->object);
      }
    }

    // 3. Update velocities and enclosing rects
    for (MultiCollision& mc : multicollisions) {
      
      for (Motion* motion : mc.motions) {
        motion->physics->HalfElasticCollision1D(
            mc.mass_sum - motion->physics->mass_kg,
            (mc.velocity_sum - motion->physics->velocity) /
                (mc.motions.size() - 1.),
            mc.dimension);
        motion->UpdateEnclosingRect(tree_.get(), mc.time, new_time_seconds);
      }
    }

    // 4. Find new collisions
    for (MultiCollision& mc : multicollisions) {
      for (Motion* motion : mc.motions)
        FindCollisions(&queue, motion);
    }
  }
  time_seconds_ = new_time_seconds;

  // Handle any removals that happened during collision handling
  auto iterator = motions_.begin();
  while (iterator != motions_.end()) {
    if ((*iterator).marked_for_removal)
      iterator = RemoveInternal(iterator);
    else
      ++iterator;
  }

  // Update objects to final positions
  for (Motion& motion : motions_)
    motion.physics->Update(new_time_seconds - motion.Time());

  --advance_time_call_depth_;
}

}  // namespace engine2

#endif  // ENGINE2_SPACE_H_