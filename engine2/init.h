#ifndef ENGINE2_INIT_H_
#define ENGINE2_INIT_H_

namespace engine2 {

// Call this once before using engine2. Returns true on success.
bool Init();

// Call this once after you're done using engine2.
void Quit();

}  // namespace engine2

#endif  // ENGINE2_INIT_H_