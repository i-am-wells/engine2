#ifndef ENGINE2_ENGINE2_H_
#define ENGINE2_ENGINE2_H_

#include <memory>

#include <SDL2/SDL_keycode.h>

#include "engine2/memory/weak_pointer.h"
#include "engine2/threads/threads_owner.h"
#include "engine2/timing.h"
#include "engine2/types.h"

namespace engine2 {

class Engine2 {
 public:
  static std::unique_ptr<Engine2> Create(
      std::unique_ptr<DrawThread::Delegate> draw_delegate);
  virtual void Run() = 0;
  virtual void Stop() = 0;

  class Cancelable {
   public:
    virtual void Cancel() = 0;
    virtual ~Cancelable() = default;
  };

  class RunClause {
   public:
    virtual std::unique_ptr<Cancelable> Run(Callback callback) = 0;
    virtual ~RunClause() = default;
  };

  // API here!
  // Creates a RunClause for running callbacks every frame.
  virtual std::unique_ptr<RunClause> EveryFrame() = 0;
  /*
  RunClause* Every(double num, Timing::TimeUnit unit);
  RunClause* After(double num, Timing::TimeUnit unit);
  RunClause* After(Condition condition);
  RunClause* Whenever(Condition condition);
  RunClause* Until(Condition condition);

  // TODO user-defined and fired events
  */

  class KeyboardEventClause {
   public:
    virtual WeakPointer<KeyboardEventClause> Pressed(Callback callback) = 0;
    virtual WeakPointer<KeyboardEventClause> Pressed(
        KeyboardCallback callback) = 0;
    virtual WeakPointer<KeyboardEventClause> Released(Callback callback) = 0;
    virtual WeakPointer<KeyboardEventClause> Released(
        KeyboardCallback callback) = 0;
    virtual ~KeyboardEventClause() = default;
  };

  virtual void EnableKeyRepeat(bool enable) = 0;
  virtual std::unique_ptr<KeyboardEventClause> OnKey(SDL_Keycode key_code) = 0;
  virtual std::unique_ptr<KeyboardEventClause> OnKey(
      const std::string& name) = 0;

  virtual ~Engine2() = default;
};

}  // namespace engine2

#endif  // ENGINE2_ENGINE2_H_