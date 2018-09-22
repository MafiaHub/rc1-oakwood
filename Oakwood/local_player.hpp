#pragma once
struct local_player_data {
	local_player_data() :
		ped(nullptr), 
		dead(false)  {
	}
	MafiaSDK::C_Player* ped;
	librg_entity_t entity;
	zplm_vec3 pose;
	bool dead;
} local_player;

inline auto get_player_from_base(void* base) -> librg_entity_t* {

	for (u32 i = 0; i < ctx.max_entities; i++) {
		librg_entity_t *entity = librg_entity_fetch(&ctx, i);
		if (!entity || entity->type != TYPE_PLAYER || !entity->user_data) continue;	
		auto pl = (mafia_player*)(entity->user_data);
		if (base == pl->ped) return entity;
	}
	return nullptr;
}

/* 
* todo add reason killer and so one ...
*/
inline auto local_player_died() -> void {
	librg_send(&ctx, NETWORK_PLAYER_DIE, data, {});
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
	librg_send(&ctx, NETWORK_PLAYER_HIT, data, {
		librg_data_went(&data, victim_ent->id);
		librg_data_wu32(&data, hit_type);
		librg_data_wptr(&data, (void*)unk1, sizeof(zplm_vec3));
		librg_data_wptr(&data, (void*)unk2, sizeof(zplm_vec3));
		librg_data_wptr(&data, (void*)unk3, sizeof(zplm_vec3));
		librg_data_wf32(&data, damage);
		librg_data_wu32(&data, player_part);
	});
}

inline auto local_player_shoot(Vector3D pos) -> void {

	librg_send(&ctx, NETWORK_PLAYER_SHOOT, data, {
		librg_data_wptr(&data, &pos, sizeof(Vector3D));
	});
}

inline auto local_player_weapondrop(inventory_item* item, char* model) -> void {

	char wep_model[32];
	strcpy(wep_model, model);

	librg_send(&ctx, NETWORK_PLAYER_WEAPON_DROP, data, {
		librg_data_wptr(&data, item, sizeof(inventory_item));
		librg_data_wptr(&data, wep_model, sizeof(char) * 32);
	});
}

inline auto local_player_weaponchange(u32 index) -> void {

	librg_send(&ctx, NETWORK_PLAYER_WEAPON_CHANGE, data, {
		librg_data_wu32(&data, index);
	});
}

inline auto local_player_weaponpickup(librg_entity_t* item_entity) -> void {

	librg_send(&ctx, NETWORK_PLAYER_WEAPON_PICKUP, data, {
		librg_data_went(&data, item_entity->id);
	});

	//force weapon change for inventory sync (weird rules wen wep is picked up, some weapons have priority and sheet)
	auto mafia_drop = (mafia_weapon_drop*)item_entity->user_data;
	local_player.ped->G_Inventory_SelectByID(mafia_drop->weapon.weaponId);
	local_player.ped->Do_ChangeWeapon(0, 0);
}

namespace hooks
{
	auto __fastcall PoseSetPoseAimed(MafiaSDK::C_Human* _this, Vector3D pose) -> int {
		local_player.pose = EXPAND_VEC(pose);
		_this->PoseSetPoseAimed(pose);
		return 0;
	}

	auto __fastcall PoseSetPoseNormal(MafiaSDK::C_Human* _this, Vector3D pose) -> int {
		local_player.pose = EXPAND_VEC(pose);
		_this->PoseSetPoseNormal(pose);
		return 0;
	}

	//----------------------------------------------
	//G_Inventory::SelectByID(id, Vec*)
	//----------------------------------------------
	typedef bool(__thiscall* G_Inventory_SelectByID_t)(void* _this, u32 index, void* items_vec);
	G_Inventory_SelectByID_t select_by_id_original = nullptr;

	bool __fastcall SelectByID(void* _this, DWORD edx, u32 index, void* items_vec) {

		void* player_inv;
		__asm {
			mov eax, local_player
			lea eax, dword ptr ds : [eax + 0x480]
			mov player_inv, eax
		}

		if (_this == player_inv)
			local_player_weaponchange(index);

		auto player = (mafia_player*)local_player.entity.user_data;
		player->current_weapon_id = index;
		return select_by_id_original(_this, index, items_vec);
	}

	//----------------------------------------------
	//C_Human:Hit(E_hit_type, S_vector const &, S_vector const &, S_vector const &, float, C_actor *, unsigned int, I3D_frame *)
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Hit_t)(void* _this, int hitType, Vector3D* unk1, Vector3D* unk2, Vector3D* unk3, float damage, MafiaSDK::C_Actor* atacker, unsigned long hittedPart, MafiaSDK::I3D_Frame* targetFrame);
	C_Human_Hit_t human_hit_original = nullptr;
	bool __fastcall OnHit(void* _this, DWORD edx, int type, Vector3D* unk1,Vector3D* unk2, Vector3D* unk3, float damage, MafiaSDK::C_Actor* atacker, unsigned long player_part, MafiaSDK::I3D_Frame* frame) {
		
		if (atacker == local_player.ped) {
			local_player_hit(reinterpret_cast<MafiaSDK::C_Human*>(_this), type, unk1, unk2, unk3, damage, atacker, player_part);
		}

		if (hit_hook_skip) return 0;

		bool ret_val = human_hit_original(_this, type, unk1, unk2, unk3, damage, atacker, player_part, frame);
		if (_this == local_player.ped) {
			auto player_int = local_player.ped->GetInterface();
			bool is_alive = player_int->humanObject.entity.isActive;
			if (!is_alive && !local_player.dead) {
				local_player_died();
				local_player.dead = true;
				MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(true, 500, 0xFFFFFF);
			}
		}
		
		return ret_val;
	}
	
	//----------------------------------------------
	//CHuman::DoSoot(const S_Vector) custom jmp hook
	//----------------------------------------------
	void OnDoShoot(Vector3D* pos, MafiaSDK::C_Player* player) {

		player->Do_Shoot(1, *pos);

		if(player == local_player.ped)
			local_player_shoot(*pos);
	}

	DWORD shoot_fix_jmp_back = 0x00591424;
	__declspec(naked) void DoShoot() {
		__asm {
			LEA ECX, DWORD PTR SS : [ESP + 0x38]
			pushad
			push esi
			push ecx
			call OnDoShoot
			add esp, 0x8
			popad

			jmp shoot_fix_jmp_back
		}
	}
};

inline auto local_player_init() -> void {

	MemoryPatcher::InstallCallHook(0x00593D46, (DWORD)&hooks::PoseSetPoseAimed);
	MemoryPatcher::InstallCallHook(0x00593D65, (DWORD)&hooks::PoseSetPoseNormal);
	MemoryPatcher::InstallJmpHook(0x00591416, (DWORD)&hooks::DoShoot);

	hooks::select_by_id_original = reinterpret_cast<hooks::G_Inventory_SelectByID_t>(
		DetourFunction((PBYTE)0x006081D0, (PBYTE)&hooks::SelectByID)
	);

	hooks::human_hit_original = reinterpret_cast<hooks::C_Human_Hit_t>(
		DetourFunction((PBYTE)0x005762A0, (PBYTE)&hooks::OnHit)
	);
}