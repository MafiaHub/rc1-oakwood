#pragma once

auto player_spawn(zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId) -> MafiaSDK::C_Player *;
