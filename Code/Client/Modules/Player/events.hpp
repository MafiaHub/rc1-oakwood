#pragma once

// =======================================================================//
// !
// ! Position Interpolation
// !
// =======================================================================//

void target_position_update(mafia_player *player) {

    if (player->interp.pos.finish_time > 0.0) {
        // Grab the current game position
        auto player_int = player->ped->GetInterface();
        zpl_vec3 current_position = EXPAND_VEC(player_int->entity.position);
   
        // Get the factor of time spent from the interpolation start to the current time.
        f64 current_time = zpl_time_now();
        f32 alpha = zpl_unlerp(current_time, player->interp.pos.start_time, player->interp.pos.finish_time);

        // Don't let it overcompensate the error too much
        alpha = zpl_clamp(alpha, 0.0f, 1.0);

        // Get the current error portion to compensate
        f32 currentAlpha = alpha - player->interp.pos.last_alpha;
        player->interp.pos.last_alpha = alpha;

        // Apply the error compensation
        zpl_vec3 compensation;
        zpl_vec3_lerp(&compensation, zpl_vec3f_zero(), player->interp.pos.error, currentAlpha);

        // If we finished compensating the error, finish it for the next pulse
        if (alpha == 1.0f) {
            player->interp.pos.finish_time = 0;
        }

        zpl_vec3 new_position;
        zpl_vec3_add(&new_position, current_position, compensation);
        player_int->entity.position = EXPAND_VEC(new_position);
    }
}

void target_position_set(mafia_player *player, zpl_vec3 target_pos, f32 interpTime) {

    auto player_int = player->ped->GetInterface();
    target_position_update(player);

    zpl_vec3 local_pos = EXPAND_VEC(player_int->entity.position);
    player->interp.pos.start = local_pos;
    player->interp.pos.target = target_pos;

    zpl_vec3 error_vec;
    zpl_vec3_sub(&error_vec, target_pos, local_pos);
    player->interp.pos.error = error_vec;

    player->interp.pos.start_time = zpl_time_now();
    player->interp.pos.finish_time = player->interp.pos.start_time + interpTime;
    player->interp.pos.last_alpha = 0.0f;
}

// =======================================================================//
// !
// ! Rotation Interpolation
// !
// =======================================================================//

void target_rotation_update(mafia_player *player) {

    auto player_int = player->ped->GetInterface();

    // Grab the current game rotation
    zpl_vec3 rotation = EXPAND_VEC(player_int->entity.rotation);
 
    if (player->interp.rot.finish_time > 0.0f) {

        // Get the factor of time spent from the interpolation start to the current time.
        f64 currentTime = zpl_time_now();
        f32 alpha = zpl_unlerp(currentTime, player->interp.rot.start_time, player->interp.rot.finish_time);

        // Don't let it to overcompensate the error
        alpha = zpl_clamp(alpha, 0.0f, 1.0f);

        // Get the current error portion to compensate
        f32 current_alpha = alpha - player->interp.rot.last_alpha;
        player->interp.rot.last_alpha = alpha;

        zpl_vec3 compensation;
        zpl_vec3_lerp(&compensation, zpl_vec3f_zero(), player->interp.rot.error, current_alpha);

        // If we finished compensating the error, finish it for the next pulse
        if (alpha == 1.0f) {
            player->interp.rot.finish_time = 0;
        }

        zpl_vec3 compensated;
        zpl_vec3_add(&compensated, rotation, compensation);
        player_int->entity.rotation = EXPAND_VEC(compensated);
    }
}

zpl_vec3 compute_rotation_offset(zpl_vec3 a, zpl_vec3 b) {

    auto one_axis = [](float a, float b) { return a - b; };
    return {one_axis(a.x, b.x), one_axis(a.y, b.y), one_axis(a.z, b.z)};
}

void target_rotation_set(mafia_player *player, zpl_vec3 target_rot, f32 interp_time) {

    target_rotation_update(player);
    auto player_int = player->ped->GetInterface();

    // Grab the current game rotation
    zpl_vec3 rotation = EXPAND_VEC(player_int->entity.rotation);
    player->interp.rot.start = rotation;
    player->interp.rot.error = compute_rotation_offset(target_rot, rotation);

    // Get the interpolation interval
    player->interp.rot.start_time = zpl_time_now();
    player->interp.rot.finish_time = player->interp.rot.start_time + interp_time;
    player->interp.rot.last_alpha = 0.0f;
}

inline auto entitycreate(librg_event* evnt) -> void {

    auto player					= new mafia_player();
    player->vehicle_id			= librg_data_ri32(evnt->data);
    player->streamer_entity_id	= librg_data_ri32(evnt->data);
    
    if (player->vehicle_id != -1) {
        player->clientside_flags |= CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
    }

    librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, player->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, player->name, sizeof(char) * 32);

    player->is_crouching = librg_data_ru8(evnt->data);
    player->is_aiming = librg_data_ru8(evnt->data);
    librg_data_rptr(evnt->data, &player->inventory, sizeof(player_inventory));
    player->current_weapon_id = librg_data_ru32(evnt->data);
    player->health = librg_data_rf32(evnt->data);

    auto new_ped = spawn(
        evnt->entity->position,
        player->rotation,
        player->inventory,
        player->model,
        player->current_weapon_id,
        player->health,
        false,
        -1,
        player->vehicle_id != -1);

    //TODO(DavoSK): Rework respawn/spawn
    if (player->ped != nullptr)
        despawn(player->ped);

    player->ped = new_ped;

    if(player->streamer_entity_id == local_player.entity_id) {
        auto me = modules::player::get_local_ped();
        auto action = player->ped->GetActionManager()->NewFollow(me, 3.0f, 13, 2, 0, 0);
        player->ped->GetActionManager()->NewTurnTo(me, action->action_id);
        player->ped->GetActionManager()->AddJob(action);
        MafiaSDK::GetFollowManager()->AddFollower(player->ped, me);
        player->ped->ForceAI(0, 1, 0, 0);
    }
    else {
        evnt->entity->flags |= ENTITY_INTERPOLATED;
    }

    evnt->entity->user_data = player;
    
}

inline auto game_tick(mafia_player* ped, f64 delta) -> void {

    // TODO(DavoSK): Move it to MafiaSDK
    // fix shooting ( fixed ammo for now :) )
    *(BYTE*)((DWORD)ped->ped + 0x4A4) = 50;
    *(BYTE*)((DWORD)ped->ped + 0x4A8) = 50;
    *(float*)((DWORD)ped->ped + 0x5F4) = ped->aim;

    auto player_int = ped->ped->GetInterface();
    if (player_int->playersCar || player_int->carLeavingOrEntering || ped->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)
        return;

    // Disable animation bug
    if (player_int->isInAnimWithCar)
        player_int->isInAnimWithCar = 0;

    target_position_update(ped);
    target_rotation_update(ped);

    // Pose interpolation
    {
        zpl_vec3 compensation;
        zpl_vec3_lerp(&compensation, ped->interp.pose.start, ped->interp.pose.target, 0.4f);

        S_vector mafia_pose = EXPAND_VEC(compensation);

        if (ped->is_aiming)
            ped->ped->PoseSetPoseAimed(mafia_pose);
        else
            ped->ped->PoseSetPoseNormal(mafia_pose);
    }
}

inline auto entityupdate(librg_event* evnt) -> void {
    
    //NOTE(DavoSK): We need to read data before we can skip event ! 
    zpl_vec3 recv_pose, recv_rotation;
    librg_data_rptr(evnt->data, &recv_rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &recv_pose, sizeof(zpl_vec3));
    auto health         = librg_data_rf32(evnt->data);
    u8 animation_state  = librg_data_ru8(evnt->data);
    u8 is_crouching     = librg_data_ru8(evnt->data);
    u8 is_aiming        = librg_data_ru8(evnt->data);
    u32 aiming_time     = librg_data_ru32(evnt->data);
    f32 aim             = librg_data_rf32(evnt->data);
    u32 ping            = librg_data_ru32(evnt->data);

    auto player = (mafia_player *)evnt->entity->user_data;
    if (!player || !player->ped) {
        librg_event_reject(evnt);
        return;
    }
    
    //NOTE(DavoSK): If player exists we update mafia_player structure and gameobject
    auto player_int = player->ped->GetInterface();
    player->animation_state     = animation_state;
    player->is_crouching        = is_crouching;
    player->is_aiming           = is_aiming;
    player->aiming_time         = aiming_time;
    player->aim                 = aim;
    player->ping                = ping;

    player->interp.pose.start = player->interp.pose.target;
    player->interp.pose.target = recv_pose;

    target_position_set(player, evnt->entity->position, GlobalConfig.interp_time_player);
    target_rotation_set(player, recv_rotation, GlobalConfig.interp_time_player);

    if (!player_int->carLeavingOrEntering) {
        player_int->animState	= player->animation_state;
        player_int->isDucking	= player->is_crouching;
        player_int->isAiming	= player->is_aiming;
        *(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;
        *(float*)((DWORD)player_int + 0x5F4) = player->aim;
    }

    if (player->vehicle_id != -1 && (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
            
            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == evnt->entity->id) {
                    player->ped->Intern_UseCar(vehicle->car, i);
                    break;
                }
            }
        }
    }

    if (evnt->entity->id != local_player.entity_id) {
        player->health = health;
    }
}

inline auto entityremove(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    if (player && player->ped) {
        evnt->entity->flags &= ~ENTITY_INTERPOLATED;

        if (player->vehicle_id != -1) {
            auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
            if (vehicle_ent && vehicle_ent->user_data) {
                auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

                for (int i = 0; i < 4; i++) {
                    if (vehicle->seats[i] == evnt->entity->id) {

                        vehicle->seats[i] = -1;
                        player->vehicle_id = -1;

                        if (player->ped) {
                            player->ped->Intern_FromCar();
                        }

                        break;
                    }
                }
            }
        }

        despawn(player->ped);
        delete player;
        evnt->entity->user_data = nullptr;
    }
}

inline auto clientstreamer_update(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    
    if (!player) {
        librg_event_reject(evnt);
        return;
    }

    auto player_int = reinterpret_cast<MafiaSDK::C_Player*>(player->ped)->GetInterface();
    if (!player_int || !player_int->humanObject.entity.frame || 
        !player_int->humanObject.entity.frame->GetInterface()) {
        librg_event_reject(evnt);
        return;
    }

    auto frame_position = player_int->humanObject.entity.frame->GetInterface()->mPosition;

    evnt->entity->position	= EXPAND_VEC(frame_position);
    player->rotation		= EXPAND_VEC(player_int->humanObject.entity.rotation);
    player->pose			= local_player.pose;
    player->health          = player_int->humanObject.health;
    player->animation_state = player_int->humanObject.animStateLocal;
    player->is_crouching	= player_int->humanObject.isDucking;
    player->is_aiming		= player_int->humanObject.isAiming;
    player->aiming_time		= *(DWORD*)((DWORD)player_int + 0xAD4);
    player->aim             = *(float*)((DWORD)player_int + 0x5F4);

    librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wf32(evnt->data, player->health);
    librg_data_wu8(evnt->data, player->animation_state);
    librg_data_wu8(evnt->data, player->is_crouching);
    librg_data_wu8(evnt->data, player->is_aiming);
    librg_data_wf32(evnt->data, player->aim);
    librg_data_wu64(evnt->data, player->aiming_time);

    if (player->vehicle_id != -1 && (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
            
            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == evnt->entity->id) {
                    player->ped->Intern_UseCar(vehicle->car, i);
                    break;
                }
            }
        }
    }
}
