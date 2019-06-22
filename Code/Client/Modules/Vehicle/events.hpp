#pragma once

// =======================================================================//
// !
// ! Position Interpolation
// !
// =======================================================================//
#define VEHICLE_THRESHOLD_FOR_SPEED 0.6f
#define VEHICLE_INTERPOLATION_THRESHOLD 15

void target_position_update(mafia_vehicle *car) {
    
    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
    zpl_vec3 current_position = EXPAND_VEC(vehicle_int->position);
    zpl_vec3 new_position = lib_inter_interpolate(car->interp.pos, current_position);

    //Check if the distance to interpolate is too far.
    zpl_vec3 velocity = EXPAND_VEC(car->speed);
    f32 threshold = zpl_vec3_mag(velocity) * 1.5f;

    zpl_vec3 distance;
    zpl_vec3_sub(&distance, current_position, car->interp.pos->target);

    if (!(zpl_vec3_mag(distance) <= threshold)) {
        new_position = car->interp.pos->target;
        lib_inter_reset(car->interp.pos, new_position);
        lib_inter_reset(car->interp.rot, EXPAND_VEC(car->interp.rot->target));
        lib_inter_reset(car->interp.rot_up, EXPAND_VEC(car->interp.rot_up->target));
        vehicle_int->rot_speed = { 0.0f, 0.0f, 0.0f };
    }

    vehicle_int->position = EXPAND_VEC(new_position);
}

void target_position_set(mafia_vehicle *car, zpl_vec3 target_pos, bool valid_velocityz, f32 velocityz) {

    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
    target_position_update(car);

    { /* UpdateUnderFloorFix */
        bool force_localz = false;
        zpl_vec3 localPos = EXPAND_VEC(vehicle_int->position);

        if (valid_velocityz) {
            // If remote z higher by too much and remote not doing any z movement, warp local z coord
            f32 deltaZ = target_pos.z - localPos.z;
            if (deltaZ > 0.4f && deltaZ < 10.0f) {
                if (zpl_abs(velocityz) < 0.01f) {
                    force_localz = true;
                }
            }
        }

        // Only force z coord if needed for at least two consecutive calls
        if (!force_localz) {
            car->interp.force_localz_counter = 0;
        } else if (car->interp.force_localz_counter++ > 1) {
            localPos.z = target_pos.z;
            vehicle_int->position = EXPAND_VEC(localPos);
        }
    }

    zpl_vec3 local_pos = EXPAND_VEC(vehicle_int->position);
    lib_inter_set_target(car->interp.pos, local_pos, target_pos);
}

// =======================================================================//
// !
// ! Rotation Interpolation
// !
// =======================================================================//

void target_rotation_update(mafia_vehicle *car) {

    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
  
    zpl_vec3 new_forward        = lib_inter_interpolate(car->interp.rot, EXPAND_VEC(vehicle_int->rot_forward));
    vehicle_int->rot_forward    = EXPAND_VEC(new_forward);

    zpl_vec3 new_up             = lib_inter_interpolate(car->interp.rot_up, EXPAND_VEC(vehicle_int->rot_up));
    vehicle_int->rot_up         = EXPAND_VEC(new_up);
}

void target_rotation_set(mafia_vehicle *car, zpl_vec3 target_rot_forward, zpl_vec3 target_rot_up) {

    target_rotation_update(car);
    
    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
    lib_inter_set_target(car->interp.rot, EXPAND_VEC(vehicle_int->rot_forward), target_rot_forward);
    lib_inter_set_target(car->interp.rot_up, EXPAND_VEC(vehicle_int->rot_up), target_rot_up);
}

inline auto entitycreate(librg_event *evnt) {
   
    auto vehicle = new mafia_vehicle();

    vehicle->interp.rot     = lib_inter_create_interpolator(GlobalConfig.interp_time_vehicle, true);
    vehicle->interp.rot_up  = lib_inter_create_interpolator(GlobalConfig.interp_time_vehicle, true);
    vehicle->interp.pos     = lib_inter_create_interpolator(GlobalConfig.interp_time_vehicle, true);

    vehicle->interp.pos->error_treshold = 0.1f;
    vehicle->interp.rot->error_treshold = vehicle->interp.rot_up->error_treshold = 0.07f;

    zpl_vec3 position;
    librg_data_rptr(evnt->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &position, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, vehicle->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, vehicle->seats, sizeof(i32) * 4);
    librg_data_rptr(evnt->data, vehicle->tyres, sizeof(mafia_vehicle_tyre) * 4);
    librg_data_rptr(evnt->data, vehicle->destroyed_components, sizeof(u8) * 15);

    u32 deltas_count = librg_data_ru32(evnt->data);

    if (!vehicle->deform_deltas.empty()) vehicle->deform_deltas.clear();

    for (u32 i = 0; i < deltas_count; i++) {
        mafia_vehicle_deform delta;
        librg_data_rptr(evnt->data, &delta, sizeof(mafia_vehicle_deform));
        vehicle->deform_deltas.push_back(delta);
    }

    vehicle->engine_rpm = librg_data_rf32(evnt->data);
    vehicle->engine_health = librg_data_rf32(evnt->data);
    vehicle->health = librg_data_rf32(evnt->data);
    vehicle->horn = librg_data_ru8(evnt->data);
    vehicle->siren = librg_data_ru8(evnt->data);
    vehicle->sound_enabled = librg_data_ru8(evnt->data);
    vehicle->is_car_in_radar = librg_data_ru8(evnt->data);
    vehicle->hand_break = librg_data_rf32(evnt->data);
    vehicle->speed_limit = librg_data_rf32(evnt->data);
    vehicle->gear = librg_data_ri32(evnt->data);
    vehicle->break_val = librg_data_rf32(evnt->data);
    vehicle->clutch = librg_data_rf32(evnt->data);
    vehicle->wheel_angle = librg_data_rf32(evnt->data);
    vehicle->engine_on = librg_data_ru8(evnt->data);
    vehicle->fuel = librg_data_rf32(evnt->data);
    vehicle->accelerating = librg_data_rf32(evnt->data);
    vehicle->is_visible_on_map = librg_data_ru8(evnt->data);
    vehicle->transparency = librg_data_rf32(evnt->data);
    vehicle->collision_state = librg_data_ru8(evnt->data);

    vehicle->car = spawn(position, vehicle, car_cache[evnt->entity->id]);
    car_cache[evnt->entity->id] = vehicle->car;

    evnt->entity->user_data = (void *)vehicle;
    evnt->entity->flags |= ENTITY_INTERPOLATED;
}

inline auto game_tick(mafia_vehicle *vehicle, f64 delta) {

    if(!vehicle || !vehicle->car) return;

    target_position_update(vehicle);
    target_rotation_update(vehicle);

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;
    vehicle_int->engine_rpm = vehicle->engine_rpm;
}

inline auto entityupdate(librg_event *evnt) {
   
    //NOTE(DavoSK): First we need to read all data so we can skip event
    zpl_vec3 target_rot_forward, target_rot_up, target_pos, rot_speed, speed;

    librg_data_rptr(evnt->data, &target_rot_forward, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &target_rot_up, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &rot_speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &speed, sizeof(zpl_vec3));

    f32 engine_rpm          = librg_data_rf32(evnt->data);
    f32 engine_health       = librg_data_rf32(evnt->data);
    f32 wheel_angle         = librg_data_rf32(evnt->data);
    f32 fuel                = librg_data_rf32(evnt->data);
    f32 accelerating        = librg_data_rf32(evnt->data);
    f32 hand_break          = librg_data_rf32(evnt->data);
    f32 break_val           = librg_data_rf32(evnt->data);
    f32 clutch              = librg_data_rf32(evnt->data);
    i32 gear                = librg_data_ri32(evnt->data);
    u8 horn                 = librg_data_ru8(evnt->data);
    u8 siren                = librg_data_ru8(evnt->data);
    u8 engine_on            = librg_data_ru8(evnt->data);
    
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;

    if (!vehicle || !vehicle->car) {
        librg_event_reject(evnt);
        return;
    }

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;
    if (!vehicle_int) {
        librg_event_reject(evnt);
        return;
    }

    //NOTE(DavoSK): Update mafia_vehicle structure 
    vehicle->engine_rpm         = engine_rpm;
    vehicle->engine_health      = engine_health;
    vehicle->wheel_angle        = wheel_angle;
    vehicle->fuel               = fuel;
    vehicle->accelerating       = accelerating;
    vehicle->hand_break         = hand_break;
    vehicle->break_val          = break_val;
    vehicle->clutch             = clutch;
    vehicle->gear               = gear;
    vehicle->horn               = horn;
    vehicle->siren              = siren;
    vehicle->engine_on          = engine_on;
    vehicle->speed              = speed;
    vehicle->rot_speed          = rot_speed;
    target_pos                  = evnt->entity->position;

    //NOTE(DavoSK): Now update gameobject interface
    vehicle_int->engine_health  = vehicle->engine_health;
    vehicle_int->wheel_angle    = vehicle->wheel_angle;
    vehicle_int->fuel           = vehicle->fuel;
    vehicle_int->accelerating   = vehicle->accelerating;
    vehicle_int->hand_break     = vehicle->hand_break;
    vehicle_int->break_val      = vehicle->break_val;
    vehicle_int->clutch         = vehicle->clutch;
    vehicle_int->horn           = vehicle->horn;
    vehicle_int->siren          = vehicle->siren;
    vehicle_int->speed          = EXPAND_VEC(vehicle->speed);
    vehicle_int->rot_speed      = EXPAND_VEC(vehicle->rot_speed);

    if (vehicle_int->engine_on != vehicle->engine_on) {
        vehicle->car->SetEngineOn(vehicle->engine_on, true);
    }
    
    if (vehicle_int->gear != vehicle->gear) {
        vehicle->car->SetGear(vehicle->gear);
    }

    target_rotation_set(vehicle, target_rot_forward, target_rot_up);
    target_position_set(vehicle, target_pos, true, vehicle_int->speed.z);
}

inline auto entityremove(librg_event *evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    if (vehicle && vehicle->car) {
        evnt->entity->flags &= ~ENTITY_INTERPOLATED;
        vehicle->clientside_flags |= CLIENTSIDE_VEHICLE_STREAMER_REMOVED;

        lib_inter_destroy_interpolator(vehicle->interp.pos);
        lib_inter_destroy_interpolator(vehicle->interp.rot);
        lib_inter_destroy_interpolator(vehicle->interp.rot_up);
        
        vehicle->car->SetTransparency(0.5f);
        despawn(vehicle);

        delete vehicle;
        evnt->entity->user_data = nullptr;
    }
}

inline auto clientstreamer_update(librg_event *evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    if (!vehicle) {
        librg_event_reject(evnt);
        return;
    }

    auto car_int = vehicle->car->GetInterface();
    if (!car_int) {
        librg_event_reject(evnt);
        return;
    }

    auto vehicle_int = car_int->vehicle_interface;
    evnt->entity->position = EXPAND_VEC(vehicle_int.position);
    librg_data_wptr(evnt->data, &vehicle_int.rot_forward, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle_int.rot_up, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle_int.rot_speed, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle_int.speed, sizeof(zpl_vec3));
    librg_data_wf32(evnt->data, vehicle_int.engine_rpm);
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

    // NOTE(DavoSK): check for vehicle tyre change
    // we will send flat tyres and health changes only when they changes
    // also we sending only particular flag for now only is tyre is flat
    /*constexpr DWORD FLAT_TYRE_FLAG = 0x80000000;

    if (vehicle->car) {
        for (int i = 0; i < 4; i++) {

            auto mafia_tyre = &vehicle->tyres[i];
            auto vehicle_tyre = vehicle->car->GetCarTyre(i);
            
            DWORD tyre_entity_flags = *(DWORD*)(vehicle_tyre + 0x120);
            float tyre_health = *(float*)((DWORD)vehicle_tyre + 0x18C);

            if ( ( (tyre_entity_flags & FLAT_TYRE_FLAG) && !(mafia_tyre->flags & FLAT_TYRE_FLAG)) ||
                mafia_tyre->health != tyre_health) {
                librg_send(&network_context, NETWORK_VEHICLE_WHEEL_UPDATE, data, {
                    DWORD tyre_flag = 0x0;
                    if (tyre_entity_flags & FLAT_TYRE_FLAG)
                        tyre_flag = FLAT_TYRE_FLAG;

                    librg_data_wu32(&data, evnt->entity->id);
                    librg_data_wu32(&data, (u32)i);
                    librg_data_wu32(&data, tyre_flag);
                    librg_data_wf32(&data, tyre_health);
                });

                mafia_tyre->flags = tyre_entity_flags;
                mafia_tyre->health = tyre_health;
            }
        }
    }*/
}
