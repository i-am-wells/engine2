local common
common = {
  tile_size = {x=16, y=16},
  sprite_image = 'piratedemo/ship_vertical.png',
  sprite_info = 'piratedemo/ship_vertical.lua',
  sprite_name = function(x, y)
    return string.format('ship%s_%s', x, y)
  end,
  sprite_full_name = function(x, y)
    return string.format('%s:%s', common.sprite_info, common.sprite_name(x, y))
  end,
}
return common

