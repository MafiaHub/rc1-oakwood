/**
 * Put a specified player into a specified
 * vehicle with specified seat id
 * @param  vid
 * @param  pid
 * @param  seat
 * @return
 */
int oak_vehicle_player_put(oak_vehicle vid, oak_player pid, oak_seat_id seat_id) {
    auto player = oak_entity_player_get(pid);
    auto vehicle = oak_entity_vehicle_get(vid);

    if (oak_vehicle_seat_assign(vid, pid, seat_id) < 0) {
        return -1;
    }

    librg_send(&network_context, NETWORK_VEHICLE_PLAYER_PUT, data, {
        librg_data_went(&data, player->native_id);
        librg_data_went(&data, vehicle->native_id);
        librg_data_wi32(&data, seat_id);
    });

    librg_entity_visibility_set_for(oak_network_ctx_get(), player->native_id, vehicle->native_id, LIBRG_ALWAYS_VISIBLE);

    return 0;
}

/**
 * Get a seat id of specified player in a specified vehicle
 * @param  vid
 * @param  pid
 * @return
 */
int oak_vehicle_player_get(oak_vehicle vid, oak_player pid) {
    auto player = oak_entity_player_get(pid);
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!player || !vehicle)
        return -1;

    for (size_t i = 0; i < 4; i++) {
        if (vehicle->seats[i] == player->native_id)
            return i;
    }

    return -1;
}

int oak_vehicle_player_remove(oak_vehicle vid, oak_player pid) {
    auto vehicle = oak_entity_vehicle_get(vid);
    auto player = oak_entity_player_get(pid);

    if (!player || !vehicle)
        return -1;

    for (u32 i = 0; i < OAK_MAX_SEATS; i++) {
        if (vehicle->seats[i] == player->native_id) {
            vehicle->seats[i] = -1;
            player->vehicle_id = -1;

            mod_message_send(&network_context, NETWORK_VEHICLE_PLAYER_REMOVE, [&](librg_data *data) {
                librg_data_went(data, player->native_id);
                librg_data_went(data, vehicle->native_id);
                librg_data_wu32(data, i);
            });

            if (i == 0) {
                oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
            }

            librg_entity_visibility_set_for(oak_network_ctx_get(), player->native_id, vehicle->native_id, LIBRG_DEFAULT_VISIBILITY);

            break;
        }
    }

    return 0;
}

/**
 * Get a current vehicle that player is being driver/passanger of
 * @param  pid
 * @return
 */
oak_vehicle oak_vehicle_player_inside(oak_player pid) {
    auto player = oak_entity_player_get(pid);

    if (!player)
        return -1;

    if (player->vehicle_id != -1) {
        auto vehicle = librg_entity_fetch(&network_context, player->vehicle_id);
        return oak_entity_get_id_from_native(vehicle);
    }

    return -2;
}

/**
 * Get a current seat ID player is sitting in
 * @param  vid
 * @param  pid
 * @return
 */
oak_seat_id oak_vehicle_player_seat_get(oak_vehicle vid, oak_player pid) {
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!vehicle) {
        return -1;
    }

    auto player = oak_entity_player_get(pid);

    if (!player) {
        return -2;
    }

    for (int i = 0; i < OAK_MAX_SEATS; ++i)
    {
        if (player->native_id == vehicle->seats[i]) {
            return i;
        }
    }

    return -3;
}

/**
 * Get player at a specific seat ID
 * @param  vid
 * @param  seat_id
 * @return
 */
oak_player oak_vehicle_player_seat_player_get(oak_vehicle vid, oak_seat_id seat_id) {
    if (seat_id < 0 || seat_id >= OAK_MAX_SEATS) {
        return -1;
    }

    auto vehicle = oak_entity_vehicle_get(vid);

    if (!vehicle) {
        return -2;
    }

    int player_id = vehicle->seats[seat_id];
    auto player_ent = librg_entity_fetch(oak_network_ctx_get(), player_id);

    if (!player_ent) {
        return -3;
    }

    auto player = oak_entity_get_id_from_native(player_ent);

    return player;
}

/**
 * Enter a vehicle seat
 * @param  vid
 * @param  pid
 * @param  seat_id
 * @param  enterFromPassengerSeat
 * @return
 */
int oak_vehicle_player_enter(oak_vehicle vid, oak_player pid, oak_seat_id seat_id, int enterFromPassengerSeat) {
    auto vehicle = oak_entity_vehicle_get(vid);
    auto player = oak_entity_player_get(pid);

    if (seat_id < 0 || seat_id >= OAK_MAX_SEATS) {
        return -1;
    }

    if (!vehicle || !player) {
        return -2;
    }

    if (enterFromPassengerSeat && seat_id == 1) {
        seat_id = 0;
    }

    vehicle->seats[seat_id] = player->native_id;
    player->vehicle_id = vehicle->native_id;

    if (seat_id == 0) {
        librg_entity_control_set(oak_network_ctx_get(), vehicle->native_id,
            player->native_entity->client_peer);
    }

    librg_entity_visibility_set_for(oak_network_ctx_get(), player->native_id, vehicle->native_id, LIBRG_ALWAYS_VISIBLE);

    return 0;
}

/**
 * Leave a vehicle seat
 * @param  vid
 * @param  pid
 * @param  seat_id
 * @return
 */
int oak_vehicle_player_leave(oak_vehicle vid, oak_player pid, oak_seat_id seat_id) {
    auto vehicle = oak_entity_vehicle_get(vid);
    auto player = oak_entity_player_get(pid);

    if (seat_id < 0 || seat_id >= OAK_MAX_SEATS) {
        return -1;
    }

    if (!vehicle || !player) {
        return -2;
    }

    if (seat_id == 0) {
        for (int i = 0; i < OAK_MAX_SEATS; i++) {
            if (vehicle->seats[i] == player->native_id) {
                seat_id = i;
                break;
            }
        }
    }

    vehicle->seats[seat_id] = -1;
    player->vehicle_id = -1;

    if (seat_id == 0) {
        oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
    }

    librg_entity_visibility_set_for(oak_network_ctx_get(), player->native_id, vehicle->native_id, LIBRG_DEFAULT_VISIBILITY);

    return 0;
}

/**
 * Throw player from a vehicle (hijack)
 * @param  oak_vehicle
 * @param  oak_player
 * @param  seat_id
 * @return
 */
int oak_vehicle_player_hijack(oak_vehicle vid, oak_player pid, oak_seat_id seat_id) {
    auto vehicle = oak_entity_vehicle_get(vid);
    auto player = oak_entity_player_get(pid);

    if (seat_id < 0 || seat_id >= OAK_MAX_SEATS) {
        return -1;
    }

    if (!vehicle || !player) {
        return -2;
    }

    if (vehicle->seats[seat_id] == -1) {
        return -3;
    }

    mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_HIJACK, [&](librg_data *data) {
        librg_data_went(data, player->native_id);
        librg_data_went(data, vehicle->native_id);
        librg_data_wi32(data, seat_id);
    });

    auto driver_ent = librg_entity_fetch(oak_network_ctx_get(), vehicle->seats[seat_id]);
    auto driver = oak_entity_player_get_from_native(driver_ent);

    if (driver) {
        driver->vehicle_id = -1;
    }

    vehicle->seats[seat_id] = -1;

    if (seat_id == 0) {
        oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
    }

    librg_entity_visibility_set_for(oak_network_ctx_get(), player->native_id, vehicle->native_id, LIBRG_DEFAULT_VISIBILITY);

    return 0;
}

oak_player *oak_vehicle_player_list(oak_vehicle vid, int *count) {
    zpl_local_persist oak_player buffer[OAK_MAX_SEATS] = {};
    int length = 0;

    /* clean up old data */
    zpl_memset(buffer, 0, OAK_MAX_SEATS * sizeof(oak_player));

    if (oak_vehicle_invalid(vid)) return NULL;
    auto vehicle = oak_entity_vehicle_get(vid);

    for (int i=0; i<OAK_MAX_SEATS; ++i) {
        librg_entity *entity = librg_entity_fetch(oak_network_ctx_get(), vehicle->seats[i]);
        if (!entity) continue;

        oak_player pid = oak_entity_get_id_from_native(entity);
        if (oak_player_invalid(pid)) continue;

        buffer[length++] = pid;
    }

    if (count) *count = length;
    return buffer;
}
