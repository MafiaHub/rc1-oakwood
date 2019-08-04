int oak_vehicle_player_register() {
    librg_network_add(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_HIJACK, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(oak_network_ctx_get(), msg->peer);
        auto vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), librg_data_rent(msg->data));
        auto seat = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_librg(vehicle_ent);
        auto sender = oak_entity_player_get_from_librg(sender_ent);

        if (vehicle && sender) {
            if (vehicle->seats[seat] != -1) {

                mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_HIJACK, [&](librg_data *data) {
                    librg_data_went(data, sender_ent->id);
                    librg_data_went(data, vehicle_ent->id);
                    librg_data_wi32(data, seat);
                });

                auto driver_ent = librg_entity_fetch(oak_network_ctx_get(), vehicle->seats[seat]);
                auto driver = oak_entity_player_get_from_librg(driver_ent);

                if (driver) {
                    driver->vehicle_id = -1;
                }

                vehicle->seats[seat] = -1;

                if (seat == 0) {
                    oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
                }
            }
        }
    });

    librg_network_add(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_REMOVE, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(oak_network_ctx_get(), msg->peer);
        auto sender = oak_entity_player_get_from_librg(sender_ent);

        if (!sender) {
            return;
        }

        if (sender->vehicle_id == -1) {
            return;
        }

        auto sender_vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), sender->vehicle_id);
        auto sender_vehicle = oak_entity_vehicle_get_from_librg(sender_vehicle_ent);

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
        auto unk3 = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_librg(vehicle_ent);
        auto sender = oak_entity_player_get_from_librg(sender_ent);

        if (vehicle && sender) {
            // NOTE(DavoSK) When entering car as driver from passanger seat
            // change the seat_id accordingly
            if (unk3 == 1 && seat_id == 1)
                seat_id = 0;

            // NOTE(DavoSK): SeatID can be NULL if player is force exiting
            // We need to get id from server :)
            if (seat_id == 0 && action == 2) {
                for (int i = 0; i < OAK_MAX_SEATS; i++) {
                    if (vehicle->seats[i] == sender_ent->id) {
                        seat_id = i;
                        break;
                    }
                }
            }

            if(action == 1) {
                vehicle->seats[seat_id] = sender_ent->id;
                sender->vehicle_id = vehicle_ent->id;
            } else if (action == 2) {
                vehicle->seats[seat_id] = -1;
                sender->vehicle_id = -1;
            }

            mod_message_send(oak_network_ctx_get(), NETWORK_VEHICLE_PLAYER_USE_DOOR, [&](librg_data *data) {
                librg_data_went(data, sender_ent->id);
                librg_data_went(data, vehicle_ent->id);
                librg_data_wi32(data, action);
                librg_data_wi32(data, seat_original);
                librg_data_wi32(data, unk3);
            });

            if (seat_id == 0 && action == 1) {
                librg_entity_control_set(oak_network_ctx_get(), vehicle_ent->id, sender_ent->client_peer);
            } else if (seat_id == 0 && action == 2) {
                oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
            }
        }
    });

    return 0;
}
