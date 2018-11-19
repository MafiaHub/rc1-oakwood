#pragma once

inline auto vehicle_entitycreate(librg_event* evnt) {

	auto vehicle = new mafia_vehicle();
    zpl_vec3 position;
    librg_data_rptr(evnt->data, &vehicle->rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &vehicle->rotation_second, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &position, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, vehicle->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, vehicle->seats, sizeof(i32) * 4);

    vehicle->engine_health      = librg_data_rf32(evnt->data);
    vehicle->health             = librg_data_rf32(evnt->data);
    vehicle->horn               = librg_data_ru8(evnt->data);
    vehicle->siren              = librg_data_ru8(evnt->data);
    vehicle->sound_enabled      = librg_data_ru8(evnt->data);
    vehicle->hand_break         = librg_data_rf32(evnt->data);
    vehicle->speed_limit        = librg_data_rf32(evnt->data);
    vehicle->gear               = librg_data_ri32(evnt->data);
    vehicle->break_val          = librg_data_rf32(evnt->data);
    vehicle->clutch             = librg_data_rf32(evnt->data);
    vehicle->wheel_angle        = librg_data_rf32(evnt->data);
    vehicle->engine_on          = librg_data_ru8(evnt->data);
    vehicle->fuel               = librg_data_rf32(evnt->data);
	vehicle->accelerating		= librg_data_rf32(evnt->data);
    vehicle->car				= vehicle_spawn(position, vehicle);

    evnt->entity->user_data = (void*)vehicle;
    evnt->entity->flags |= ENTITY_INTERPOLATED;
}

inline auto vehicle_game_tick(mafia_vehicle* vehicle, f64 delta) {
    
	f64 alpha = vehicle->inter_delta / network_context.timesync.server_delay;
    vehicle->inter_delta += delta;

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;    
	zpl_vec3 lerped_pos, lerped_rot, lerped_rot_second;
	zpl_vec3_lerp(&lerped_pos, vehicle->last_pos, vehicle->target_pos, alpha);
	zpl_vec3_lerp(&lerped_rot, vehicle->last_rot, vehicle->target_rot, alpha);
	zpl_vec3_lerp(&lerped_rot_second, vehicle->last_rot_second, vehicle->target_rot_second, alpha);

	vehicle->interpolated_pos		= lerped_pos;
	vehicle->interpolated_rot		= lerped_rot;

	vehicle_int->position			= EXPAND_VEC(lerped_pos);
    vehicle_int->rotation			= EXPAND_VEC(lerped_rot);
	vehicle_int->rotation_second	= EXPAND_VEC(lerped_rot_second);

	MafiaSDK::C_Actor* car_act = (MafiaSDK::C_Actor*)vehicle->car;
	car_act->SetActState(0);
}

inline auto vehicle_entityupdate(librg_event* evnt) {
	auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    librg_data_rptr(evnt->data, &vehicle->target_rot, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &vehicle->target_rot_second, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));

	vehicle->engine_health 		= librg_data_rf32(evnt->data);
	vehicle->wheel_angle 		= librg_data_rf32(evnt->data);
	vehicle->fuel 				= librg_data_rf32(evnt->data);
	vehicle->accelerating		= librg_data_rf32(evnt->data);
	vehicle->hand_break 		= librg_data_rf32(evnt->data);
	vehicle->break_val 			= librg_data_rf32(evnt->data);
	vehicle->clutch 			= librg_data_rf32(evnt->data);
	vehicle->gear 				= librg_data_ri32(evnt->data);
	vehicle->horn 				= librg_data_ru8(evnt->data);
	vehicle->siren 				= librg_data_ru8(evnt->data);
	vehicle->engine_on			= librg_data_ru8(evnt->data);
	vehicle->target_pos			= evnt->entity->position;

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;    
    vehicle_int->engine_health      = vehicle->engine_health;
    vehicle_int->wheel_angle        = vehicle->wheel_angle;
    vehicle_int->fuel               = vehicle->fuel;
	vehicle_int->accelerating		= vehicle->accelerating;
    vehicle_int->hand_break         = vehicle->hand_break;
    vehicle_int->break_val          = vehicle->break_val;
    vehicle_int->clutch             = vehicle->clutch;
    vehicle_int->gear               = vehicle->gear;
    vehicle_int->horn               = vehicle->horn;
    vehicle_int->siren              = vehicle->siren;

	vehicle_int->speed				= EXPAND_VEC(vehicle->speed);
	vehicle->last_pos				= EXPAND_VEC(vehicle_int->position);
	vehicle->last_rot				= EXPAND_VEC(vehicle_int->rotation);
	vehicle->last_rot_second		= EXPAND_VEC(vehicle_int->rotation_second);

    if(vehicle_int->engine_on != vehicle->engine_on) {
        vehicle->car->SetEngineOn(vehicle->engine_on, vehicle->engine_on);
    }

	vehicle->car->MotorForce(vehicle->accelerating, vehicle->accelerating);
	vehicle_int->engine_on = vehicle->engine_on;

    if(vehicle_int->gear != vehicle->gear) {
        vehicle->car->SetGear(vehicle->gear);
        vehicle->car->GearSnd();
    }

    vehicle->inter_delta = 0.0f;
}

inline auto vehicle_entityremove(librg_event* evnt) {
	auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
	if (vehicle->car) {
		evnt->entity->flags &= ~ENTITY_INTERPOLATED;

        vehicle_remove(vehicle);
		vehicle->car = nullptr;

        free(vehicle);
        vehicle = nullptr;
    }
}

inline auto vehicle_clientstreamer_update(librg_event* evnt) {
	auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
	auto car_int = reinterpret_cast<MafiaSDK::C_Car*>(vehicle->car)->GetInterface();
	if (!car_int) {
		librg_event_reject(evnt);
		return;
	}

    auto vehicle_int = car_int->vehicle_interface;
	evnt->entity->position	= EXPAND_VEC(vehicle_int.position);
    librg_data_wptr(evnt->data, &vehicle_int.rotation, sizeof(zpl_vec3));
	librg_data_wptr(evnt->data, &vehicle_int.rotation_second, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle_int.speed, sizeof(zpl_vec3));
	librg_data_wf32(evnt->data, vehicle_int.engine_health);
    librg_data_wf32(evnt->data, vehicle_int.wheel_angle);
    librg_data_wf32(evnt->data, vehicle_int.fuel);
	librg_data_wf32(evnt->data, vehicle_int.accelerating);
    librg_data_wf32(evnt->data, vehicle_int.hand_break);
    librg_data_wf32(evnt->data, vehicle_int.break_val);
    librg_data_wf32(evnt->data, vehicle_int.clutch);
    librg_data_wi32(evnt->data, vehicle_int.gear);
    librg_data_wu8(evnt->data, vehicle_int.horn);
    librg_data_wu8(evnt->data, vehicle_int.siren);
    librg_data_wu8(evnt->data, vehicle_int.engine_on);
}