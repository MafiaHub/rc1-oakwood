#pragma once

auto player_spawn(mafia_player *player,
                  zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId) -> void {
                      
    Vector3D default_scale = { 1.0f, 1.0f, 1.0f };
    Vector3D default_pos = EXPAND_VEC(position);

    auto player_frame = new MafiaSDK::I3D_Frame();
    player_frame->SetName("testing_player");
    player_frame->LoadModel(model);
    player_frame->SetScale(default_scale);
    player_frame->SetPos(default_pos);

    if (is_local_player)
        local_player.ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Player));
    else
        player->ped = reinterpret_cast<MafiaSDK::C_Human*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Enemy));

    local_player.ped->Init(player_frame);

    if (!is_local_player)
        player->ped->SetBehavior(MafiaSDK::C_Human_Enum::BehaviorStates::DoesntReactOnWeapon);

    local_player.ped->SetShooting(1.0f);

    MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(local_player.ped);

    if (is_local_player)
        MafiaSDK::GetMission()->GetGame()->SetLocalPlayer(local_player.ped);

    local_player.ped->GetInterface()->humanObject.health = health;
    local_player.ped->GetInterface()->humanObject.entity.position = default_pos;
    local_player.ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);

    for (size_t i = 0; i < 8; i++) {
        S_GameItem* item = (S_GameItem*)&inventory.items[i];
        if (item->weaponId != expectedWeaponId) {
            ((MafiaSDK::C_Human*)local_player.ped)->G_Inventory_AddItem(*item);
        }
    }

    ((MafiaSDK::C_Human*)local_player.ped)->G_Inventory_SelectByID(current_wep);

    if (!is_local_player)
        ((MafiaSDK::C_Human*)player->ped)->Do_ChangeWeapon(0, 0);

    ((MafiaSDK::C_Human*)local_player.ped)->ChangeWeaponModel();

    if (current_wep == 0)
        ((MafiaSDK::C_Human*)local_player.ped)->Do_Holster();

    MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(local_player.ped);
}