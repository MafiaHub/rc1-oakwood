#pragma once

auto vehicle_despawn(mafia_vehicle* vehicle) -> void {
    if(vehicle && vehicle->car) {
        for (int i = 0; i < 4; i++) {
            auto seat = vehicle->seats[i];
            if (seat != -1) {
                auto player_ent = librg_entity_fetch(&network_context, seat);
                if (player_ent && player_ent->user_data) {
                    auto player = (mafia_player*)player_ent->user_data;
                    player->ped->Intern_FromCar();
                }
            }
        }

        if (vehicle->is_car_in_radar)
            MafiaSDK::GetMission()->GetGame()->GetIndicators()->RadarRemoveCar(vehicle->car);
            
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(vehicle->car);
        vehicle->car = nullptr;
    }
}