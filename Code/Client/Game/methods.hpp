#pragma once

void cam_set_target(librg_entity* ent) {
    if (MafiaSDK::GetMission()->GetGame()) {
        auto camera = MafiaSDK::GetMission()->GetGame()->GetCamera();
        printf("Camera: %X, ent: %X\n", camera, ent);

        if (camera != nullptr) {
            if (!ent) {
                auto lped = modules::player::get_local_player()->ped;
                auto veh = lped->GetInterface()->playersCar;

                if (veh) {
                    camera->SetCar(veh);
                }
                else
                {
                    camera->SetCar(NULL);
                    camera->SetPlayer(lped);
                }
            }
            else if (ent->user_data && ent->type == TYPE_PLAYER) {
                auto player = (mafia_player*)ent->user_data;
                auto ped = player->ped;

                if (!ped) return;

                auto veh = ped->GetInterface()->playersCar;

                if (veh) {
                    camera->SetCar(veh);
                }
                else {
                    camera->SetCar(NULL);
                    camera->SetPlayer(ped);
                }
            }
            else if (ent->user_data && ent->type == TYPE_VEHICLE) {
                auto vehicle = (mafia_vehicle*)ent->user_data;
                camera->SetPlayer(NULL);
                camera->SetCar(vehicle->car);
            }
        }
    }
}
