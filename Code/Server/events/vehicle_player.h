int oak_vehicle_player_register() {
    librg_network_add(&network_context, NETWORK_VEHICLE_PLAYER_HIJACK, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);
        auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_rent(msg->data));
        auto seat = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_librg(vehicle_ent);
        auto sender = oak_entity_player_get_from_librg(sender_ent);

        if (vehicle && sender) {
            if (vehicle->seats[seat] != -1) {

                mod_message_send(&network_context, NETWORK_VEHICLE_PLAYER_HIJACK, [&](librg_data *data) {
                    librg_data_went(data, sender_ent->id);
                    librg_data_went(data, vehicle_ent->id);
                    librg_data_wi32(data, seat);
                });

                auto driver_ent = librg_entity_fetch(&network_context, vehicle->seats[seat]);
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

    librg_network_add(&network_context, NETWORK_VEHICLE_PLAYER_REMOVE, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);

        if (sender_ent) {
            auto sender = oak_entity_player_get_from_librg(sender_ent);

            if (sender->vehicle_id != -1) {
                auto sender_vehicle_ent = librg_entity_fetch(&network_context, sender->vehicle_id);
                if (sender_vehicle_ent) {
                    auto sender_vehicle = oak_entity_vehicle_get((oak_vehicle)sender_vehicle_ent->user_data);

                    for (u32 i = 0; i < 4; i++) {
                        if (sender_vehicle->seats[i] == sender_ent->id) {
                            sender_vehicle->seats[i] = -1;
                            sender->vehicle_id = -1;

                            mod_message_send(&network_context, NETWORK_VEHICLE_PLAYER_REMOVE, [&](librg_data *data) {
                                librg_data_went(data, sender_ent->id);
                                librg_data_went(data, sender_vehicle_ent->id);
                                librg_data_wu32(data, i);
                            });

                            // NOTE(DavoSK) : He was driver find new streamer,
                            // Do we need this part code if player will not actualy change position until message is send

                            if (i == 0) {
                                oak_vehicle_streamer_assign_nearest(sender_vehicle->oak_id);
                            }

                            break;
                        }
                    }
                }
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_PLAYER_USE_DOOR, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);
        auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
        auto action = librg_data_ri32(msg->data);
        auto seat_id = librg_data_ri32(msg->data);
        i32 seat_original = seat_id;
        auto unk3 = librg_data_ri32(msg->data);

        auto vehicle = oak_entity_vehicle_get_from_librg(vehicle_ent);
        auto sender = oak_entity_player_get_from_librg(sender_ent);

        if (sender_ent && vehicle_ent && vehicle && sender) {


            // NOTE(DavoSK) When entering car as driver from passanger seat
            // change the seat_id accordingly
            if (unk3 == 1 && seat_id == 1)
                seat_id = 0;

            // NOTE(DavoSK): SeatID can be NULL if player is force exiting
            // We need to get id from server :)
            if (seat_id == 0 && action == 2) {
                for (int i = 0; i < 4; i++) {
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

            mod_message_send(&network_context, NETWORK_VEHICLE_PLAYER_USE_DOOR, [&](librg_data *data) {
                librg_data_went(data, sender_ent->id);
                librg_data_went(data, vehicle_ent->id);
                librg_data_wi32(data, action);
                librg_data_wi32(data, seat_original);
                librg_data_wi32(data, unk3);
            });

            if (seat_id == 0 && action == 1) {
                librg_entity_control_set(&network_context, vehicle_ent->id, sender_ent->client_peer);
            } else if (seat_id == 0 && action == 2) {
                oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
            }
        }
    });

    return 0;
}
