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

    auto player_model = (MafiaSDK::I3D_Model*)MafiaSDK::I3DGetDriver()->CreateFrame(MafiaSDK::I3D_Driver_Enum::FrameType::MODEL);
    while(MafiaSDK::GetModelCache()->Open(player_model, model, NULL, NULL, NULL, NULL))  {
        printf("Error: Unable to create player model <%s> !\n", model);
    }

    player_model->SetName("testing_player");
    player_model->SetScale(default_scale);
    player_model->SetWorldPos(default_pos);
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
        auto game = MafiaSDK::GetMission()->GetGame();
        if (game) {
            game->GetCamera()->SetCar(NULL);
            game->GetCamera()->SetMode(true, 1);
            game->GetCamera()->SetPlayer(new_ped);
            game->SetLocalPlayer(new_ped);
        }

        MafiaSDK::GetIndicators()->PlayerSetWingmanLives(100);

        auto player = get_local_player();
        if (player) {
            local_player.dead = false;
            if (player->ped) {
                player_despawn(player->ped);
            }

            player->ped = new_ped;
            strcpy(player->name, GlobalConfig.username);
        }
    }

    new_ped->GetInterface()->humanObject.health = health;

    if (!is_in_car) {
        new_ped->GetInterface()->humanObject.entity.position = default_pos;
        new_ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);
    }

    //Foreach every weapon in inventory and give it to the player
    for (size_t i = 0; i < 8; i++) {
        S_GameItem* item = (S_GameItem*)&inventory.items[i];
        if (item->weaponId != expectedWeaponId) {
            new_ped->G_Inventory_AddItem(*item);
        }
    }
    
    //TODO(DavoSK): Make it more fancy !
    //Select right weapon
    hooks::select_by_id_original((void *)new_ped->GetInventory(), current_wep, nullptr);
    new_ped->Do_ChangeWeapon(0, 0);
    new_ped->ChangeWeaponModel();

    //If player have hands do holster
    if (current_wep == 0)
        new_ped->Do_Holster();

    return new_ped;
}
