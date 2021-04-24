local common = require 'piratedemo.common'

local all_points_in = function(x0, y0, x1, y1)
  local result = {}
  for yy = y0, y1 do
    for xx = x0, x1 do
      result[#result+1] = {xx, yy}
    end
  end
  return result
end

local make_walls = function(points)
  local result = {}
  for i, run in ipairs(points) do
    for ii, point in ipairs(run) do
      local x_points = result[point[1]]
      if x_points == nil then
        x_points = {}
        result[point[1]] = x_points
      end
      x_points[point[2]] = true
    end
  end
  return result
end

local walls = make_walls({
  -- test
  {{0,0}},

  -- Main deck stern wall
  all_points_in(9, 0, 17, 0),

  -- Main deck side walls
  all_points_in(9, 1, 9, 16),
  all_points_in(17, 1, 17, 16),

  -- Main deck bow walls
  {{10, 17}, {11, 18}, {15, 18}, {16, 17}},
  all_points_in(12, 19, 14, 19),

  -- Upper inner railing
  all_points_in(11, 3, 15, 3),
  -- Lower inner railing
  all_points_in(11, 14, 15, 14),
})

local wall_tag = {'wall'}

local tiles = {}
for y = 0, 19 do
  for x = 0, 26 do
    local tags
    if walls[x] and walls[x][y] then
      tags = wall_tag
    end

    tiles[#tiles+1] = {
      sprite_name = common.sprite_full_name(x, y),
      tags = tags,
    }
  end
end

return {
  layers = 1,
  grid_size = {x=50, y=50},
  tile_size = common.tile_size,
  world_position = {x=0, y=0},
  tags = wall_tag,
  tiles = tiles,
}
