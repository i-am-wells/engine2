#ifndef ENGINE2_SPRITE_CACHE_H_
#define ENGINE2_SPRITE_CACHE_H_

#include <map>
#include <optional>
#include <string>

#include "engine2/sprite.h"
#include "engine2/texture_cache.h"

// TODO: Separate into cache and loader

namespace engine2 {
class SpriteSheetInfo;

// Sprites are loaded from sprite sheets stored in Lua objects. A sprite sheet
// defines one or more sprites:
//
// ```sprites.lua
// return {
//   image = 'path/to/image.png',
//   sprites = {
//     {
//        name = "tree",
//        frames = {
//          {
//            source_rect = {0, 0, 16, 32},
//            dest_offset = {10, 10},
//          },
//        }
//     },
//     {
//        name = "rock",
//        frames = {
//          {
//            source_rect = {16, 0, 16, 16},
//          },
//        }
//     },
//   }
// }
// ```
//
// Within SpriteCache, each sprite is retrieved using a string that combines the
// sprite sheet file path and the "name" field of the sprite. Example:
//
// "path/to/sprites.lua:tree" refers to the "tree" in sprites.lua.
//
// The sprite name can be omitted to refer to a sprite with the "name" field
// unset.
class SpriteCache {
 public:
  SpriteCache(TextureCache* texture_cache);

  // Retrieve the sprite corresponding to the path. Returns nullptr if the
  // sprite isn't found.
  //
  // If the sprite for |path| isn't cached, we attempt to load the sprite sheet
  // as described above. Therefore this method can be used to preload a sprite
  // sheet by passing the bare name of the sprite sheet file and ignoring the
  // return value.
  Sprite* Get(const std::string& path);

  // Manually store a sprite at |path|, overwriting the original value at
  // |path|.
  void Put(const std::string& path, Sprite sprite);

  // Create a new sprite at |path|.
  Sprite* Create(const std::string& path,
                 Texture* texture = nullptr,
                 int frame_count = 1);

  // Given a pointer to a sprite, return the path it's stored under, or nullopt
  // if it can't be found.
  std::optional<std::string> LookupName(Sprite* sprite) const;

  // Load sprites defined in a SpriteSheetInfo. Returns false if the image can't
  // be read; true otherwise.
  bool LoadSpriteSheet(SpriteSheetInfo info);
  bool LoadSpriteSheet(const std::string& sprite_sheet_luadata_file);

  std::map<std::string, Sprite>::iterator begin() { return map_.begin(); }
  std::map<std::string, Sprite>::iterator end() { return map_.end(); }

 private:
  Sprite* LoadInternal(const std::string& path);

  TextureCache* texture_cache_;
  std::map<std::string, Sprite> map_;
};

}  // namespace engine2

#endif  // ENGINE2_SPRITE_CACHE_H_