#pragma once


namespace hooks
{
	//----------------------------------------------
	//C_Human::PoseSetPoseAimed() && C_Human::PoseSetPoseNormal
	//----------------------------------------------
	auto __fastcall PoseSetPoseAimed(MafiaSDK::C_Human* _this, Vector3D pose) -> int {
	
		if(_this == local_player.ped) 
			local_player.pose = EXPAND_VEC(pose);

		_this->PoseSetPoseAimed(pose);
		return 0;
	}

	auto __fastcall PoseSetPoseNormal(MafiaSDK::C_Human* _this, Vector3D pose) -> int {

		if(_this == local_player.ped)
			local_player.pose = EXPAND_VEC(pose);

		_this->PoseSetPoseNormal(pose);
		return 0;
	}

	//----------------------------------------------
	//C_Human::Do_Reload()
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Do_Reload_t)(void* _this);
	C_Human_Do_Reload_t human_do_reload_original = nullptr;

	bool __fastcall C_Human_Do_Reload(void* _this) {
		bool to_return = human_do_reload_original(_this);

		if(local_player.ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == local_player.ped) {
			local_player_reload();
		}

		return to_return;
	}

	//----------------------------------------------
	//C_Human::Do_Holster()
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Do_Holster_t)(void* _this);
	C_Human_Do_Holster_t human_do_holster_original = nullptr;

	bool __fastcall C_Human_Do_Holster(void* _this) {
		bool to_return = human_do_holster_original(_this);

		if(local_player.ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == local_player.ped) {
			local_player_holster();
		}

		return to_return;
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
	bool __fastcall OnHit(void* _this, DWORD edx, int type, Vector3D* unk1,Vector3D* unk2, Vector3D* unk3, float damage, MafiaSDK::C_Actor* attacker, unsigned long player_part, MafiaSDK::I3D_Frame* frame) {
		
		if (hit_hook_skip && (_this != local_player.ped)) return 0;

		bool ret_val = human_hit_original(_this, type, unk1, unk2, unk3, damage, attacker, player_part, frame);

		if (_this == local_player.ped) {
			local_player_hit(reinterpret_cast<MafiaSDK::C_Human*>(_this), type, unk1, unk2, unk3, damage, attacker, player_part);
			auto player_int = local_player.ped->GetInterface();
			bool is_alive = player_int->humanObject.entity.isActive;
			if (!is_alive && !local_player.dead) {
				local_player_died();
				local_player.dead = true;
			}
		}
		
		return ret_val;
	}
	
	//----------------------------------------------
	// C_human::Do_ThrowGrenade((S_vector &)) 
	//----------------------------------------------
	void C_Human_Do_ThrowGrenade(void* _this) {

		if(_this != nullptr) {
			Vector3D pos = *(Vector3D*)((DWORD)_this + 0x200);
			if(_this == local_player.ped) {
				local_player_throwgrenade(pos);
			}
		}
	}

	DWORD throw_grenade_jmp_back = 0x00583A68;
	DWORD c_human_person_anim = 0x00573E50;

	__declspec(naked) void ThrowGrenade() {
		__asm {
			pushad
				push esi
				call C_Human_Do_ThrowGrenade
				add esp, 0x4
			popad

			push 	ebx
			push    1
			push    ebx
			mov     ecx, esi
			mov     dword ptr [esi+74h], 0x0A3
			call    c_human_person_anim
			jmp throw_grenade_jmp_back
		}
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

	//----------------------------------------------
	//C_human::Use_Actor((C_actor *, int, int, int))
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Use_Actor_t)(void* _this, DWORD actor, int unk1, int unk2, int unk3);
	C_Human_Use_Actor_t human_use_actor_original = nullptr;
	bool __fastcall C_Human_Use_Actor(void* _this, DWORD edx, DWORD actor, int unk1, int unk2, int unk3) {

		if (_this == local_player.ped) {
			local_player_useactor(actor, unk1, unk2, unk3);
		}

		return human_use_actor_original(_this, actor, unk1, unk2, unk3);
	}

	//----------------------------------------------
	//C_human::Do_ThrowCocotFromCar(C_car *, int)
	//----------------------------------------------
	typedef bool(__thiscall* C_Human_Do_ThrowCocotFromCar_t)(void* _this, DWORD car, int seat);
	C_Human_Do_ThrowCocotFromCar_t human_do_throw_cocot_from_car_original = nullptr;
	bool __fastcall C_Human_Do_ThrowCocotFromCar(void* _this, DWORD edx, DWORD car, int seat) {

		if (_this == local_player.ped) {
			local_player_hijack(car, seat);
		}

		return human_do_throw_cocot_from_car_original(_this, car, seat);
	}

	//----------------------------------------------
	//C_car::Prepare_DropOut_Wheel
	//----------------------------------------------
	typedef bool(__fastcall* C_car_Prepare_DropOut_Wheel_t)(void* _this, int idx, const Vector3D& speed, const Vector3D* unk);
	C_car_Prepare_DropOut_Wheel_t car_prepare_dropout_wheel_original = nullptr;
	bool __fastcall C_car_Prepare_DropOut_Wheel(void* _this, int idx, const Vector3D& speed, const Vector3D* unk) {

		auto vehicle_ent = get_vehicle_from_base((void*)_this);
		if (!vehicle_ent) return false;

		zpl_vec3 send_speed = EXPAND_VEC(speed);
		zpl_vec3 send_unk = { 0.0f, 0.0f, 0.0f };

		if (unk) send_unk = EXPAND_VEC((*unk));

		librg_send(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, data, {
			librg_data_wu32(&data, vehicle_ent->id);
			librg_data_wu32(&data, idx);
			librg_data_wptr(&data, (void*)&send_speed, sizeof(zpl_vec3));
			librg_data_wptr(&data, (void*)&send_unk, sizeof(zpl_vec3));
		});

		return false;
	}

	//----------------------------------------------
	//C_car::Prepare_DropOut
	//----------------------------------------------
	typedef bool(__fastcall* C_car_Prepare_DropOut_t)(void* _this, int idx, const Vector3D& speed, const Vector3D* unk);
	C_car_Prepare_DropOut_Wheel_t car_prepare_dropout_original = nullptr;
	bool __fastcall C_car_Prepare_DropOut(void* _this, int idx, const Vector3D& speed, const Vector3D* unk) {

		auto vehicle_ent = get_vehicle_from_base((void*)_this);
		if (!vehicle_ent) return false;

		zpl_vec3 send_speed = EXPAND_VEC(speed);
		zpl_vec3 send_unk = { 0.0f, 0.0f, 0.0f };

		if (unk) send_unk = EXPAND_VEC((*unk));

		librg_send(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, data, {
			librg_data_wu32(&data, vehicle_ent->id);
			librg_data_wu32(&data, idx);
			librg_data_wptr(&data, (void*)&send_speed, sizeof(zpl_vec3));
			librg_data_wptr(&data, (void*)&send_unk, sizeof(zpl_vec3));
		});

		return false;
	}
	
	//----------------------------------------------
	//C_Vehicle::Deform((S_vector const &,S_vector const &,float,float,uint,S_vector const *))
	//----------------------------------------------s
	
	/*bool __fastcall C_Vehicle_Deform(void* _this, 
		DWORD edx, 	
		const Vector3D & pos, 
		const Vector3D & rot, 
		float unk1,
		float unk2,
		unsigned int unk3,
		Vector3D* unk4) {
		
		//lets debug arguments
		for (auto current_car : vehicle_deformations) {
			vehicle_deform_original((void*)current_car, pos, rot, unk1, unk2, unk3, NULL);
		}

		return vehicle_deform_original(_this, pos, rot, unk1, unk2, unk3, unk4);
	}*/

	/* 
	* Simple filtration of forbidden object types 
	*/
	using ObjTypes = MafiaSDK::C_Mission_Enum::ObjectTypes;
	std::vector<ObjTypes> forbidden_objects = {
		ObjTypes::Car,
		ObjTypes::Dog,
		ObjTypes::Enemy,
		ObjTypes::Pumpar,
		ObjTypes::Trolley
	};

	MafiaSDK::C_Actor* Scene_CreateActor_Filter(MafiaSDK::C_Mission_Enum::ObjectTypes type, DWORD frame) {
		
		for(auto forbidden_type : forbidden_objects) {
			if(type == forbidden_type) {
				//NOTE(DavoSK): Dont spawn actor but we need to call destructor of frame
				__asm {
					mov eax, frame
					push eax
					mov ecx, [eax]
					call dword ptr ds : [ecx]
				}
				return nullptr;
			}
		}
		
		return MafiaSDK::GetMission()->CreateActor(type);;
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

	/*
	* Draw distance hook
	*/
	float vd_min = 20.0f;
	float vd_max = 450.0f;
	float vd_value = GlobalConfig.view_distance > 0.0f ? GlobalConfig.view_distance : vd_max;

	void __declspec(naked)VD_Hook_1_0_ENG() {
		_asm {
			ADD[ESP], 0x2;
			FLD DWORD PTR DS : [ESP + 0x0B8];	
			FLD DWORD PTR DS : vd_min;			
			FCOMiP ST, ST(1);						
			JAE finaly;								
			FLD DWORD PTR DS : vd_max;				
			FCOMiP ST, ST(1);						
			JBE finaly;								
			FFREE ST;				
			FLD DWORD PTR DS : vd_value;							
		finaly:
			FSTP DWORD PTR DS : [ESP + 0x0B8];
			MOV ECX, DWORD PTR SS : [ESP + 0x0B8];
			RETN;
		}
	}
};

BOOL streamed_udate = FALSE;
void check_for_streamed(DWORD car) {
	auto veh = get_vehicle_from_base((void*)car);
	streamed_udate = veh && veh->flags & ENTITY_INTERPOLATED;
}

DWORD jump_back_update_pos = 0x004E5290;
__declspec(naked) void ChangeUpdateCarPos() {
	__asm {
		pushad
			push ebp
			call check_for_streamed
			add esp, 0x4
		popad

		cmp streamed_udate, 1
		je interpolated_update

		fld dword ptr ss : [esp + 0xC8] 
		fadd dword ptr ds : [esi]
		fstp dword ptr ds : [esi]
		fld dword ptr ss : [esp + 0xCC]
		fadd dword ptr ds : [esi + 0x4]
		fstp dword ptr ds : [esi + 0x4]
		fld dword ptr ss : [esp + 0xD0]
		fadd dword ptr ds : [esi + 0x8]
		fstp dword ptr ds : [esi + 0x8]
		jmp jump_back_update_pos

	interpolated_update:
		
		fld dword ptr ds : [esi]
		fstp dword ptr ds : [esi]
		fld dword ptr ds : [esi + 0x4]
		fstp dword ptr ds : [esi + 0x4]
		fld dword ptr ds : [esi + 0x8]
		fstp dword ptr ds : [esi + 0x8]
		jmp jump_back_update_pos
	}
}

DWORD jump_back_update_pos_coll = 0x004E5ECD;
__declspec(naked) void ChangeUpdateCarPosCollision() {
	__asm {
		pushad
			push ebp
			call check_for_streamed
			add esp, 0x4
		popad

		cmp streamed_udate, 1
		je interpolated_update

		fld dword ptr ss : [esp + 0x138]
		fadd dword ptr ds : [esi]
		mov al, byte ptr ss : [esp + 0x9b]
		test al, al
		fstp dword ptr ds : [esi]
		fld dword ptr ss : [esp + 0x13c]
		fadd dword ptr ds : [esi + 0x4]
		fstp dword ptr ds : [esi + 0x4]
		fld dword ptr ss : [esp + 0x140]
		fadd dword ptr ds : [esi + 0x8]
		fstp dword ptr ds : [esi + 0x8]

		jmp jump_back_update_pos_coll
		interpolated_update :

		fld dword ptr ds : [esi]
		mov al, byte ptr ss : [esp + 0x9b]
		test al, al
		fstp dword ptr ds : [esi]
		fld dword ptr ds : [esi + 0x4]
		fstp dword ptr ds : [esi + 0x4]
		fld dword ptr ds : [esi + 0x8]
		fstp dword ptr ds : [esi + 0x8]
		jmp jump_back_update_pos_coll
	}
}

inline auto local_player_init() {
	
	//Scene
	MemoryPatcher::InstallJmpHook(0x00544AFF, (DWORD)&hooks::Scene_CreateActor);
	
	auto vd_value1 = hooks::vd_value - 50.0f;
	auto vd_value2 = hooks::vd_value - 20.0f;
	MemoryPatcher::PatchAddress(0x00402201 + 0x4, (BYTE*)&vd_value1, 4);
	MemoryPatcher::PatchAddress(0x00402209 + 0x4, (BYTE*)&vd_value2, 4);
	MemoryPatcher::InstallCallHook(0x0054192E, (DWORD)hooks::VD_Hook_1_0_ENG);
	MemoryPatcher::InstallNopPatch(0x0054192E + 0x5, 0x2);

	//Human
	MemoryPatcher::InstallCallHook(0x00593D46, (DWORD)&hooks::PoseSetPoseAimed);
	MemoryPatcher::InstallCallHook(0x00593D65, (DWORD)&hooks::PoseSetPoseNormal);
	MemoryPatcher::InstallJmpHook(0x00591416, (DWORD)&hooks::DoShoot);
	MemoryPatcher::InstallJmpHook(0x00583A56, (DWORD)&hooks::ThrowGrenade);

	hooks::select_by_id_original = reinterpret_cast<hooks::G_Inventory_SelectByID_t>(
		DetourFunction((PBYTE)0x006081D0, (PBYTE)&hooks::SelectByID)
	);

	hooks::human_hit_original = reinterpret_cast<hooks::C_Human_Hit_t>(
		DetourFunction((PBYTE)0x005762A0, (PBYTE)&hooks::OnHit)
	);

	hooks::human_do_reload_original = reinterpret_cast<hooks::C_Human_Do_Reload_t>(
		DetourFunction((PBYTE)0x00585B40, (PBYTE)&hooks::C_Human_Do_Reload)
	);

	hooks::human_do_holster_original = reinterpret_cast<hooks::C_Human_Do_Holster_t>( 
		DetourFunction((PBYTE)0x00585C60, (PBYTE)&hooks::C_Human_Do_Holster)
	);

	hooks::human_use_actor_original = reinterpret_cast<hooks::C_Human_Use_Actor_t>(
		DetourFunction((PBYTE)0x00582180, (PBYTE)&hooks::C_Human_Use_Actor)
	);

	hooks::human_do_throw_cocot_from_car_original = reinterpret_cast<hooks::C_Human_Do_ThrowCocotFromCar_t>(
		DetourFunction((PBYTE)0x00587D70, (PBYTE)&hooks::C_Human_Do_ThrowCocotFromCar)
	);

	// Vehicle
	MemoryPatcher::InstallJmpHook(0x004E526B, (DWORD)&ChangeUpdateCarPos);
	MemoryPatcher::InstallJmpHook(0x004E5E9F, (DWORD)&ChangeUpdateCarPosCollision);

	hooks::car_prepare_dropout_wheel_original = reinterpret_cast<hooks::C_car_Prepare_DropOut_Wheel_t>(
		DetourFunction((PBYTE)0x00426DD0, (PBYTE)&hooks::C_car_Prepare_DropOut_Wheel)
	);

	hooks::car_prepare_dropout_original = reinterpret_cast<hooks::C_car_Prepare_DropOut_t>(
		DetourFunction((PBYTE)0x00426EC0, (PBYTE)&hooks::C_car_Prepare_DropOut)
	);
}