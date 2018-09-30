#pragma once

struct local_player_data {
	local_player_data() :
		ped(nullptr), 
		dead(false)  {
	}
	MafiaSDK::C_Player* ped;
	librg_entity_t entity;
	zpl_vec3 pose;
	bool dead;
} local_player;

inline auto get_player_from_base(void* base) -> librg_entity_t* {

	for (u32 i = 0; i < network_context.max_entities; i++) {
		librg_entity_t *entity = librg_entity_fetch(&network_context, i);
		if (!entity || entity->type != TYPE_PLAYER || !entity->user_data) continue;	
		auto pl = (mafia_player*)(entity->user_data);
		if (base == pl->ped) return entity;
	}
	return nullptr;
}

/* 
* todo add reason killer and so one ...
*/
inline auto local_player_died() {
	librg_send(&network_context, NETWORK_PLAYER_DIE, data, {});
}

inline auto local_player_hit(
	MafiaSDK::C_Human* victim,
	DWORD hit_type, 
	const Vector3D* unk1, 
	const Vector3D* unk2, 
	const Vector3D* unk3,
	float damage, 
	MafiaSDK::C_Actor* atacker, 
	unsigned int player_part) -> void {

	auto victim_ent = get_player_from_base(victim);

	if (!victim_ent) return;
	
	librg_send(&network_context, NETWORK_PLAYER_HIT, data, {
		librg_data_went(&data, victim_ent->id);
		librg_data_wu32(&data, hit_type);
		librg_data_wptr(&data, (void*)unk1, sizeof(zpl_vec3));
		librg_data_wptr(&data, (void*)unk2, sizeof(zpl_vec3));
		librg_data_wptr(&data, (void*)unk3, sizeof(zpl_vec3));
		librg_data_wf32(&data, damage);
		librg_data_wu32(&data, player_part);
	});
}

inline auto local_player_shoot(Vector3D pos) -> void {

	librg_send(&network_context, NETWORK_PLAYER_SHOOT, data, {
		librg_data_wptr(&data, &pos, sizeof(Vector3D));
	});
}

inline auto local_player_weapondrop(inventory_item* item, char* model) -> void {

	char wep_model[32];
	strcpy(wep_model, model);

	librg_send(&network_context, NETWORK_PLAYER_WEAPON_DROP, data, {
		librg_data_wptr(&data, item, sizeof(inventory_item));
		librg_data_wptr(&data, wep_model, sizeof(char) * 32);
	});
}

inline auto local_player_weaponchange(u32 index) -> void {

	librg_send(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, data, {
		librg_data_wu32(&data, index);
	});
}

//TODO send inventory on each message related with weapons !
inline auto local_player_reload() -> void {
	librg_send(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, data, {});
}

inline auto local_player_holster() -> void {
	librg_send(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, data, {});
}

inline auto local_player_weaponpickup(librg_entity_t* item_entity) -> void {

	librg_send(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, data, {
		librg_data_went(&data, item_entity->id);
	});

	//force weapon change for inventory sync (weird rules wen wep is picked up, some weapons have priority and sheet)
	auto mafia_drop = (mafia_weapon_drop*)item_entity->user_data;
	local_player.ped->G_Inventory_SelectByID(mafia_drop->weapon.weaponId);
	local_player.ped->Do_ChangeWeapon(0, 0);
}

inline auto local_player_throwgrenade(const Vector3D & pos) {
	Vector3D vec_copy = pos;
	librg_send(&network_context, NETWORK_PLAYER_THROW_GRENADE, data, {
		librg_data_wptr(&data, &vec_copy, sizeof(Vector3D));
	});
}

#include "Game/Hooks/local_player.hpp"