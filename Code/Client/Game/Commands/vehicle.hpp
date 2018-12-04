#pragma once

auto vehicle_despawn(mafia_vehicle* vehicle) -> void {
    if(vehicle && vehicle->car) {
        for (int i = 0; i < 4; i++) {
            
            if (vehicle->seats[i] != -1) {
                auto player_ent = librg_entity_fetch(&network_context, vehicle->seats[i]);
                if (player_ent && player_ent->user_data) { 
                    auto player = (mafia_player*)player_ent->user_data;
                    if (player->ped) {
                        player->ped->Intern_FromCar();
                    }
                    

                    player->vehicle_id = -1;
                    vehicle->seats[i] = -1;
                }
            }
        }

        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(vehicle->car);
    }
}
