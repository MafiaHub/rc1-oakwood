#pragma once

// =======================================================================//
// !
// ! Position Interpolation
// !
// =======================================================================//

void car_target_position_update(mafia_vehicle *car) {
    if (car->interp.pos.finishTime == 0) {
        return;
    }

    // Grab the current game position
    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
    zpl_vec3 currentPosition = EXPAND_VEC(vehicle_int->position);

    // Get the factor of time spent from the interpolation start to the current time.
    f64 currentTime = zpl_time_now();
    f32 alpha = zpl_unlerp(currentTime, car->interp.pos.startTime, car->interp.pos.finishTime);

    // Don't let it overcompensate the error too much
    alpha = zpl_clamp(0.0f, alpha, 1.5f);

    // Get the current error portion to compensate
    f32 currentAlpha = alpha - car->interp.pos.lastAlpha;
    car->interp.pos.lastAlpha = alpha;

    // Apply the error compensation
    zpl_vec3 compensation;
    zpl_vec3_lerp(&compensation, zpl_vec3f_zero(), car->interp.pos.error, currentAlpha);

    // If we finished compensating the error, finish it for the next pulse
    if (alpha == 1.5f) {
        car->interp.pos.finishTime = 0;
    }

    zpl_vec3 newPosition;
    zpl_vec3_add(&newPosition, currentPosition, compensation);

    // Check if the distance to interpolate is too far.
    // vec3 velocity = car->CCar->m_pVehicle.m_vSpeed;
    // f32 threshold = (VEHICLE_INTERPOLATION_THRESHOLD * zplm_vec3_mag(velocity)) * M2O_TICKRATE_SERVER / 0.1f;

    // // There is a reason to have this condition this way: To prevent NaNs generating new NaNs after interpolating (Comparing with NaNs always results to false).
    // if (!(zplm_vec3_mag(currentPosition - car->interp.pos.target) <= threshold)) {
    //     // Abort all interpolation
    //     if (car->interp.rot.finishTime != 0) {
    //         car->CCar->SetRot(zplm_quat_from_eular(
    //             zplm_vec3_to_radians(car->interp.rot.target)
    //         ));
    //     }

    //     newPosition = car->interp.pos.target;
    //     car->interp.pos.finishTime = 0;
    //     car->interp.rot.finishTime = 0;
    // }

    vehicle_int->position = EXPAND_VEC(newPosition);
}

void car_target_position_set(mafia_vehicle *car, zpl_vec3 targetPos, f32 interpTime, bool validVelocityZ, f32 velocityZ) {
    
    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;
    car_target_position_update(car);

    { /* UpdateUnderFloorFix */
        bool forceLocalZ = false;
        zpl_vec3 localPos = EXPAND_VEC(vehicle_int->position);

        if (validVelocityZ) {
            // If remote z higher by too much and remote not doing any z movement, warp local z coord
            f32 deltaZ = targetPos.z - localPos.z;
            if (deltaZ > 0.4f && deltaZ < 10.0f) {
                if (zpl_abs(velocityZ) < 0.01f) {
                    forceLocalZ = true;
                }
            }
        }

        // Only force z coord if needed for at least two consecutive calls
        if (!forceLocalZ) {
            car->interp.forceLocalZCounter = 0;
        }
        else if (car->interp.forceLocalZCounter++ > 1) {
            localPos.z = targetPos.z;
            vehicle_int->position = EXPAND_VEC(localPos);
        }
    }

    zpl_vec3 localPos = EXPAND_VEC(vehicle_int->position);

    car->interp.pos.start = localPos;
    car->interp.pos.target = targetPos;

    zpl_vec3 error_vec;
    zpl_vec3_sub(&error_vec, targetPos, localPos);
    car->interp.pos.error = error_vec;

    // Apply the error over 400ms (i.e. 1/4 per 100ms )
    zpl_vec3_mul(&car->interp.pos.error, car->interp.pos.error, zpl_lerp(0.4f, 1.0f, zpl_clamp01(zpl_unlerp(interpTime, 0.1f, 0.4f))));
    
    // Get the interpolation interval
    car->interp.pos.startTime = zpl_time_now();
    car->interp.pos.finishTime = car->interp.pos.startTime + interpTime;
    car->interp.pos.lastAlpha = 0.0f;
}

// =======================================================================//
// !
// ! Rotation Interpolation
// !
// =======================================================================//

void car_target_rotation_update(mafia_vehicle *car) {
    if (car->interp.rot.finishTime == 0) {
        return;
    }

    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;

    // Grab the current game rotation (in degrees)
    zpl_vec3 rotation		= EXPAND_VEC(vehicle_int->rotation);
    zpl_vec3 rotation_sec	= EXPAND_VEC(vehicle_int->rotation_second);

    // Get the factor of time spent from the interpolation start to the current time.
    f64 currentTime = zpl_time_now();
    f32 alpha = zpl_unlerp(currentTime, car->interp.rot.startTime, car->interp.rot.finishTime);

    // Don't let it to overcompensate the error
    alpha = zpl_clamp(0.0f, alpha, 1.0f);

    // Get the current error portion to compensate
    f32 currentAlpha = alpha - car->interp.rot.lastAlpha;
    car->interp.rot.lastAlpha = alpha;

    zpl_vec3 compensation;
    zpl_vec3_lerp(&compensation, zpl_vec3f_zero(), car->interp.rot.error, currentAlpha);

    // If we finished compensating the error, finish it for the next pulse
    if (alpha == 1.0f) {
        car->interp.rot.finishTime = 0;
    }

    zpl_vec3 compensated;
    zpl_vec3_add(&compensated, rotation, compensation);
    vehicle_int->rotation = EXPAND_VEC(compensated);


    // Get the current error portion to compensate
    f32 currentAlphaSecond = alpha - car->interp.rot_second.lastAlpha;
    car->interp.rot_second.lastAlpha = alpha;

    zpl_vec3 compensation_second;
    zpl_vec3_lerp(&compensation_second, zpl_vec3f_zero(), car->interp.rot_second.error, currentAlphaSecond);

    // If we finished compensating the error, finish it for the next pulse
    if (alpha == 1.0f) {
        car->interp.rot_second.finishTime = 0;
    }

    zpl_vec3 compensated_second;
    zpl_vec3_add(&compensated_second, rotation_sec, compensation_second);
    vehicle_int->rotation_second = EXPAND_VEC(compensated_second);
}

void car_target_rotation_set(mafia_vehicle *car, zpl_vec3 targetRot, zpl_vec3 targetRotSecond, f32 interpTime) {
    car_target_rotation_update(car);

    auto vehicle_int = &car->car->GetInterface()->vehicle_interface;

    // Grab the current game rotation (in degrees)
    zpl_vec3 rotation		= EXPAND_VEC(vehicle_int->rotation);
    zpl_vec3 rotation_sec	= EXPAND_VEC(vehicle_int->rotation_second);

    // ------------------- FIRST ----------------------
    car->interp.rot.start = rotation;
    car->interp.rot.target = targetRot;

    // Get the error
    car->interp.rot.error.x = targetRot.x - rotation.x;
    car->interp.rot.error.y = targetRot.y - rotation.y;
    car->interp.rot.error.z = targetRot.z - rotation.z;

    // Apply the error over 250ms (i.e. 2/5 per 100ms )
    zpl_vec3_mul(&car->interp.rot.error, car->interp.rot.error, zpl_lerp(0.25f, 1.0f, zpl_clamp01(zpl_unlerp(interpTime, 0.1f, 0.4f))));

    // Get the interpolation interval
    car->interp.rot.startTime = zpl_time_now();
    car->interp.rot.finishTime = car->interp.rot.startTime + interpTime;
    car->interp.rot.lastAlpha = 0.0f;

    // ------------------- SECOND ----------------------

    car->interp.rot_second.start = rotation_sec;
    car->interp.rot_second.target = targetRotSecond;

    // Get the error
    car->interp.rot_second.error.x = targetRotSecond.x - rotation_sec.x;
    car->interp.rot_second.error.y = targetRotSecond.y - rotation_sec.y;
    car->interp.rot_second.error.z = targetRotSecond.z - rotation_sec.z;

    // Apply the error over 250ms (i.e. 2/5 per 100ms )
    zpl_vec3_mul(&car->interp.rot_second.error, car->interp.rot_second.error, zpl_lerp(0.25f, 1.0f, zpl_clamp01(zpl_unlerp(interpTime, 0.1f, 0.4f))));

    // Get the interpolation interval
    car->interp.rot_second.startTime = zpl_time_now();
    car->interp.rot_second.finishTime = car->interp.rot_second.startTime + interpTime;
    car->interp.rot_second.lastAlpha = 0.0f;
}

inline auto vehicle_entitycreate(librg_event* evnt) {

    auto vehicle = new mafia_vehicle();
    zpl_vec3 position;
    librg_data_rptr(evnt->data, &vehicle->rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->rotation_second, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &position, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, vehicle->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, vehicle->seats, sizeof(i32) * 4);
    librg_data_rptr(evnt->data, vehicle->tyres, sizeof(mafia_vehicle_tyre) * 4);
    librg_data_rptr(evnt->data, vehicle->destroyed_components, sizeof(u8) * 15);

    u32 deltas_count = librg_data_ru32(evnt->data);

    if(!vehicle->deform_deltas.empty()) 
        vehicle->deform_deltas.clear();

    for (u32 i = 0; i < deltas_count; i++) {
        mafia_vehicle_deform delta;
        librg_data_rptr(evnt->data, &delta, sizeof(mafia_vehicle_deform));
        vehicle->deform_deltas.push_back(delta);
    }

    vehicle->engine_rpm			= librg_data_rf32(evnt->data);
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
    
    car_target_position_update(vehicle);
    car_target_rotation_update(vehicle);

    MafiaSDK::C_Actor* car_act = (MafiaSDK::C_Actor*)vehicle->car;
    car_act->SetActState(0);

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;
    vehicle_int->engine_rpm = vehicle->engine_rpm;
}

inline auto vehicle_entityupdate(librg_event* evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;

    zpl_vec3 target_rot, target_rot_second, target_pos;

    librg_data_rptr(evnt->data, &target_rot, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &target_rot_second, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));

    vehicle->engine_rpm			= librg_data_rf32(evnt->data);
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
    target_pos					= evnt->entity->position;

    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;    
    vehicle_int->engine_health      = vehicle->engine_health;
    vehicle_int->wheel_angle        = vehicle->wheel_angle;
    vehicle_int->fuel               = vehicle->fuel;
    vehicle_int->accelerating		= vehicle->accelerating;
    vehicle_int->hand_break         = vehicle->hand_break;
    vehicle_int->break_val          = vehicle->break_val;
    vehicle_int->clutch             = vehicle->clutch;
    vehicle_int->horn               = vehicle->horn;
    vehicle_int->siren              = vehicle->siren;
    vehicle_int->speed				= EXPAND_VEC(vehicle->speed);

    if(vehicle_int->engine_on != vehicle->engine_on) {
        vehicle->car->SetEngineOn(vehicle->engine_on, vehicle->engine_on);
    }
    vehicle_int->engine_on = vehicle->engine_on;
    
    if(vehicle_int->gear != vehicle->gear) {
        vehicle->car->SetGear(vehicle->gear);
    }

    car_target_rotation_set(vehicle, target_rot, target_rot_second, 0.1f);
    car_target_position_set(vehicle, target_pos, 0.1f, true, vehicle_int->speed.z);
}

inline auto vehicle_entityremove(librg_event* evnt) {
    auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
    if (vehicle && vehicle->car) {
        evnt->entity->flags &= ~ENTITY_INTERPOLATED;
        vehicle_despawn(vehicle);
        zpl_zero_item(vehicle);
        evnt->entity->user_data = nullptr;
    }
}

inline auto vehicle_clientstreamer_update(librg_event* evnt) {
    auto vehicle = (mafia_vehicle*)evnt->entity->user_data;
    
    if (!vehicle) {
        librg_event_reject(evnt);
        return;
    }

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
    constexpr DWORD FLAT_TYRE_FLAG = 0x80000000;

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
    }
}

#include "Game/Hooks/vehicle.hpp"
