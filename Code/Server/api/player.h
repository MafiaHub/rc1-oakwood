// =======================================================================//
// !
// ! General
// !
// =======================================================================//

/**
 * Allocate a player struct and store it in the player buffer (sike)
 * (should be used on successful connection)
 * @param  event
 * @return
 */
oak_player oak_player_create(librg_event *e) {
    auto oak_id = oak_entity_next(OAK_PLAYER);
    auto entity = oak_entity_player_get(oak_id);

    entity->native_id = e->entity->id;
    entity->native_entity = e->entity;
    entity->reset();

    /* store index of our entity in user_data */
    e->entity->stream_range = 500.0f;
    e->entity->user_data = cast(void*)(uintptr)oak_id;
    oak_player_visibility_set(oak_id, OAK_VISIBILITY_ICON, 1);

    /* set our player to be our controller */
    librg_entity_control_set(oak_network_ctx_get(), e->entity->id, e->peer);
    oak_log("[info] player connected with oakid: %d\n", oak_id);

    return oak_id;
}

/**
 * Deallocate player struct and remove it from the buffer
 * @param  event
 * @return
 */
int oak_player_destroy(librg_event *e) {
    return oak_entity_free(OAK_PLAYER, (uintptr)e->entity->user_data);
}

/**
 * Check if player is invalid
 * @param  id
 * @return
 */
int oak_player_invalid(oak_player id) {
    return oak_entity_invalid(OAK_PLAYER, id);
}

// =======================================================================//
// !
// ! Actions
// !
// =======================================================================//

/**
 * Spawn a player ped in the world
 * @param  id
 * @return
 */
int oak_player_spawn(oak_player id) {
    if (oak_player_invalid(id)) return -1;

    auto player = oak_entity_player_get(id);
    auto entity = player->native_entity;

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SPAWN, data, {
        librg_data_wu32(&data, entity->id);
        librg_data_wptr(&data, &entity->position, sizeof(zpl_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        librg_data_wu32(&data, player->current_weapon_id);
        librg_data_wf32(&data, player->health);
    });

    return 0;
}

/**
 * Remove player ped from the world
 * @param  id
 * @return
 */
int oak_player_despawn(oak_player id) {
    if (oak_player_invalid(id)) return -1;

    auto player = oak_entity_player_get(id);

    if (player->vehicle_id != -1) {
        if (librg_entity_valid(oak_network_ctx_get(), player->vehicle_id)) {
            auto vid = oak_entity_vehicle_get_from_native(
                librg_entity_fetch(oak_network_ctx_get(), player->vehicle_id))->oak_id;

            oak_vehicle_player_remove(vid, player->oak_id);
        }
    }

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_DESPAWN, data, {
        librg_data_went(&data, player->native_id);
    });

    return 0;
}

/**
 * Kill a specified player
 * @param  id
 * @return
 */
int oak_player_kill(oak_player id) {
    if (oak_player_invalid(id)) return -1;
    auto res = oak_player_health_set(id, 0.0f);
    auto player = oak_entity_player_get(id);

    if (player->vehicle_id != -1) {
        auto vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), player->vehicle_id);

        if (!vehicle_ent) {
            player->vehicle_id = -1;
            return -2;
        }

        auto vehicle = oak_entity_vehicle_get_from_native(vehicle_ent);

        if (vehicle) {
            oak_vehicle_player_remove(vehicle->oak_id, player->oak_id);
        }
    }

    return res;
}

int oak_player_kick(oak_player id, const char *reason, int length) {
    // todo: send reason

    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    librg_network_kick(oak_network_ctx_get(), player->native_entity->client_peer);
    return 0;
}

/**
 * Play a speicific animation for specific player
 * Sends message to all players
 * @param  id
 * @param  text
 * @return
 */
int oak_player_playanim(oak_player id, const char *text, int length) {
    if (oak_player_invalid(id)) return -1;

    auto entity = oak_entity_player_get(id);
    char animation[32] = {};
    zpl_strncpy(animation, text, length);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_PLAY_ANIMATION, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, animation, sizeof(char) * 32);
    });

    return 0;
}

// =======================================================================//
// !
// ! STREAM SETTERS (auto-streamed values)
// !
// =======================================================================//

int oak_player_health_set(oak_player id, float health) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    entity->health = health;

    if (entity->died_ingame) {
        librg_send_except(oak_network_ctx_get(), NETWORK_PLAYER_SET_HEALTH, entity->native_entity->client_peer, data, {
            librg_data_went(&data, entity->native_id);
            librg_data_wf32(&data, health);
        });
    }
    else {
        librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_HEALTH, data, {
            librg_data_went(&data, entity->native_id);
            librg_data_wf32(&data, health);
        });
    }

    return 0;
}

int oak_player_position_set(oak_player id, oak_vec3 position) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    // TODO: fix bug with interpolation on client side
    entity->native_entity->position = EXPAND_VEC(position);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_POS, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, &position, sizeof(zpl_vec3));
    });

    return 0;
}

int oak_player_direction_set(oak_player id, oak_vec3 direction) {
    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    player->rotation = EXPAND_VEC(direction);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_ROT, data, {
        librg_data_went(&data, player->native_id);
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
    });

    return 0;
}

int oak_player_heading_set(oak_player id, float angle) {
    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    player->rotation = ComputeDirVector(angle);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_ROT, data, {
        librg_data_went(&data, player->native_id);
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
    });

    return 0;
}

// =======================================================================//
// !
// ! DATA SETTERS (action-based values)
// !
// =======================================================================//

/**
 * Set player model to a specific model
 * Event is sent to all users
 * @param  id
 * @param  model
 * @return
 */
int oak_player_model_set(oak_player id, const char *model, int length) {
    if (oak_player_invalid(id)) return -1;

    auto entity = oak_entity_player_get(id);

    zpl_memset(entity->model, 0, OAK_PLAYER_MODEL_SIZE);
    zpl_memcopy(entity->model, model, length);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_MODEL, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, (void *)entity->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
    });

    return 0;
}

// =======================================================================//
// !
// ! GETTERS
// !
// =======================================================================//

const char *oak_player_name_get(oak_player id) {
    int code = oak_player_invalid(id);

    /* entity is valid, or exists, but marked as invalid */
    /* extract raw data to access values even if player removed (disconnected) */
    if (code == 0 || code == 3) {
        return oak__entities_data.players[id].name;
    }

    return nullptr;
}

const char *oak_player_model_get(oak_player id) {
    return oak_player_invalid(id) ? nullptr : oak_entity_player_get(id)->model;
}

float oak_player_health_get(oak_player id) {
    return oak_player_invalid(id) ? -1.0f : oak_entity_player_get(id)->health;
}

oak_vec3 oak_player_position_get(oak_player id) {
    oak_vec3 temp = {-1,-1,-1};

    if (!oak_player_invalid(id))
        zpl_memcopy(&temp, &oak_entity_player_get(id)->position, sizeof(oak_vec3));

    return temp;
}

oak_vec3 oak_player_direction_get(oak_player id) {
    oak_vec3 temp = { -1,-1,-1 };

    if (!oak_player_invalid(id))
        zpl_memcopy(&temp, &oak_entity_player_get(id)->rotation, sizeof(oak_vec3));

    return temp;
}

float oak_player_heading_get(oak_player id) {
    float temp = -1.0f;

    if (!oak_player_invalid(id))
        temp = DirToRotation180(oak_entity_player_get(id)->rotation);

    return temp;
}

// =======================================================================//
// !
// ! PLAYER VISIBILITY
// !
// =======================================================================//

/**
 * Set different player visibility values
 * @param  id
 * @param  type
 * @param  state
 * @return
 */
int oak_player_visibility_set(oak_player id, oak_visiblity_type type, int state) {
    auto entity = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(entity);

    switch (type) {
        case OAK_VISIBILITY_ICON: {
            entity->is_visible_on_map = state;

            librg_send(oak_network_ctx_get(), NETWORK_PLAYER_MAP_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_NAME: {
            entity->has_visible_nameplate = state;

            librg_send(oak_network_ctx_get(), NETWORK_PLAYER_NAMEPLATE_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        default:
            ZPL_ASSERT_MSG(0, "oak_player_visibility_set: specified visibility type is not implemented!");
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
int oak_player_visibility_get(oak_player id, oak_visiblity_type type) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    switch (type) {
        case OAK_VISIBILITY_ICON: return (int)entity->is_visible_on_map; break;
        case OAK_VISIBILITY_NAME: return (int)entity->has_visible_nameplate; break;
        default: return -1;
    }
}
