#pragma once

inline auto drop_entitycreate(librg_event_t* evnt) -> void {
	auto drop = new mafia_weapon_drop;
	librg_data_rptr(evnt->data, drop->model, sizeof(char) * 32);
	librg_data_rptr(evnt->data, &drop->weapon, sizeof(inventory_item));

	auto drop_frame = new MafiaSDK::I3D_Frame();
	Vector3D default_scale = { 1.5f, 1.5f, 1.5f };
	Vector3D default_pos = EXPAND_VEC(evnt->entity->position);
	drop_frame->SetName("testing_drop");
	drop_frame->LoadModel(drop->model);
	drop_frame->SetScale(default_scale);
	drop_frame->SetPos(default_pos);

	auto weapon_drop = new MafiaSDK::C_Drop_In_Weapon;
	weapon_drop->Init(drop_frame);
	weapon_drop->GetInterface()->DropItem = { drop->weapon.weaponId, drop->weapon.ammoLoaded, drop->weapon.ammoHidden, NULL };
	weapon_drop->SetActive(1);
	drop->weapon_drop_actor = weapon_drop;
	MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(weapon_drop);
	evnt->entity->user_data = drop;
}

inline auto drop_entityremove(librg_event_t* evnt) -> void {
	auto drop = (mafia_weapon_drop *)evnt->entity->user_data;
	if (drop) {
		MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(drop->weapon_drop_actor);
	}
}
 
inline auto drop_frame_init(MafiaSDK::C_Human* _this, MafiaSDK::C_Drop_In_Weapon* drop, char* model_name) -> void {
	if (local_player.ped == _this) {
		auto item = (inventory_item*)&drop->GetInterface()->DropItem;
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
		librg_entity_t *entity = librg_entity_fetch(&network_context, i);
		if (!entity || entity->type != TYPE_WEAPONDROP) continue;

		auto mafia_drop = (mafia_weapon_drop*)entity->user_data;
		if (mafia_drop->weapon_drop_actor == drop) {
			local_player_weaponpickup(entity);
		}
	}
}

#include "Game/Hooks/weapon_drop.hpp"