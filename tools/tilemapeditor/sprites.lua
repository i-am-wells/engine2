local tileW, tileH = 16, 16
local sprite = function(name, x, y, w, h)
  return {
    name = name,
    frames = {
      {
        source_rect = {x=x * tileW, y=y * tileH, w=w * tileW, h=h * tileH},
      },
    },
  }
end

return {
  image = 'tools/tilemapeditor/sprites.png',
  sprites = {
    sprite('grass0', 0, 0, 1, 1),
    sprite('grass1', 1, 0, 1, 1),
    sprite('flowers0', 2, 0, 1, 1),
    sprite('flowers1', 3, 0, 1, 1),

    sprite('tree0', 1, 2, 3, 4),
    sprite('rock0', 5, 4, 1, 1),
    sprite('log0', 4, 5, 3, 1),
  }
}