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

int oak_vehicle_player_remove(oak_vehicle, oak_player) {
    ZPL_ASSERT_MSG(0, "oak_vehicle_player_remove: not implemented");
    return -1;
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
        return oak_entity_get_id_from_librg(vehicle);
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

    auto player = oak_entity_get_id_from_librg(player_ent);

    return player;
}
