#pragma once

namespace hooks
{
    
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
    //C_car::CarExplosion
    //----------------------------------------------
    typedef bool(__thiscall* C_car_CarExplosion_t)(void* _this, unsigned int tick);
    C_car_CarExplosion_t c_car_carexplosion_original = nullptr;

    bool __fastcall C_car_CarExplosion(void* _this, DWORD edx, unsigned int tick) {
        
        auto vehicle_ent = get_vehicle_from_base((void*)_this);
        if (!vehicle_ent) return false;

        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

        //NOTE(DavoSK): prevents spamming network when vehicle is not yet exploded
        if (vehicle && !vehicle->wants_explode) {
            librg_send(&network_context, NETWORK_VEHICLE_EXPLODE, data, {
                librg_data_wu32(&data, vehicle_ent->id);
            });
            vehicle->wants_explode = true;
        }

        return false;
    }

    //----------------------------------------------
    //C_Vehicle::Deform((S_vector const &,S_vector const &,float,float,uint,S_vector const *))
    //----------------------------------------------
    typedef bool(__thiscall* C_Vehicle_Deform_t)(void* _this, const Vector3D& unk1, const Vector3D& unk2, float unk3, float unk4, unsigned int unk5, Vector3D* unk6);
    C_Vehicle_Deform_t c_vehicle_deform_original = nullptr;
    bool __fastcall C_Vehicle_Deform(void* _this, 
        DWORD edx, 	
        const Vector3D & pos, 
        const Vector3D & rot, 
        float unk1,
        float unk2,
        unsigned int unk3,
        Vector3D* unk4) {
        
        printf("ARG1, %f %f %f\n", EXPLODE_VEC(pos));
        printf("ARG2, %f %f %f\n", EXPLODE_VEC(rot));
        printf("ARG3, %f\n", unk1);
        printf("ARG4, %f\n", unk2);
        printf("ARG5, %d\n", unk3);

        if (unk4) {
            printf("ARG6, %f %f %f\n", unk4->x, unk4->y, unk4->z);
        }

        return c_vehicle_deform_original(_this, pos, rot, unk1, unk2, unk3, unk4);
    }
}

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

 void vehicle_init() {
    // Vehicle
    MemoryPatcher::InstallJmpHook(0x004E526B, (DWORD)&ChangeUpdateCarPos);
    MemoryPatcher::InstallJmpHook(0x004E5E9F, (DWORD)&ChangeUpdateCarPosCollision);

    hooks::car_prepare_dropout_wheel_original = reinterpret_cast<hooks::C_car_Prepare_DropOut_Wheel_t>(
        DetourFunction((PBYTE)0x00426DD0, (PBYTE)&hooks::C_car_Prepare_DropOut_Wheel)
    );

    hooks::car_prepare_dropout_original = reinterpret_cast<hooks::C_car_Prepare_DropOut_t>(
        DetourFunction((PBYTE)0x00426EC0, (PBYTE)&hooks::C_car_Prepare_DropOut)
    );

    hooks::c_car_carexplosion_original = reinterpret_cast<hooks::C_car_CarExplosion_t>(
        DetourFunction((PBYTE)0x00421D60, (PBYTE)&hooks::C_car_CarExplosion)
    );

    //Deform
    hooks::c_vehicle_deform_original = reinterpret_cast<hooks::C_Vehicle_Deform_t>(
        ((PBYTE)0x004D5610, (PBYTE)&hooks::C_Vehicle_Deform)
    );
}