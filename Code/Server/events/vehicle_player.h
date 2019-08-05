int oak_vehicle_player_register() {
    librg_network_add(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_HIJACK, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(oak_network_ctx_get(), msg->peer);
        auto vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), librg_data_rent(msg->data));
        auto seat = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_native(vehicle_ent);
        auto sender = oak_entity_player_get_from_native(sender_ent);

        if (!vehicle || !sender) {
            return;
        }

        oak_vehicle_player_hijack(vehicle->oak_id, sender->oak_id, seat);
    });

    librg_network_add(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_REMOVE, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(oak_network_ctx_get(), msg->peer);
        auto sender = oak_entity_player_get_from_native(sender_ent);

        if (!sender) {
            return;
        }

        if (sender->vehicle_id == -1) {
            return;
        }

        auto sender_vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), sender->vehicle_id);
        auto sender_vehicle = oak_entity_vehicle_get_from_native(sender_vehicle_ent);

        if (!sender_vehicle) {
            return;
        }

        oak_vehicle_player_remove(sender_vehicle->oak_id, sender->oak_id);
    });

    librg_network_add(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_USE_DOOR, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(oak_network_ctx_get(), msg->peer);
        auto vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), librg_data_ru32(msg->data));
        auto action = librg_data_ri32(msg->data);
        auto seat_id = librg_data_ri32(msg->data);
        i32 seat_original = seat_id;
        auto enter_from_passenger_seat = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_native(vehicle_ent);
        auto sender = oak_entity_player_get_from_native(sender_ent);

        if (vehicle && sender) {
            if(action == 1) {
                oak_vehicle_player_enter(vehicle->oak_id, sender->oak_id, seat_id, enter_from_passenger_seat);
            } else if (action == 2) {
                oak_vehicle_player_leave(vehicle->oak_id, sender->oak_id, seat_id);
            }

            mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_USE_DOOR, [&](librg_data *data) {
                librg_data_went(data, sender_ent->id);
                librg_data_went(data, vehicle_ent->id);
                librg_data_wi32(data, action);
                librg_data_wi32(data, seat_original);
                librg_data_wi32(data, enter_from_passenger_seat);
            });
        }
    });

    return 0;
}
