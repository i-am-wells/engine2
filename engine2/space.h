#ifndef ENGINE2_SPACE_H_
#define ENGINE2_SPACE_H_

#include <list>
#include <queue>
#include <variant>
#include <vector>

#include "engine2/impl/rect_search_tree.h"
#include "engine2/physics_object.h"
#include "engine2/time.h"

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
    bool operator!=(const Iterator& other) const;

    typename RectSearchTree<N + 1, Motion*>::NearIterator tree_iterator;
  };

  template <class T>
  Iterator Add(T* object);

  void Remove(Iterator iterator);

  void AdvanceTime(const Time::Delta& delta);

  struct NearView {
    typename RectSearchTree<N + 1, Motion*>::NearIterable tree_view;
    Iterator begin();
    Iterator end();
  };
  NearView Near(const Rect<int64_t, N>& rect);

 private:
  friend class Iterator;

  struct Motion {
    Variant object;
    PhysicsObject<N>* physics;
    Rect<int64_t, N + 1> enclosing_rect{};
    typename RectSearchTree<N + 1, Motion*>::NearIterator tree_iterator;
    typename std::list<Motion>::iterator list_iterator;
    bool marked_for_removal = false;

    Time GetTime() const {
      return Time::FromMicroseconds(enclosing_rect.pos[N]);
    }

    void UpdateEnclosingRect(RectSearchTree<N + 1, Motion*>* tree,
                             const Time& start_time,
                             const Time& finish_time) {
      Rect<int64_t, N> start_rect = physics->GetRect();
      Rect<int64_t, N> finish_rect =
          physics->GetRectAfterTime(finish_time - start_time);
      for (int i = 0; i < N; ++i) {
        enclosing_rect.pos[i] = std::min(start_rect.pos[i], finish_rect.pos[i]);
        enclosing_rect.size[i] =
            std::max(start_rect.pos[i] + start_rect.size[i],
                     finish_rect.pos[i] + finish_rect.size[i]) -
            enclosing_rect.pos[i];
      }

      enclosing_rect.pos[N] = start_time.ToMicroseconds();
      enclosing_rect.size[N] = (finish_time - start_time).ToMicroseconds();

      // Update tree storage
      tree_iterator = tree->Move(std::move(tree_iterator), enclosing_rect);
    }

    void UpdatePositionToTime(const Time& time) {
      physics->Update(time - GetTime());
    }
  };

  struct Collision {
    Motion* motion_a;
    Motion* motion_b;
    Time time;
    int dimension;
    bool operator>(const Collision& other) const { return time > other.time; }
    bool IsValid() const {
      if (!motion_a->physics->GetRectAfterTime(time - motion_a->GetTime())
               .Touches(motion_b->physics->GetRectAfterTime(
                   time - motion_b->GetTime()))) {
        return false;
      }

      if (motion_a->physics->rect.pos[dimension] <
          motion_b->physics->rect.pos[dimension]) {
        return motion_a->physics->velocity[dimension] >
               motion_b->physics->velocity[dimension];
      }
      return motion_b->physics->velocity[dimension] >
             motion_a->physics->velocity[dimension];
    }
    void UpdatePositions() {
      motion_a->UpdatePositionToTime(time);
      motion_b->UpdatePositionToTime(time);
    }
    void UpdateVelocitiesAndRects(RectSearchTree<N + 1, Motion*>* tree,
                                  Time new_time) {
      PhysicsObject<N>::ElasticCollision1D(motion_a->physics, motion_b->physics,
                                           dimension);
      motion_a->UpdateEnclosingRect(tree, time, new_time);
      motion_b->UpdateEnclosingRect(tree, time, new_time);
    }
  };

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
  Time time_ = Time::FromSeconds(0);
};

template <int N, class... ObjectTypes>
Space<N, ObjectTypes...>::Space(const Rect<int64_t, N>& rect) {
  Rect<int64_t, N + 1> rect_with_time;
  int64_t avg_size = 0;
  for (int i = 0; i < N; ++i) {
    rect_with_time.pos[i] = rect.pos[i];
    rect_with_time.size[i] = rect.size[i];
    avg_size += rect.size[i];
  }
  avg_size /= N;

  // The "time" dimension is represented as microseconds since the beginning of
  // an update.
  const int64_t time_max = 1'000'000;
  rect_with_time.pos[N] = 0;
  rect_with_time.size[N] = time_max;

  Point<double, N + 1> breakdown_scale = Point<double, N + 1>::Ones();
  breakdown_scale[N] = time_max / avg_size;
  tree_ = RectSearchTree<N + 1, Motion*>::Create(rect_with_time, N * 2,
                                                 breakdown_scale);
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Variant&
Space<N, ObjectTypes...>::Iterator::operator*() {
  return (*tree_iterator)->object;
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Iterator&
Space<N, ObjectTypes...>::Iterator::operator++() {
  ++tree_iterator;
  return *this;
}

template <int N, class... ObjectTypes>
bool Space<N, ObjectTypes...>::Iterator::operator==(
    const Iterator& other) const {
  return tree_iterator == other.tree_iterator;
}

template <int N, class... ObjectTypes>
bool Space<N, ObjectTypes...>::Iterator::operator!=(
    const Iterator& other) const {
  return tree_iterator != other.tree_iterator;
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

  // TODO set enclosing rect here! consider Near() case

  motion.tree_iterator = tree_->Insert(motion.enclosing_rect, &motion);
  motion.list_iterator = motions_.begin();

  // ???
  motion.UpdateEnclosingRect(tree_.get(), Time::FromMicroseconds(0),
                             Time::FromMicroseconds(1));

  return Iterator{motion.tree_iterator};
}

template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::Remove(Iterator iterator) {
  Motion* motion = *(iterator.tree_iterator);
  if (advance_time_call_depth_ > 0) {
    motion->marked_for_removal = true;
  } else {
    RemoveInternal(motion->list_iterator);
  }
}

template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::FindCollisions(CollisionQueue* queue,
                                              Motion* motion_a) {
  for (Motion* motion_b : tree_->Near(motion_a->enclosing_rect)) {
    if (motion_a == motion_b ||
        !motion_a->enclosing_rect.Overlaps(motion_b->enclosing_rect)) {
      continue;
    }

    // If there's a collision, calculate dt and enqueue, otherwise skip
    // TODO time unit?
    auto [ab_collision_time, dimension] =
        GetCollisionTime(*(motion_a->physics), motion_a->GetTime(),
                         *(motion_b->physics), motion_b->GetTime());

    if (ab_collision_time < Time())
      continue;

    queue->push({motion_a, motion_b, ab_collision_time, dimension});
  }
}

// TODO collect requirements for objects
template <int N, class... ObjectTypes>
void Space<N, ObjectTypes...>::AdvanceTime(const Time::Delta& delta) {
  Time start_time = Time::FromMicroseconds(0);
  Time end_time = start_time + delta;

  // TODO use or remove
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
    motion.UpdateEnclosingRect(tree_.get(), start_time, end_time);
  }

  // Find first collisions and enqueue by earliest time.
  CollisionQueue queue;
  for (Motion& motion : motions_)
    FindCollisions(&queue, &motion);

  // Process collisions and motion until all objects have reached the end
  // time.
  while (!queue.empty()) {
    Collision collision = queue.top();
    if (!collision.IsValid()) {
      queue.pop();
      continue;
    }

    // 1. Update positions to time of collision
    collision.UpdatePositions();

    // 2. Run handlers
    std::visit(
        [&collision](auto* object_a) {
          std::visit(
              [&collision, object_a](auto* object_b) {
                object_a->OnCollideWith(*object_b);
                object_b->OnCollideWith(*object_a);
              },
              collision.motion_b->object);
        },
        collision.motion_a->object);

    // 3. Update velocities and enclosing rects
    collision.UpdateVelocitiesAndRects(tree_.get(), end_time);

    // 4. Find new collisions
    FindCollisions(&queue, collision.motion_a);
    FindCollisions(&queue, collision.motion_b);

    queue.pop();
  }

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
    motion.physics->Update(end_time - motion.GetTime());

  --advance_time_call_depth_;
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::NearView Space<N, ObjectTypes...>::Near(
    const Rect<int64_t, N>& rect) {
  // TODO this is done a few places now, refactor
  Rect<int64_t, N + 1> rect_with_time;
  for (int i = 0; i < N; ++i) {
    rect_with_time.pos[i] = rect.pos[i];
    rect_with_time.size[i] = rect.size[i];
  }
  rect_with_time.pos[N] = 0;
  rect_with_time.size[N] = 1;
  return NearView{tree_->Near(rect_with_time)};
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Iterator
Space<N, ObjectTypes...>::NearView::begin() {
  return Iterator{tree_view.begin()};
}

template <int N, class... ObjectTypes>
typename Space<N, ObjectTypes...>::Iterator
Space<N, ObjectTypes...>::NearView::end() {
  return Iterator{tree_view.end()};
}

}  // namespace engine2

#endif  // ENGINE2_SPACE_H_