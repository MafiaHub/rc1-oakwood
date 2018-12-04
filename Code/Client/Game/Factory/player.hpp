#pragma once

auto player_spawn(zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId,
                  bool is_in_car) -> MafiaSDK::C_Player *{
    
    S_vector default_scale = { 1.0f, 1.0f, 1.0f };
    S_vector default_pos = EXPAND_VEC(position);

    auto player_model = MafiaSDK::I3DGetDriver()->CreateFrame<MafiaSDK::I3D_Model>(MafiaSDK::I3D_Driver_Enum::FrameType::MODEL);
    player_model->SetName("testing_player");
    player_model->SetScale(default_scale);
    player_model->SetWorldPos(default_pos);

    MafiaSDK::GetModelCache()->Open(player_model, model, NULL, NULL, NULL, NULL);
    player_model->Update();
    
    MafiaSDK::C_Player *new_ped = nullptr;

    if (is_local_player)
        new_ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Player));
    else
        new_ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Enemy));

    new_ped->Init(player_model);

    if (!is_local_player)
        new_ped->SetBehavior(MafiaSDK::C_Human_Enum::BehaviorStates::DoesntReactOnWeapon);

    new_ped->SetShooting(1.0f);
    new_ped->SetActive(1);
  
    MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_ped);

    if (is_local_player) {
        MafiaSDK::GetMission()->GetGame()->SetLocalPlayer(new_ped);
        MafiaSDK::GetMission()->GetGame()->GetCamera()->SetCar(NULL);
        MafiaSDK::GetMission()->GetGame()->GetCamera()->SetMode(true, 1);
        MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(new_ped);
        MafiaSDK::GetIndicators()->PlayerSetWingmanLives(100);

        if (local_player.ped) {
            player_despawn(reinterpret_cast<MafiaSDK::C_Player*>(local_player.ped));
        }

        local_player.dead = false;
        local_player.ped = new_ped;

        auto local_userdata = (mafia_player*)local_player.entity.user_data;
        local_userdata->ped = new_ped;
        strcpy(local_userdata->name, GlobalConfig.username.c_str());
    }

    new_ped->GetInterface()->humanObject.health = health;

    if (!is_in_car) {
        new_ped->GetInterface()->humanObject.entity.position = default_pos;
        new_ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);
    }

    for (size_t i = 0; i < 8; i++) {
        S_GameItem* item = (S_GameItem*)&inventory.items[i];
        if (item->weaponId != expectedWeaponId) {
            ((MafiaSDK::C_Human*)new_ped)->G_Inventory_AddItem(*item);
        }
    }

    ((MafiaSDK::C_Human*)new_ped)->G_Inventory_SelectByID(current_wep);

    if (!is_local_player && (current_wep != 0 || current_wep != -1))
        ((MafiaSDK::C_Human*)new_ped)->Do_ChangeWeapon(0, 0);

    ((MafiaSDK::C_Human*)new_ped)->ChangeWeaponModel();

    if (current_wep == 0)
        ((MafiaSDK::C_Human*)new_ped)->Do_Holster();

    return new_ped;
}
