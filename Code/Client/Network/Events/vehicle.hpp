#pragma once

inline auto vehicle_entitycreate(librg_event* evnt) {

	auto vehicle = new mafia_vehicle();
    zpl_vec3 position;
    librg_data_rptr(evnt->data, &vehicle->rotation, sizeof(zpl_vec3));
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

    vehicle->car = vehicle_spawn(position, vehicle);
    evnt->entity->user_data = (void*)vehicle;
    evnt->entity->flags |= ENTITY_INTERPOLATED;
}

inline auto vehicle_game_tick(mafia_vehicle* vehicle, f64 delta) {
    
    f32 alpha = vehicle->inter_delta / network_context.timesync.server_delay;
    vehicle->inter_delta += (f32)delta;

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;    
    vehicle_int->position   = EXPAND_VEC(cubic_hermite_v3_interpolate(&vehicle->inter_pos, alpha));
    vehicle_int->rotation   = EXPAND_VEC(cubic_hermite_v3_interpolate(&vehicle->inter_rot, alpha));
    vehicle_int->speed      = EXPAND_VEC(cubic_hermite_v3_interpolate(&vehicle->inter_speed, alpha));
}

inline auto vehicle_entityupdate(librg_event* evnt) {
	auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    librg_data_rptr(evnt->data, &vehicle->rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
	vehicle->engine_health 		= librg_data_rf32(evnt->data);
	vehicle->wheel_angle 		= librg_data_rf32(evnt->data);
	vehicle->fuel 				= librg_data_rf32(evnt->data);
	vehicle->hand_break 		= librg_data_rf32(evnt->data);
	vehicle->break_val 			= librg_data_rf32(evnt->data);
	vehicle->clutch 			= librg_data_rf32(evnt->data);
	vehicle->gear 				= librg_data_ri32(evnt->data);
	vehicle->horn 				= librg_data_ru8(evnt->data);
	vehicle->siren 				= librg_data_ru8(evnt->data);
	vehicle->engine_on			= librg_data_ru8(evnt->data);

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;    
    vehicle_int->engine_health      = vehicle->engine_health;
    vehicle_int->wheel_angle        = vehicle->wheel_angle;
    vehicle_int->fuel               = vehicle->fuel;
    vehicle_int->hand_break         = vehicle->hand_break;
    vehicle_int->break_val          = vehicle->break_val;
    vehicle_int->clutch             = vehicle->clutch;
    vehicle_int->gear               = vehicle->gear;
    vehicle_int->horn               = vehicle->horn;
    vehicle_int->siren              = vehicle->siren;
    
    if(vehicle_int->engine_on != vehicle->engine_on) {
        vehicle->car->SetEngineOn(vehicle->engine_on, vehicle->engine_on);
    }

    if(vehicle_int->gear != vehicle->gear) {
        vehicle->car->SetGear(vehicle->gear);
        vehicle->car->GearSnd();
    }

    /* update interpolation tables */
    cubic_hermite_v3_value(&vehicle->inter_pos, evnt->entity->position);
	cubic_hermite_v3_value(&vehicle->inter_rot, vehicle->rotation);
    cubic_hermite_v3_value(&vehicle->inter_speed, vehicle->speed);
    vehicle->inter_delta = 0.0f;
}

inline auto vehicle_entityremove(librg_event* evnt) {
	auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
	if (vehicle->car) {
        vehicle_remove(vehicle);
		vehicle->car = nullptr;

        free(vehicle);
        vehicle = nullptr;
	
        evnt->entity->flags &= ~ENTITY_INTERPOLATED;
    }
}

inline auto vehicle_clientstreamer_update(librg_event* evnt) {
	auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
	auto car_int = reinterpret_cast<MafiaSDK::C_Car*>(vehicle->car)->GetInterface();
	auto our_int = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()->GetInterface()->humanObject;

	if (!car_int) {
		librg_event_reject(evnt);
		return;
	}

    auto vehicle_int = car_int->vehicle_interface;
	evnt->entity->position	= EXPAND_VEC(vehicle_int.position);
    librg_data_wptr(evnt->data, &vehicle_int.rotation, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle_int.speed, sizeof(zpl_vec3));
	librg_data_wf32(evnt->data, vehicle_int.engine_health);
    librg_data_wf32(evnt->data, vehicle_int.wheel_angle);
    librg_data_wf32(evnt->data, vehicle_int.fuel);
    librg_data_wf32(evnt->data, vehicle_int.hand_break);
    librg_data_wf32(evnt->data, vehicle_int.break_val);
    librg_data_wf32(evnt->data, vehicle_int.clutch);
    librg_data_wi32(evnt->data, vehicle_int.gear);
    librg_data_wu8(evnt->data, vehicle_int.horn);
    librg_data_wu8(evnt->data, vehicle_int.siren);
    librg_data_wu8(evnt->data, vehicle_int.engine_on);
}