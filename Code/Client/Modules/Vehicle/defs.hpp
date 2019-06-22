auto spawn(zpl_vec3 position, 
           mafia_vehicle* spawn_struct, MafiaSDK::C_Car* cached) -> MafiaSDK::C_Car*;

auto despawn(mafia_vehicle* vehicle) -> void;
