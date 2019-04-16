//----------------------------------------------
//C_game::NewShoot((C_actor*, S_vector const&, S_vector const&, float, int, I3D_frame*, int))
//----------------------------------------------
typedef DWORD(__thiscall* C_game__NewShoot_t)(void* _this, MafiaSDK::C_Actor* arg1, S_vector arg2, S_vector arg3, float arg4, int arg5, MafiaSDK::I3D_Frame* arg6, int arg7);
C_game__NewShoot_t game_shoot_original = nullptr;
DWORD __fastcall C_game__NewShoot(void* _this, DWORD edx, MafiaSDK::C_Actor* arg1, S_vector arg2, S_vector arg3, float arg4, int arg5, MafiaSDK::I3D_Frame* arg6, int arg7) {
    S_vector pos = arg2;
    S_vector dir = arg3;

    auto local_ped = get_local_ped();
    // if shooter is local_player we want to send shoot message
    if (arg1 == local_ped && local_shoot_data.player_base == (DWORD)arg1) {
        local_shoot_data.pos = arg2;
        local_shoot_data.dir = arg3;
        modules::player::shoot(local_shoot_data);
    }
    else if (local_ped != arg1 && arg1 != nullptr) {
        auto shoot_data = shoot_queue[arg1];
        pos = shoot_data.pos;
        dir = shoot_data.dir;
    }

    return game_shoot_original(_this, arg1, pos, dir, arg4, arg5, arg6, arg7);
}


//----------------------------------------------
//C_Human::PoseSetPoseAimed() && C_Human::PoseSetPoseNormal
//----------------------------------------------
auto __fastcall PoseSetPoseAimed(MafiaSDK::C_Human* _this, S_vector pose) -> int {

	if(_this == get_local_ped()) 
		local_player.pose = EXPAND_VEC(pose);

	_this->PoseSetPoseAimed(pose);
	return 0;
}

auto __fastcall PoseSetPoseNormal(MafiaSDK::C_Human* _this, S_vector pose) -> int {

	if(_this == get_local_ped())
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
	auto ped = modules::player::get_local_ped();
	if (ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == ped) {
		modules::player::reload();
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
	auto ped = modules::player::get_local_ped();
	if (ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == ped){
		modules::player::holster();
	}

	return to_return;
}

//----------------------------------------------
//G_Inventory::SelectByID(id, Vec*)
//----------------------------------------------
typedef bool(__thiscall* G_Inventory_SelectByID_t)(void* _this, u32 index, void* items_vec);
G_Inventory_SelectByID_t select_by_id_original = nullptr;

bool __fastcall SelectByID(void* _this, DWORD edx, u32 index, void* items_vec) {

	auto player = modules::player::get_local_player();
	if (player && player->ped) {
		void* player_inv = (void*)player->ped->GetInventory();
		
		if (_this == player_inv) {
			modules::player::weaponchange(index);
			player->current_weapon_id = index;
		}
	}

	return select_by_id_original(_this, index, items_vec);
}

//----------------------------------------------
//C_Human:Hit(E_hit_type, S_vector const &, S_vector const &, S_vector const &, float, C_actor *, unsigned int, I3D_frame *)
//----------------------------------------------
typedef bool(__thiscall* C_Human_Hit_t)(void* _this, int hitType, S_vector* unk1, S_vector* unk2, S_vector* unk3, float damage, MafiaSDK::C_Actor* atacker, unsigned long hittedPart, MafiaSDK::I3D_Frame* targetFrame);
C_Human_Hit_t human_hit_original = nullptr;
bool __fastcall OnHit(void* _this, DWORD edx, int type, S_vector* unk1,S_vector* unk2, S_vector* unk3, float damage, MafiaSDK::C_Actor* attacker, unsigned long player_part, MafiaSDK::I3D_Frame* frame) {
	
	auto ped = modules::player::get_local_ped();
	if (hit_hook_skip && (_this != ped)) return 0;
	auto victim = reinterpret_cast<MafiaSDK::C_Human*>(_this);

	float current_health = victim->GetInterface()->health;
	bool ret_val = human_hit_original(_this, type, unk1, unk2, unk3, damage, attacker, player_part, frame);
	float new_health = victim->GetInterface()->health;

	damage = (current_health - new_health);

	if (_this == ped) {
        modules::player::hit(reinterpret_cast<MafiaSDK::C_Human*>(_this), type, unk1, unk2, unk3, damage, attacker, player_part);
		auto player_int = ped->GetInterface();
		bool is_alive = player_int->humanObject.entity.isActive;

		if (!is_alive)
			modules::player::died();
	}
	
	return ret_val;
}

//----------------------------------------------
// C_human::Do_ThrowGrenade((S_vector &)) 
//----------------------------------------------
void C_Human_Do_ThrowGrenade(void* _this) {

	if(_this != nullptr) {
		S_vector pos = *(S_vector*)((DWORD)_this + 0x200);
		if(_this == modules::player::get_local_ped()) {
            modules::player::throwgrenade(pos);
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
typedef bool(__thiscall* C_Human_Do_Shoot_t)(void* _this, BOOL do_shoot, S_vector* pos);
C_Human_Do_Shoot_t human_do_shoot_original = nullptr;

BOOL __fastcall HumanDoShoot(void*_this, DWORD edx, BOOL do_shoot, S_vector* pos) {

	if (do_shoot && pos && _this == modules::player::get_local_ped()) {
        local_shoot_data.screen_coord = *pos;
        local_shoot_data.player_base = (DWORD)_this;
	}
	return human_do_shoot_original(_this, do_shoot, pos);
}

//----------------------
// Sink & Fall hook
//----------------------
void Player__OnSink()
{
    modules::player::died();
}

__declspec(naked) void PlayerOnSink() {
	__asm {
		pushad
			call Player__OnSink
		popad

		// 0x0057BAB1
		mov eax, 0x005A545D
		jmp eax
	}
}

__declspec(naked) void PlayerFall() {
	__asm {
		pushad
			call Player__OnSink
		popad

		// 0x0057BAB1
		mov eax, 0x0057A7F8
		jmp eax
	}
}

//----------------------------------------------
//C_human::Use_Actor((C_actor *, int, int, int))
//----------------------------------------------
typedef bool(__thiscall* C_Human_Use_Actor_t)(void* _this, DWORD actor, int unk1, int unk2, int unk3);
C_Human_Use_Actor_t human_use_actor_original = nullptr;
bool __fastcall C_Human_Use_Actor(void* _this, DWORD edx, DWORD actor, int action, int unk2, int unk3) {
	
	bool return_val = human_use_actor_original(_this, actor, action, unk2, unk3);
	if (_this == modules::player::get_local_ped()) {
		auto ped_interface = ((MafiaSDK::C_Human*)_this)->GetInterface();
		
		/* 
		* We send use actor only when we entering car or exiting
		* If player forceexit carLeavingOrEntering is NULL, thats why we check if we are exiting by action
		*/
		if(ped_interface->carLeavingOrEntering || action == 2)
            modules::player::useactor(actor, action, unk2, unk3);
	}

	return return_val;
}

//----------------------------------------------
//C_human::Do_ThrowCocotFromCar(C_car *, int)
//----------------------------------------------
typedef bool(__thiscall* C_Human_Do_ThrowCocotFromCar_t)(void* _this, DWORD car, int seat);
C_Human_Do_ThrowCocotFromCar_t human_do_throw_cocot_from_car_original = nullptr;
bool __fastcall C_Human_Do_ThrowCocotFromCar(void* _this, DWORD edx, DWORD car, int seat) {

	if (_this == modules::player::get_local_ped()) {
        modules::player::hijack(car, seat);
	}

	return human_do_throw_cocot_from_car_original(_this, car, seat);
}

//----------------------------------------------
//C_Game::RemoveTemporaryActor_t
//----------------------------------------------
typedef void(__thiscall* RemoveTemporaryActor_t)(void* _this, void* actor);
RemoveTemporaryActor_t remove_temporary_actor_original = nullptr;

void __fastcall RemoveTemporaryActor(void* _this, DWORD edx, void* actor) { 
    modules::player::remove_temporary_actor(actor);
	remove_temporary_actor_original(_this, actor);
}

//----------------------------------------------
//C_Indicators Render game map hook
//----------------------------------------------
DWORD player_cursor_back = 0x005FFF8B;
__declspec(naked) void PlayerCursorHook() {
	__asm {
		LEA ECX, DWORD PTR SS : [ESP + 0x8C]
		pushad
		push ecx
		call gamemap::draw_player_cursor
		add esp, 0x4
		popad

		JMP player_cursor_back
	}
}

//----------------------------------------------
//Proper exit handling
//----------------------------------------------
void OnGameExit() {
    if (librg_is_connected(&network_context)) {
        librg_network_stop(&network_context);
    }
    exit(0);
}

__declspec(naked) void OnGameExitHook() {
	__asm {
		pushad
		call OnGameExit
		popad
		retn
	}
}

bool is_ped_local(DWORD esi) {
    return (DWORD)modules::player::get_local_ped() == esi;
}

DWORD shooting_aimcarback = 0x0057B5CF;
__declspec(naked) void ShootingAimCar() {
    __asm {
        push esi
        call is_ped_local
        add esp, 0x4
        cmp eax, 0 
        je skip

        fstp    dword ptr ds : [esi + 0x5F4]
        skip:
        fld     dword ptr ds :[esi + 0x5F0]
        
        jmp shooting_aimcarback
    }
}

inline auto init() {
	//Engine exit hook ( for quiting window )
    MemoryPatcher::InstallJmpHook(0x1008E8B0, (DWORD)&OnGameExitHook);

    //Game exit hook ( for exiting from menu )
    MemoryPatcher::InstallJmpHook(0x00612485, (DWORD)&OnGameExitHook);
    
    //G_Indicators blips rendering hook 
    MemoryPatcher::InstallJmpHook(0x005FFF77, (DWORD)&PlayerCursorHook);

    //Human
    MemoryPatcher::InstallCallHook(0x00593D46, (DWORD)&PoseSetPoseAimed);
    MemoryPatcher::InstallCallHook(0x00593D65, (DWORD)&PoseSetPoseNormal); 
    MemoryPatcher::InstallJmpHook(0x00583A56, (DWORD)&ThrowGrenade);
    MemoryPatcher::InstallJmpHook(0x005A543B, (DWORD)&PlayerOnSink);

    //Disable dealocation second remove actor
    MemoryPatcher::InstallJmpHook(0x005A7F44, 0x005A7F4B);

    //Shooting aim 
    MemoryPatcher::InstallJmpHook(0x0057B5C3, (DWORD)ShootingAimCar);

    game_shoot_original = reinterpret_cast<C_game__NewShoot_t>(
        DetourFunction((PBYTE)0x005A84A0, (PBYTE)&C_game__NewShoot)
    );

    human_do_shoot_original = reinterpret_cast<C_Human_Do_Shoot_t>(
        DetourFunction((PBYTE)0x00583590, (PBYTE)&HumanDoShoot)
    );
    
    remove_temporary_actor_original = reinterpret_cast<RemoveTemporaryActor_t>(
        DetourFunction((PBYTE)0x005A79A0, (PBYTE)&RemoveTemporaryActor)
    );

    select_by_id_original = reinterpret_cast<G_Inventory_SelectByID_t>(
        DetourFunction((PBYTE)0x006081D0, (PBYTE)&SelectByID)
    );

    human_hit_original = reinterpret_cast<C_Human_Hit_t>(
        DetourFunction((PBYTE)0x005762A0, (PBYTE)&OnHit)
    );

    human_do_reload_original = reinterpret_cast<C_Human_Do_Reload_t>(
        DetourFunction((PBYTE)0x00585B40, (PBYTE)&C_Human_Do_Reload)
    );

    human_do_holster_original = reinterpret_cast<C_Human_Do_Holster_t>(
        DetourFunction((PBYTE)0x00585C60, (PBYTE)&C_Human_Do_Holster)
    );

    human_use_actor_original = reinterpret_cast<C_Human_Use_Actor_t>(
        DetourFunction((PBYTE)0x00582180, (PBYTE)&C_Human_Use_Actor)
    );

    human_do_throw_cocot_from_car_original = reinterpret_cast<C_Human_Do_ThrowCocotFromCar_t>(
        DetourFunction((PBYTE)0x00587D70, (PBYTE)&C_Human_Do_ThrowCocotFromCar)
    );
}
