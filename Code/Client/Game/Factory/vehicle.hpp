auto vehicle_spawn(zpl_vec3 position, 
                   mafia_vehicle* spawn_struct) -> MafiaSDK::C_Car* {

    Vector3D default_scale = { 1.0f, 1.0f, 1.0f };
    Vector3D default_pos = EXPAND_VEC(position);

    auto vehicle_frame = new MafiaSDK::I3D_Frame();
    vehicle_frame->SetName("mafia_vehicle");
    vehicle_frame->LoadModel(spawn_struct->model);
    vehicle_frame->SetScale(default_scale);
    vehicle_frame->SetPos(default_pos);
    
    MafiaSDK::C_Car *new_car = reinterpret_cast<MafiaSDK::C_Car*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Car));
    new_car->Init(vehicle_frame);
    new_car->SetActive(1);
    MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_car);

    auto veh_inter = &new_car->GetInterface()->vehicle_interface;
    
    if (veh_inter->engine_on) {
        new_car->SetEngineOn(spawn_struct->engine_on, spawn_struct->engine_on);
    }

    veh_inter->health           = spawn_struct->health;
    veh_inter->position			= EXPAND_VEC(position);
    veh_inter->rotation			= EXPAND_VEC(spawn_struct->rotation);
    veh_inter->rotation_second	= EXPAND_VEC(spawn_struct->rotation_second);
    veh_inter->speed            = EXPAND_VEC(spawn_struct->speed);
    veh_inter->engine_health    = spawn_struct->engine_health;
    veh_inter->health           = spawn_struct->health;
    veh_inter->horn             = spawn_struct->horn;
    veh_inter->siren            = spawn_struct->siren;
    veh_inter->sound_enabled    = spawn_struct->sound_enabled;
    veh_inter->hand_break       = spawn_struct->hand_break;
    veh_inter->speed_limit      = spawn_struct->speed_limit;
    veh_inter->gear             = spawn_struct->gear;
    veh_inter->break_val        = spawn_struct->break_val;
    veh_inter->clutch           = spawn_struct->clutch;
    veh_inter->wheel_angle      = spawn_struct->wheel_angle;
    veh_inter->engine_on        = spawn_struct->engine_on;
    veh_inter->fuel             = spawn_struct->fuel;
    veh_inter->accelerating		= spawn_struct->accelerating;
    veh_inter->engine_rpm		= spawn_struct->engine_rpm;

    // NOTE(DavoSK): Update tyres
    for (int i = 0; i < 4; i++) {
        auto mafia_tyre = spawn_struct->tyres[i];
        auto tyre = new_car->GetCarTyre(i);

        if (tyre == NULL) continue;

        *(DWORD*)((DWORD)tyre + 0x120) |= mafia_tyre.flags;
        *(float*)((DWORD)tyre + 0x18C) = mafia_tyre.health;

        if (mafia_tyre.health <= 0.0f) {
            new_car->RemoveTyre(i);
        }
    }
    
    // Apply deformation deltas
    for (auto delta : spawn_struct->deform_deltas) {
        auto mesh = new_car->GetMeshByIndex(delta.mesh_index);
        if (mesh) {
            auto mesh_lod = mesh->GetLOD(0);
            if (mesh_lod) {
                auto vertices = mesh_lod->LockVertices(0);
                vertices[delta.vertex_index].n = EXPAND_VEC(delta.normal);
                vertices[delta.vertex_index].p = EXPAND_VEC(delta.position);
                mesh_lod->UnlockVertices();
            }
        }
    }

    // NOTE(DavoSK): Update components
    for (int i = 0; i < 15; i++) {
        if (spawn_struct->destroyed_components[i]) {
            new_car->RemoveComponent(i);
        }
    }

    return new_car;
}

auto vehicle_remove(mafia_vehicle* vehicle) -> void {
    if(vehicle->car) {

        for (int i = 0; i < 4; i++) {
            auto seat = vehicle->seats[i];
            if (seat != -1) {
                auto player_ent = librg_entity_fetch(&network_context, seat);
                if (player_ent && player_ent->user_data) {
                    auto player = (mafia_player*)player_ent->user_data;
                    player->ped->Intern_FromCar();
                }
            }
        }

        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(vehicle->car);
    }
}
