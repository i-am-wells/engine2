#ifndef ENGINE2_BASIC_WINDOW_H_
#define ENGINE2_BASIC_WINDOW_H_

#include <memory>

#include "engine2/graphics2d.h"
#include "engine2/rect.h"
#include "engine2/threads/draw_thread.h"
#include "engine2/window.h"

namespace engine2 {

class BasicWindow : public DrawThread::Delegate {
 public:
  class Delegate {
   public:
    virtual bool SetUp(Graphics2D* graphics, Window* window) { return true; }
    virtual bool ReadState() { return true; }
    virtual bool Draw() { return true; }
    virtual void TearDown() {}
    virtual ~Delegate() = default;
  };

  BasicWindow(const Rect& rect,
              std::unique_ptr<Delegate> delegate,
              const std::string& name = "engine2",
              int sdl_window_flags = 0,
              int sdl_renderer_flags = 0);
  ~BasicWindow();

  Graphics2D* graphics() { return graphics_.get(); }
  Window* window() { return window_.get(); }

  // DrawThread::Delegate implementation
  bool SetUp() override;
  bool ReadState() override;
  bool Draw() override;
  void TearDown() override;

 protected:
  std::unique_ptr<Graphics2D> graphics_;
  std::unique_ptr<Window> window_;

 private:
  std::string name_;
  Rect rect_;
  int window_flags_;
  int renderer_flags_;
  std::unique_ptr<Delegate> delegate_;
};

}  // namespace engine2

#endif  // ENGINE2_BASIC_WINDOW_H_