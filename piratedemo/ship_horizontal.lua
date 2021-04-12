local tileW, tileH = 16, 16

local frame = function(x, y, w, h, ms)
  return {
    source_rect = {x=x * tileW, y=y * tileH, w=w * tileW, h=h * tileH},
    duration_ms = ms,
  }
end

local sprite = function(name, x, y, w, h)
  return {
    name = name,
    frames = {frame(x, y, w, h, 0)},
  }
end

-- Wave
local wave = {
  name = 'wave',
  frames = {},
}
for x = 6, 21 do
  wave.frames[#wave.frames+1] = frame(x, 11, 1, 1, --[[ms=]]100)
end

local sprites = {
  image = 'piratedemo/ship_horizontal.png',
  sprites = {wave},
}

-- Ship sprites
for yy = 0, 10 do
  for xx = 0, 22 do
    local name = 'ship'..xx..'_'..yy
    sprites.sprites[#sprites.sprites+1] = sprite(name, xx, yy, 1, 1)
  end
end

return sprites