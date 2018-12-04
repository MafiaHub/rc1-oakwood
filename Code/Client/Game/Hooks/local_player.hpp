#pragma once


namespace hooks
{
    //----------------------------------------------
    //C_Human::PoseSetPoseAimed() && C_Human::PoseSetPoseNormal
    //----------------------------------------------
    auto __fastcall PoseSetPoseAimed(MafiaSDK::C_Human* _this, S_vector pose) -> int {
    
        if(_this == local_player.ped) 
            local_player.pose = EXPAND_VEC(pose);

        _this->PoseSetPoseAimed(pose);
        return 0;
    }

    auto __fastcall PoseSetPoseNormal(MafiaSDK::C_Human* _this, S_vector pose) -> int {

        if(_this == local_player.ped)
            local_player.pose = EXPAND_VEC(pose);

        _this->PoseSetPoseNormal(pose);
        return 0;
    }

    //----------------------------------------------
    //C_Human::Intern_FromCar()
    //----------------------------------------------
    typedef bool(__thiscall* C_Human_Itern_FromCar_t)(void* _this, MafiaSDK::I3D_Frame* frame);
    C_Human_Itern_FromCar_t human_intern_fromcar_original = nullptr;
    
    bool __fastcall C_Human_Intern_FromCar(void* _this, DWORD edx, MafiaSDK::I3D_Frame* frame) {
        if (reinterpret_cast<MafiaSDK::C_Player*>(_this) == local_player.ped) {
            local_player_fromcar();
        }

        return false;
    }

    //----------------------------------------------
    //C_Human::Do_Reload()
    //----------------------------------------------
    typedef bool(__thiscall* C_Human_Do_Reload_t)(void* _this);
    C_Human_Do_Reload_t human_do_reload_original = nullptr;

    bool __fastcall C_Human_Do_Reload(void* _this) {
        bool to_return = human_do_reload_original(_this);

        if (local_player.ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == local_player.ped) {
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

        if (local_player.ped && reinterpret_cast<MafiaSDK::C_Player*>(_this) == local_player.ped) {
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
    typedef bool(__thiscall* C_Human_Hit_t)(void* _this, int hitType, S_vector* unk1, S_vector* unk2, S_vector* unk3, float damage, MafiaSDK::C_Actor* atacker, unsigned long hittedPart, MafiaSDK::I3D_Frame* targetFrame);
    C_Human_Hit_t human_hit_original = nullptr;
    bool __fastcall OnHit(void* _this, DWORD edx, int type, S_vector* unk1,S_vector* unk2, S_vector* unk3, float damage, MafiaSDK::C_Actor* attacker, unsigned long player_part, MafiaSDK::I3D_Frame* frame) {
        
        if (hit_hook_skip && (_this != local_player.ped)) return 0;
        auto victim = reinterpret_cast<MafiaSDK::C_Human*>(_this);

        float current_health = victim->GetInterface()->health;
        bool ret_val = human_hit_original(_this, type, unk1, unk2, unk3, damage, attacker, player_part, frame);
        float new_health = victim->GetInterface()->health;

        damage = (current_health - new_health);

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
            S_vector pos = *(S_vector*)((DWORD)_this + 0x200);
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
    void OnDoShoot(S_vector* pos, MafiaSDK::C_Player* player) {

        player->Do_Shoot(1, *pos);

        if (player == local_player.ped)
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
    bool __fastcall C_Human_Use_Actor(void* _this, DWORD edx, DWORD actor, int action, int unk2, int unk3) {
        
        bool return_val = human_use_actor_original(_this, actor, action, unk2, unk3);
        if (_this == local_player.ped) {
            auto ped_interface = ((MafiaSDK::C_Human*)_this)->GetInterface();

            /* 
            * We send use actor only when we entering car or exiting
            * If player forceexit carLeavingOrEntering is NULL, thats why we check if we are exiting by action
            */
            if(ped_interface->carLeavingOrEntering || action == 2)
                local_player_useactor(actor, action, unk2, unk3);
        }

        return return_val;
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
}

inline auto local_player_init() {

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
}
