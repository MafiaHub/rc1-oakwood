#pragma once

auto player_spawn(zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId,
                  bool is_in_car) -> MafiaSDK::C_Player*;

auto vehicle_spawn(zpl_vec3 position,
                   mafia_vehicle* spawn_struct) -> MafiaSDK::C_Car*;

auto vehicle_remove(mafia_vehicle* vehicle) -> void;