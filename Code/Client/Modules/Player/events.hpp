#pragma once

// =======================================================================//
// !
// ! Position Interpolation
// !
// =======================================================================//

void target_position_update(mafia_player *player) {

    auto player_int = player->ped->GetInterface();
    zpl_vec3 current_position = EXPAND_VEC(player_int->entity.position);
    zpl_vec3 new_position = lib_inter_interpolate(player->interp.pos, current_position);

    constexpr float distance_threshold = 2.0f;
    zpl_vec3 distance;
    zpl_vec3_sub(&distance, current_position, player->interp.pos->target);

    if (zpl_vec3_mag(distance) > distance_threshold) {
        new_position = player->interp.pos->target;
        lib_inter_reset(player->interp.pos, new_position);
    }

    player_int->entity.position = EXPAND_VEC(new_position);
}

void target_position_set(mafia_player *player, zpl_vec3 target_pos) {

    auto player_int = player->ped->GetInterface();
    target_position_update(player);

    zpl_vec3 local_pos = EXPAND_VEC(player_int->entity.position);
    lib_inter_set_target(player->interp.pos, local_pos, target_pos);
}

// =======================================================================//
// !
// ! Rotation Interpolation
// !
// =======================================================================//

void target_rotation_update(mafia_player *player) {

    auto player_int = player->ped->GetInterface();
    zpl_vec3 rotation = EXPAND_VEC(player_int->entity.rotation);
    zpl_vec3 new_rotation = lib_inter_interpolate(player->interp.rot, rotation);
    player_int->entity.rotation = EXPAND_VEC(new_rotation);
}

void target_rotation_set(mafia_player *player, zpl_vec3 target_rot) {

    target_rotation_update(player);
    auto player_int = player->ped->GetInterface();

    zpl_vec3 rotation = EXPAND_VEC(player_int->entity.rotation);
    lib_inter_set_target(player->interp.rot, rotation, target_rot);
}

// =======================================================================//
// !
// ! Pose Interpolation
// !
// =======================================================================//

void target_pose_update(mafia_player* player) {

    //NOTE(DavoSK): Move current pose into MafiaSDK
    zpl_vec3 current_player_pose = *(zpl_vec3*)((DWORD)player->ped + 0xA7C);
    zpl_vec3 new_pose = lib_inter_interpolate(player->interp.pose, current_player_pose);

    S_vector mafia_pose = EXPAND_VEC(new_pose);

    if (player->is_aiming)
        player->ped->PoseSetPoseAimed(mafia_pose);
    else
        player->ped->PoseSetPoseNormal(mafia_pose);
}

void target_pose_set(mafia_player* player, zpl_vec3 target_pose) {

    //NOTE(DavoSK): Move current pose into MafiaSDK
    zpl_vec3 current_player_pose = *(zpl_vec3*)((DWORD)player->ped + 0xA7C);
    lib_inter_set_target(player->interp.pose, current_player_pose, target_pose);
}

inline auto entitycreate(librg_event* evnt) -> void {

    auto player					= new mafia_player();
    player->vehicle_id			= librg_data_ri32(evnt->data);
    player->streamer_entity_id	= librg_data_ri32(evnt->data);

    player->interp.pos          = lib_inter_create_interpolator(GlobalConfig.interp_time_player, false);
    player->interp.rot          = lib_inter_create_interpolator(GlobalConfig.interp_time_player, false);
    player->interp.pose         = lib_inter_create_interpolator(GlobalConfig.interp_time_player, false);

    if (player->vehicle_id != -1) {
        player->clientside_flags |= CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
    }

    librg_data_rptr(evnt->data, &player->position, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, player->model, sizeof(char) * 32);
    librg_data_rptr(evnt->data, player->name, sizeof(char) * 32);

    player->is_crouching = librg_data_ru8(evnt->data);
    player->is_aiming = librg_data_ru8(evnt->data);
    librg_data_rptr(evnt->data, &player->inventory, sizeof(player_inventory));
    player->current_weapon_id = librg_data_ru32(evnt->data);
    player->health = librg_data_rf32(evnt->data);
    player->is_visible_on_map = librg_data_ru8(evnt->data);
    player->has_visible_nameplate = librg_data_ru8(evnt->data);

    auto new_ped = spawn(
        player->position,
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

    evnt->entity->flags |= ENTITY_INTERPOLATED;

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

#ifndef OAKWOOD_DISABLE_INTERPOLATION
    target_position_update(ped);
    target_rotation_update(ped);
    target_pose_update(ped);
#endif

    //Car shooting interpolation
    if (ped->interp.car_shooting.alpha <= 1.0f) {
        ped->interp.car_shooting.alpha += delta;

        float interpolated_aim = zpl_lerp(ped->interp.car_shooting.start, ped->interp.car_shooting.target, ped->interp.car_shooting.alpha);
        *(float*)((DWORD)player_int + 0x5F4) = interpolated_aim;
    }
}

inline auto entityupdate(librg_event* evnt) -> void {

    //NOTE(DavoSK): We need to read data before we can skip event !
    zpl_vec3 recv_pose, recv_rotation, recv_position;
    librg_data_rptr(evnt->data, &recv_position, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &recv_rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &recv_pose, sizeof(zpl_vec3));
    auto health = librg_data_rf32(evnt->data);
    u8 animation_state = librg_data_ru8(evnt->data);
    u8 is_crouching = librg_data_ru8(evnt->data);
    u8 is_aiming = librg_data_ru8(evnt->data);
    u32 aiming_time = librg_data_ru32(evnt->data);
    f32 aim = librg_data_rf32(evnt->data);
    u32 ping = librg_data_ru32(evnt->data);

    auto player = (mafia_player*)evnt->entity->user_data;
    if (!player || !player->ped) {
        librg_event_reject(evnt);
        return;
    }

    if (player->vehicle_id != -1 && (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == evnt->entity->id) {
                    player->ped->Intern_UseCar(vehicle->car, i);
                    printf("UseCar: %d(%X) seat: %d\n", player->vehicle_id, vehicle->car, i);
                    player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
                    break;
                }
            }
        }
    }

    if (player->ped == MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()) {
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

    player->interp.car_shooting.start = *(float*)((DWORD)player_int + 0x5F4);
    player->interp.car_shooting.target = aim;
    player->interp.car_shooting.alpha = 0.0f;
    player->interp.car_shooting.start_time = zpl_time_now();

#ifdef OAKWOOD_DISABLE_INTERPOLATION
    player_int->entity.position = EXPAND_VEC(recv_position);
    player_int->entity.rotation = EXPAND_VEC(recv_rotation);
#else
    target_position_set(player, recv_position);
    target_rotation_set(player, recv_rotation);
    target_pose_set(player, recv_pose);
#endif
    if (!player_int->carLeavingOrEntering) {
        player_int->animState	= player->animation_state;
        player_int->isDucking	= player->is_crouching;
        player_int->isAiming	= player->is_aiming;
        *(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;
    }

    if (evnt->entity->id != local_player.entity_id) {
        player->health = health;
    }

    if (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_SKIN) {
        if (!player->ped->GetInterface()->playersCar && !player->ped->GetInterface()->carLeavingOrEntering) {
            player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_SKIN;
            ((MafiaSDK::C_Human*)player->ped)->Intern_ChangeModel(player->model);
        }
    }
}

inline auto entityremove(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    if (player && player->ped) {
        evnt->entity->flags &= ~ENTITY_INTERPOLATED;

        lib_inter_destroy_interpolator(player->interp.pos);
        lib_inter_destroy_interpolator(player->interp.rot);
        lib_inter_destroy_interpolator(player->interp.pose);

        if (player->vehicle_id != -1) {
            auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
            if (vehicle_ent && vehicle_ent->user_data) {
                auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

                for (int i = 0; i < 4; i++) {
                    if (vehicle->seats[i] == evnt->entity->id) {

                        player->vehicle_id = -1;

                        if (player->ped) {
                            player->ped->EraseDynColls();
                            player->ped->Intern_FromCar();
                        }

                        break;
                    }
                }
            }
        }

        despawn(player->ped);
        delete (mafia_player *)player;
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

    auto frame_position = player_int->humanObject.entity.frame->GetInterface()->position;
    auto cam_position = frame_position; // NOTE: fallback position if camera is lost somehow
    auto cam = MafiaSDK::GetCurrentCamera();

    if (cam) {
        cam_position = cam->GetInterface()->position;
    }

    evnt->entity->position  = EXPAND_VEC(cam_position);
    player->position    	= EXPAND_VEC(frame_position);
    player->rotation		= EXPAND_VEC(player_int->humanObject.entity.rotation);
    player->pose			= local_player.pose;
    player->health          = player_int->humanObject.health;
    player->animation_state = player_int->humanObject.animStateLocal;
    player->is_crouching	= player_int->humanObject.isDucking;
    player->is_aiming		= player_int->humanObject.isAiming;
    player->aiming_time		= *(DWORD*)((DWORD)player_int + 0xAD4);
    player->aim             = *(float*)((DWORD)player_int + 0x5F4);

    librg_data_wptr(evnt->data, &player->position, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wf32(evnt->data, player->health);
    librg_data_wu8(evnt->data, player->animation_state);
    librg_data_wu8(evnt->data, player->is_crouching);
    librg_data_wu8(evnt->data, player->is_aiming);
    librg_data_wf32(evnt->data, player->aim);
    librg_data_wu64(evnt->data, player->aiming_time);

    if (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_DEATH) {
        if (!player->ped->GetInterface()->carLeavingOrEntering) {
            player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_DEATH;
            player->ped->Intern_ForceDeath();
        }
    }

    if (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_SKIN) {
        if (!player->ped->GetInterface()->playersCar && !player->ped->GetInterface()->carLeavingOrEntering) {
            player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_SKIN;
            ((MafiaSDK::C_Human*)player->ped)->Intern_ChangeModel(player->model);
        }
    }

    if (player->vehicle_id != -1 && (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == evnt->entity->id) {
                    player->ped->Intern_UseCar(vehicle->car, i);
                    player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
                    break;
                }
            }
        }
    }
}
