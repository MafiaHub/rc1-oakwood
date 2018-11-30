#pragma once

inline auto drop_entitycreate(librg_event* evnt) -> void {
    auto drop = new mafia_weapon_drop;
    *drop = {};
    librg_data_rptr(evnt->data, drop->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, &drop->weapon, sizeof(inventory_item));

    auto drop_model = MafiaSDK::I3DGetDriver()->CreateFrame<MafiaSDK::I3D_Model>(MafiaSDK::I3D_Driver_Enum::FrameType::MODEL);
    S_vector default_scale = { 1.5f, 1.5f, 1.5f };
    S_vector default_pos = EXPAND_VEC(evnt->entity->position);
    drop_model->SetName("testing_drop");
    MafiaSDK::GetModelCache()->Open(drop_model, drop->model, NULL, NULL, NULL, NULL);
    drop_model->SetScale(default_scale);
    drop_model->SetWorldPos(default_pos);

    auto weapon_drop = new MafiaSDK::C_Drop_In_Weapon;
    weapon_drop->Init(drop_model);
    weapon_drop->GetInterface()->DropItem = { drop->weapon.weaponId, drop->weapon.ammoLoaded, drop->weapon.ammoHidden, NULL };
    weapon_drop->SetActive(1);
    drop->weapon_drop_actor = weapon_drop;
    MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(weapon_drop);
    evnt->entity->user_data = drop;
}

inline auto drop_entityremove(librg_event* evnt) -> void {
    auto drop = (mafia_weapon_drop *)evnt->entity->user_data;
    if (drop) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(drop->weapon_drop_actor);
        zpl_zero_item(drop);
    }
}
 
inline auto drop_frame_init(MafiaSDK::C_Human* _this, MafiaSDK::C_Drop_In_Weapon* drop, char* model_name) -> void {
    if (local_player.ped == _this) {
        auto item = (inventory_item*)&drop->GetInterface()->DropItem;
        local_player_weapondrop(item, model_name);
        mod_log(("Dropping gun: " + std::string(model_name)).c_str());
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(drop);
    }
}

inline auto drop_game_tick(mafia_weapon_drop* drop) -> void {
    drop->angle		+= 0.04f;
    drop->rotation	= { cos(drop->angle*0.1744f), 0.0, sin(drop->angle*0.1744f), 0.0 };
    MafiaSDK::I3D_Frame* drop_frame = *(MafiaSDK::I3D_Frame**)((DWORD)drop->weapon_drop_actor + 0x68);
    if(drop_frame) drop_frame->SetRot(drop->rotation);
}

inline auto drop_on_pickup(MafiaSDK::C_Drop_In_Weapon* drop) {

    for (u32 i = 0; i < network_context.max_entities; i++) {

        // skip local player 
        if (i == local_player.entity.id) continue;
        librg_entity *entity = librg_entity_fetch(&network_context, i);
        if (!entity || entity->type != TYPE_WEAPONDROP) continue;

        auto mafia_drop = (mafia_weapon_drop*)entity->user_data;
        if (mafia_drop->weapon_drop_actor == drop) {
            local_player_weaponpickup(entity);
        }
    }
}

#include "Game/Hooks/weapon_drop.hpp"
