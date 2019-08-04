/**
 * Put a specified player into a specified
 * vehicle with specified seat id
 * @param  vid
 * @param  pid
 * @param  seat
 * @return
 */
int oak_vehicle_player_put(oak_vehicle vid, oak_player pid, int seat_id) {
    auto player = oak_entity_player_get(pid);
    auto vehicle = oak_entity_vehicle_get(vid);

    if (!player || !vehicle)
        return -1;

    if (vehicle->seats[seat_id] != -1)
        return -2;

    if (player->vehicle_id != -1)
        return -3;

    if (seat_id < 0 || seat_id > 3)
        return -4;

    vehicle->seats[seat_id] = player->librg_id;
    player->vehicle_id = vehicle->librg_id;

    if (seat_id == 0 && vehicle->seats[0] == -1) {
        librg_entity_control_set(&network_context, vehicle->librg_id, player->librg_entity->client_peer);
    }

    librg_send(&network_context, NETWORK_PLAYER_PUT_TO_VEHICLE, data, {
        librg_data_went(&data, player->librg_id);
        librg_data_went(&data, vehicle->librg_id);
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
        if (vehicle->seats[i] == player->librg_id)
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
        return (oak_vehicle)vehicle->user_data;
    }

    return -2;
}
