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

    return new_car;
}

auto vehicle_remove(mafia_vehicle* vehicle) -> void {
    if(vehicle->car) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(vehicle->car);
    }
}