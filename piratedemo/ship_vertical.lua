local tileW, tileH = 16, 16
local dest_offset = {x=0, y=0}

local frame = function(x, y, w, h, ms)
  return {
    source_rect = {x=x * tileW, y=y * tileH, w=w * tileW, h=h * tileH},
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
  image = 'piratedemo/ship_vertical.png',
  sprites = {},
}

-- Ship sprites
for yy = 0, 26 do
  for xx = 0, 19 do
    local name = 'ship'..xx..'_'..yy
    sprites.sprites[#sprites.sprites+1] = sprite(name, xx, yy, 1, 1)
  end
end

return sprites