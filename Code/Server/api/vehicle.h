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
oak_vehicle oak_vehicle_spawn(const char *model, int length) {
    auto oak_id = oak_entity_next(OAK_VEHICLE);
    auto entity = oak_entity_vehicle_get(oak_id);
    auto native = librg_entity_create(&network_context, OAK_VEHICLE);

    entity->reset();
    entity->librg_id = native->id;
    entity->librg_entity = native;
    native->user_data = (void *)(uintptr)oak_id;
    native->position = {0};

    entity->health             = 100.0f;
    entity->engine_health      = 100.0f;
    entity->fuel               = 60.0f;
    entity->sound_enabled      = 1;
    entity->is_car_in_radar    = true;
    entity->rot_forward        = ComputeDirVector(0.0f);
    entity->rot_up             = { 0.0f, 1.0f, 0.0f };

    zpl_memset(entity->model, 0, OAK_VEHICLE_MODEL_SIZE);
    zpl_memcopy(entity->model, model, length);

    // TODO: refactor
    mod_vehicle_assign_nearest_player(&network_context, native);

    return oak_id;
}

/**
 * Remove a vehicle from server
 * @param  id
 * @return
 */
int oak_vehicle_despawn(oak_vehicle id) {
    if (oak_vehicle_invalid(id)) return -1;

    // TODO: refactor
    auto vehicle = oak_entity_vehicle_get(id);

    for(size_t i = 0; i < 4; i++) {
        auto pid = vehicle->seats[i];

        if (pid == -1) continue;

        auto player_ent = librg_entity_fetch(&network_context, pid);

        if (player_ent && player_ent->user_data) {
            auto player = oak_entity_player_get((oak_player)player_ent->user_data);

            player->vehicle_id = -1;

            mod_message_send(&network_context, NETWORK_VEHICLE_PLAYER_REMOVE, [&](librg_data *data) {
                librg_data_went(data, player_ent->id);
                librg_data_went(data, vehicle->librg_id);
                librg_data_wu32(data, i);
            });
        }
    }

    mod_message_send(&network_context, NETWORK_VEHICLE_GAME_DESTROY, [&](librg_data * data) {
        librg_data_went(data, vehicle->librg_id);
    });

    librg_entity_destroy(&network_context, vehicle->librg_id);
    oak_entity_free(OAK_VEHICLE, id);
    vehicle->librg_entity->user_data = nullptr;

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

    librg_send(&network_context, NETWORK_VEHICLE_REPAIR, data, {
        librg_data_went(&data, entity->librg_id);
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
    librg_entity_control_ignore_next_update(&network_context, entity->librg_id);
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
    librg_entity_control_ignore_next_update(&network_context, entity->librg_id);
    entity->rot_forward = hard_cast(zpl_vec3*)direction;

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

    librg_entity_control_ignore_next_update(&network_context, entity->librg_id);
    entity->librg_entity->position = hard_cast(zpl_vec3*)(position);

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

    librg_send(&network_context, NETWORK_VEHICLE_SET_TRANSPARENCY, data, {
        librg_data_went(&data, entity->librg_id);
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
    return hard_cast(oak_vec3*)entity->librg_entity->position;
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

            librg_send(&network_context, NETWORK_VEHICLE_MAP_VISIBILITY, data, {
                librg_data_went(&data, entity->librg_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_RADAR: {
            entity->is_car_in_radar = state;

            librg_send(&network_context, NETWORK_VEHICLE_RADAR_VISIBILITY, data, {
                librg_data_went(&data, entity->librg_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_COLLISION: {
            entity->collision_state = state;

            librg_send(&network_context, NETWORK_VEHICLE_SET_COLLISION_STATE, data, {
                librg_data_went(&data, entity->librg_id);
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
