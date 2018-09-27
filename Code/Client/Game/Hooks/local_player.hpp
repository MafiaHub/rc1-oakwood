#pragma once


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
	// C_human::Do_ThrowGrenade((S_vector &)) 
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Do_ThrowGrenade_t)(void* _this, const Vector3D & pos);
	C_Human_Do_ThrowGrenade_t do_throw_grenade_original = nullptr;

	bool __fastcall C_Human_Do_ThrowGrenade(void* _this, const Vector3D & pos) {

		if(_this == local_player.ped) {
			local_player_throwgrenade(pos);
		}

		return do_throw_grenade_original(_this, pos);
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

	MafiaSDK::C_Actor* Scene_CreateActor_Filter(MafiaSDK::C_Mission_Enum::ObjectTypes type, DWORD frame) {
		if (type != MafiaSDK::C_Mission_Enum::Car &&
			type != MafiaSDK::C_Mission_Enum::Dog &&
			type != MafiaSDK::C_Mission_Enum::Enemy) {
			return MafiaSDK::GetMission()->CreateActor(type);
		}

		//NOTE(DavoSK): Dont spawn actor but we need to call destructor of frame
		__asm {
			mov eax, frame
			push eax
			mov ecx, [eax]
			call dword ptr ds : [ecx]
		}

		return nullptr;
	}

	DWORD filter_create_actor_back = 0x00544B07;
	__declspec(naked) void Scene_CreateActor() {
		__asm {
			push edi
			push eax
			call Scene_CreateActor_Filter
			add esp, 0x8

			jmp filter_create_actor_back
		}
	}
};

inline auto local_player_init() {
	MemoryPatcher::InstallCallHook(0x00593D46, (DWORD)&hooks::PoseSetPoseAimed);
	MemoryPatcher::InstallCallHook(0x00593D65, (DWORD)&hooks::PoseSetPoseNormal);
	MemoryPatcher::InstallJmpHook(0x00591416, (DWORD)&hooks::DoShoot);
	MemoryPatcher::InstallJmpHook(0x00544AFF, (DWORD)&hooks::Scene_CreateActor);

	hooks::select_by_id_original = reinterpret_cast<hooks::G_Inventory_SelectByID_t>(
		DetourFunction((PBYTE)0x006081D0, (PBYTE)&hooks::SelectByID)
	);

	hooks::human_hit_original = reinterpret_cast<hooks::C_Human_Hit_t>(
		DetourFunction((PBYTE)0x005762A0, (PBYTE)&hooks::OnHit)
	);

	hooks::do_throw_grenade_original = reinterpret_cast<hooks::C_Human_Do_ThrowGrenade_t>(
		DetourFunction((PBYTE)0x00583F40, (PBYTE)&hooks::C_Human_Do_ThrowGrenade)
	);
}