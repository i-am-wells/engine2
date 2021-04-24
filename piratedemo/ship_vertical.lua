local common = require 'piratedemo.common'

local tile_w, tile_h = common.tile_size.x, common.tile_size.y
local dest_offset = {x=0, y=0}

local frame = function(x, y, w, h, ms)
  return {
    source_rect = {x=x * tile_w, y=y * tile_h, w=w * tile_w, h=h * tile_h},
    dest_offset = dest_offset,
    duration_ms = ms,
  }
end

local sprite = function(name, x, y, w, h)
  return {
    name = name,
    frames = {frame(x, y, w, h, 0)},
  }
end

local sprites = {
  image = common.sprite_image,
  sprites = {},
}

-- Ship sprites
for yy = 0, 19 do
  for xx = 0, 26 do
    sprites.sprites[#sprites.sprites+1] = sprite(common.sprite_name(xx, yy), xx, yy, 1, 1)
  end
end

return sprites