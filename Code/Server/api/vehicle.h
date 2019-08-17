// =======================================================================//
// !
// ! Actions
// !
// =======================================================================//

/**
 * Spawn a vehicle at 0,0,0
 * (use setters to change its values right after spawn)
 * @param model
 * @return
 */
oak_vehicle oak_vehicle_spawn(const char *model, int length, oak_vec3 position, float heading) {
    auto oak_id = oak_entity_next(OAK_VEHICLE);
    auto entity = oak_entity_vehicle_get(oak_id);
    auto native = librg_entity_create(oak_network_ctx_get(), OAK_VEHICLE);

    entity->reset();
    entity->native_id = native->id;
    entity->native_entity = native;
    native->user_data = (void *)(uintptr)oak_id;
    native->position = EXPAND_VEC(position);

    entity->health             = 100.0f;
    entity->engine_health      = 100.0f;
    entity->fuel               = 60.0f;
    entity->sound_enabled      = 1;
    entity->is_car_in_radar    = true;
    entity->rot_forward        = ComputeDirVector(heading);
    entity->rot_up             = { 0.0f, 1.0f, 0.0f };
    oak_vehicle_visibility_set(oak_id, OAK_VISIBILITY_ICON, 1);

    zpl_memset(entity->model, 0, OAK_VEHICLE_MODEL_SIZE);
    zpl_memcopy(entity->model, model, length);

    oak_vehicle_streamer_assign_nearest(entity->oak_id);

    return oak_id;
}

/**
 * Remove a vehicle from server
 * @param  id
 * @return
 */
int oak_vehicle_despawn(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return -1;

    auto vehicle = oak_entity_vehicle_get(id);

    for(size_t i = 0; i < OAK_MAX_SEATS; i++) {
        auto pid = vehicle->seats[i];

        if (pid == -1) continue;

        auto player_ent = librg_entity_fetch(oak_network_ctx_get(), pid);
        auto player = oak_entity_player_get_from_native(player_ent);

        player->vehicle_id = -1;

        mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_REMOVE, [&](librg_data *data) {
            librg_data_went(data, player_ent->id);
            librg_data_went(data, vehicle->native_id);
            librg_data_wu32(data, i);
        });
    }

    mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_GAME_DESTROY, [&](librg_data * data) {
        librg_data_went(data, vehicle->native_id);
    });

    librg_entity_destroy(oak_network_ctx_get(), vehicle->native_id);
    oak_entity_free(OAK_VEHICLE, id);
    vehicle->native_entity->user_data = nullptr;

    return 0;
}

/**
 * Check wether or not the vehicle id is valid
 * @param  id
 * @return
 */
int oak_vehicle_invalid(oak_vehicle id) {
    return oak_entity_invalid(OAK_VEHICLE, id);
}

/**
 * Immidiately repair a specified vehicle
 * @param  id
 * @return
 */
int oak_vehicle_repair(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);

    librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_REPAIR, data, {
        librg_data_went(&data, entity->native_id);
    });

    return 0;
}

// =======================================================================//
// !
// ! STREAM SETTERS (auto-streamed values)
// !
// =======================================================================//

/**
 * Set vehicle fuel level
 * @param  id
 * @param  fuel
 * @return
 */
int oak_vehicle_fuel_set(oak_vehicle id, float fuel) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);

    /* skip updates for the next change */
    librg_entity_control_ignore_next_update(oak_network_ctx_get(), entity->native_id);
    entity->fuel = fuel;

    return 0;
}

/**
 * Set vehicle fuel level
 * @param  id
 * @param  direction
 * @return
 */
int oak_vehicle_direction_set(oak_vehicle id, oak_vec3 direction) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);

    /* skip updates for the next change */
    librg_entity_control_ignore_next_update(oak_network_ctx_get(), entity->native_id);
    entity->rot_forward = hard_cast(zpl_vec3*)direction;

    librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_SET_DIR, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, &direction, sizeof(zpl_vec3));
    });

    return 0;
}

/**
 * Custom position setter method (using librg position)
 * @param  id
 * @param  position
 * @return
 */
int oak_vehicle_position_set(oak_vehicle id, oak_vec3 position) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);

    librg_entity_control_ignore_next_update(oak_network_ctx_get(), entity->native_id);
    entity->native_entity->position = EXPAND_VEC(position);

    librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_SET_POS, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, &position, sizeof(zpl_vec3));
    });

    return 0;
}

/**
 * Helper heading setting method
 * @param  id
 * @param  heading
 * @return
 */
int oak_vehicle_heading_set(oak_vehicle id, float heading) {
    if (oak_vehicle_invalid(id)) return -1;
    auto vec = ComputeDirVector(heading);
    return oak_vehicle_direction_set(id, hard_cast(oak_vec3*)vec);
}

// =======================================================================//
// !
// ! DATA SETTERS (action-based values)
// !
// =======================================================================//

/**
 * Set how transpared the vehicle should be rendered
 * @param  id
 * @param  transparency
 * @return
 */
int oak_vehicle_transparency_set(oak_vehicle id, float transparency) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);

    entity->transparency = transparency;

    librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_SET_TRANSPARENCY, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wf32(&data, transparency);
    });

    return 0;
}

// =======================================================================//
// !
// ! GETTERS
// !
// =======================================================================//

/**
 * Get current fuel level
 * @param  id
 * @return
 */
float oak_vehicle_fuel_get(oak_vehicle id) {
    return oak_vehicle_invalid(id) ? -1.0f : oak_entity_vehicle_get(id)->fuel;
}

/**
 * Get forward rotation
 * @param  id
 * @return
 */
oak_vec3 oak_vehicle_direction_get(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return {-1, -1, -1};
    auto vec =  oak_entity_vehicle_get(id)->rot_forward;
    return hard_cast(oak_vec3 *)vec;
}

/**
 * Get current entity position
 * @param  id
 * @return
 */
oak_vec3 oak_vehicle_position_get(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return {-1, -1, -1};
    auto entity = oak_entity_vehicle_get(id);
    return hard_cast(oak_vec3*)entity->native_entity->position;
}

/**
 * Get vehicle heading (+-180) based off direction
 * @param  id
 * @return
 */
float oak_vehicle_heading_get(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return -1;
    auto entity = oak_entity_vehicle_get(id);
    return DirToRotation180(entity->rot_forward);
}

float oak_vehicle_transparency_get(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return -1.0f;
    return oak_entity_vehicle_get(id)->transparency;
}

// =======================================================================//
// !
// ! VEHICLE VISIBILITY
// !
// =======================================================================//

/**
 * Set different visibility values
 * @param  id
 * @param  type
 * @param  state
 * @return
 */
int oak_vehicle_visibility_set(oak_vehicle id, oak_visiblity_type type, int state) {
    auto entity = oak_entity_vehicle_get(id); ZPL_ASSERT_NOT_NULL(entity);

    switch (type) {
        case OAK_VISIBILITY_ICON: {
            entity->is_visible_on_map = state;

            librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_MAP_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_RADAR: {
            entity->is_car_in_radar = state;

            librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_RADAR_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_COLLISION: {
            entity->collision_state = state;

            librg_send(oak_network_ctx_get(), NETWORK_VEHICLE_SET_COLLISION_STATE, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        default:
            ZPL_ASSERT_MSG(0, "oak_vehicle_visibility_set: specified visibility type is not implemented!");
            break;
    }

    return -1;
}

/**
 * Get current visibility value for specific type
 * @param  id
 * @param  type
 * @return
 */
int oak_vehicle_visibility_get(oak_vehicle id, oak_visiblity_type type) {
    auto entity = oak_entity_vehicle_get(id); ZPL_ASSERT_NOT_NULL(entity);

    switch (type) {
        case OAK_VISIBILITY_ICON: return (int)entity->is_visible_on_map; break;
        case OAK_VISIBILITY_RADAR: return (int)entity->is_car_in_radar; break;
        case OAK_VISIBILITY_COLLISION: return (int)entity->collision_state; break;
        default: return -1;
    }
}

/**
 * Attempts to put player into vehicle
 * @param  oak_vehicle
 * @param  oak_player
 * @param  oak_seat_id
 * @return
 */
int oak_vehicle_seat_assign(oak_vehicle vid, oak_player pid, oak_seat_id seat_id) {
    auto player = oak_entity_player_get(pid);
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!player || !vehicle)
        return -1;

    if (seat_id < 0 || seat_id >= OAK_MAX_SEATS)
        return -2;

    if (vehicle->seats[seat_id] != -1)
        return -3;

    if (player->vehicle_id != -1)
        return -4;

    vehicle->seats[seat_id] = player->native_id;
    player->vehicle_id = vehicle->native_id;

    if (seat_id == 0) {
        oak_vehicle_streamer_set(vid, pid);
    }

    return 0;
}

/**
 * Retrieve the current vehicle streamer
 * @param  oak_vehicle
 * @return
 */
oak_player oak_vehicle_streamer_get(oak_vehicle vid) {
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!vehicle)
        return -1;

    if (!librg_entity_valid(oak_network_ctx_get(), vehicle->native_id)) {
        return -2;
    }

    auto peer = librg_entity_control_get(oak_network_ctx_get(), vehicle->native_id);

    if (!peer) {
        return -3;
    }

    auto player_ent = librg_entity_find(oak_network_ctx_get(), peer);

    if (!player_ent) {
        return -4;
    }

    auto player = oak_entity_player_get_from_native(player_ent);

    if (!player) {
        return -5;
    }

    return player->oak_id;
}

/**
 * Assigns a new streamer to the vehicle
 * @param  oak_vehicle
 * @param  oak_player
 * @return
 */
int oak_vehicle_streamer_set(oak_vehicle vid, oak_player pid) {
    auto player = oak_entity_player_get(pid);
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!player || !vehicle)
        return -1;

    if (!librg_entity_valid(oak_network_ctx_get(), vehicle->native_id)) {
        return -2;
    }

    librg_entity_control_set(oak_network_ctx_get(), vehicle->native_id,
            player->native_entity->client_peer);

    return 0;
}

librg_entity *oak__vehicle_streamer_nearest_find(librg_entity *vehicle, int exception = -1) {
    int smallest_distance = 1000.0f;
    librg_entity *current_entity = nullptr;
    auto ctx = oak_network_ctx_get();

    librg_entity_id *list;
    auto amount = librg_entity_query(ctx, vehicle->id, &list);

    for (u32 i = 0; i < amount; i++) {
        librg_entity *entity = librg_entity_fetch(ctx, list[i]);
        if (entity->type != OAK_PLAYER || exception == entity->id) continue;

        zpl_vec3 final_vec;
        zpl_vec3_sub(&final_vec, entity->position, vehicle->position);
        float dist = zpl_vec3_mag(final_vec);
        if (dist < smallest_distance && dist < entity->stream_range) {
            smallest_distance = dist;
            current_entity = entity;
        }
    }

    return current_entity;
}

/**
 * Assign a nearest player to vehicle as a streamer
 * @param vid
 * @param exception
 */
int oak_vehicle_streamer_assign_nearest(oak_vehicle vid, int exception) {
    auto vehicle = oak_entity_vehicle_get(vid);
    auto ctx = oak_network_ctx_get();

    if (!vehicle) {
        return -1;
    }

    auto streamer = oak__vehicle_streamer_nearest_find(vehicle->native_entity, exception);

    if (streamer != nullptr) {
        librg_entity_control_set(ctx, vehicle->native_id, streamer->client_peer);
    }
    else {
        librg_entity_control_remove(ctx, vehicle->native_id);
        return -2;
    }

    return 0;
}
