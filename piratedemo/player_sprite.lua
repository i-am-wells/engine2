local hit_box_x, hit_box_y, hit_box_w, hit_box_h = 3, 22, 10, 10
local draw_offset = {x=-hit_box_x, y=-hit_box_y}
local sprite_w, sprite_h = 16, 32

local frame_duration_ms = 256

local make_frame = function(x, y, duration_ms)
  return {
    source_rect = {x=x * sprite_w, y=y * sprite_h, w=sprite_w, h=sprite_h},
    dest_offset = draw_offset,
    duration_ms = duration_ms
  }
end

local sprites = {}
for i, direction in ipairs{'south', 'north', 'east', 'west'} do
  local x = i - 1
  sprites[#sprites+1] = {
    name = 'stand_'..direction,
    frames = {
      make_frame(x, 1, frame_duration_ms),
      make_frame(x, 0, frame_duration_ms),
    }
  }

  local walk_frames = {}
  for y = 1, 4 do
    walk_frames[#walk_frames+1] = make_frame(x, y, frame_duration_ms)
  end
  sprites[#sprites+1] = {
    name = 'walk_'..direction,
    frames = walk_frames,
  }
end

return {
  image = 'piratedemo/player_sprite.lua',
  sprites = sprites
}