librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message* msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    mod_message_send_except(&network_context, NETWORK_PLAYER_DIE, msg->peer, [&](librg_data *data) {
        librg_data_went(data, sender_ent->id);
    });

    if (sender_ent->user_data) {
        auto player = (mafia_player*)sender_ent->user_data;
        player->health = 0.0f;

        if (player->vehicle_id != -1) {
            auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
            if (vehicle_ent && vehicle_ent->user_data) {
                auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                for (int i = 0; i < 4; i++) {
                    if (vehicle->seats[i] == sender_ent->id) {
                        vehicle->seats[i] = -1;
                        player->vehicle_id = -1;
                        break;
                    }
                }
            }
        }

        if (gm.on_player_died)
            gm.on_player_died(sender_ent, player);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, [](librg_message *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    if (sender_ent->user_data && sender_ent->type == TYPE_PLAYER) {
        auto player = (mafia_player *)sender_ent->user_data;
        librg_data_rptr(msg->data, &player->inventory, sizeof(player_inventory));
        player_inventory_send(sender_ent);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_HIT, [](librg_message* msg) {

    auto sender_ent = librg_entity_find(&network_context, msg->peer);
    librg_entity_id attacker_id = librg_data_rent(msg->data);
    u32 hit_type = librg_data_ru32(msg->data);
    zpl_vec3 unk1, unk2, unk3;
    librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));
    f32 damage = librg_data_rf32(msg->data);
    f32 health = librg_data_rf32(msg->data);
    u32 player_part = librg_data_ru32(msg->data);

    auto player = (mafia_player*)sender_ent->user_data;

    float current_health = player->health;

    if (player) {
        player->health = health;
    }

    if (gm.on_player_hit)
        gm.on_player_hit(librg_entity_fetch(&network_context, attacker_id), sender_ent, current_health - health);

    mod_message_send_except(&network_context, NETWORK_PLAYER_HIT, msg->peer, [&](librg_data *data) {
        librg_data_went(data, sender_ent->id);
        librg_data_went(data, attacker_id);
        librg_data_wu32(data, hit_type);
        librg_data_wptr(data, (void*)&unk1, sizeof(zpl_vec3));
        librg_data_wptr(data, (void*)&unk2, sizeof(zpl_vec3));
        librg_data_wptr(data, (void*)&unk3, sizeof(zpl_vec3));
        librg_data_wf32(data, damage);
        librg_data_wf32(data, health);
        librg_data_wu32(data, player_part);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_HIJACK, [](librg_message *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto seat = librg_data_ri32(msg->data);

    if (sender_ent->user_data && vehicle_ent) {
        auto vehicle = (mafia_vehicle *)vehicle_ent->user_data;
        auto sender = (mafia_player*)sender_ent->user_data;

        // remove driver from vehicle !
        if (vehicle->seats[seat] != -1) {
            
            mod_message_send(&network_context, NETWORK_PLAYER_HIJACK, [&](librg_data *data) {
                librg_data_went(data, sender_ent->id);
                librg_data_went(data, vehicle_ent->id);
                librg_data_wi32(data, seat);
            });

            auto driver_ent = librg_entity_fetch(&network_context, vehicle->seats[seat]);
            if (driver_ent && driver_ent->user_data) {
                auto driver = (mafia_player*)driver_ent->user_data;
                driver->vehicle_id = -1;
            }

            vehicle->seats[seat] = -1;

            if (seat == 0) {
                auto streamer = mod_get_nearest_player(&network_context, vehicle_ent->position);
                if (streamer != nullptr) {
                    librg_entity_control_set(&network_context, vehicle_ent->id, streamer->client_peer);
                }
            }
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_FROM_CAR, [](librg_message *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    if (sender_ent && sender_ent->user_data) {
        auto sender = (mafia_player*)sender_ent->user_data;
        
        if (sender->vehicle_id != -1) {
            auto sender_vehicle_ent = librg_entity_fetch(&network_context, sender->vehicle_id);
            if (sender_vehicle_ent && sender_vehicle_ent->user_data) {
                auto sender_vehicle = (mafia_vehicle*)sender_vehicle_ent->user_data;

                for (u32 i = 0; i < 4; i++) {
                    if (sender_vehicle->seats[i] == sender_ent->id) {
                        sender_vehicle->seats[i] = -1;
                        sender->vehicle_id = -1;

                        mod_message_send(&network_context, NETWORK_PLAYER_FROM_CAR, [&](librg_data *data) {
                            librg_data_went(data, sender_ent->id);
                            librg_data_went(data, sender_vehicle_ent->id);
                            librg_data_wu32(data, i);
                        });

                        // NOTE(DavoSK) : He was driver find new streamer, 
                        // Do we need this part code if player will not actualy change position until message is send

                        if (i == 0) {
                            auto streamer = mod_get_nearest_player(&network_context, sender_vehicle_ent->position);
                            if (streamer != nullptr) {
                                librg_entity_control_set(&network_context, sender_vehicle_ent->id, streamer->client_peer);
                            }
                            else {
                                librg_entity_control_remove(&network_context, sender_vehicle_ent->id);
                            }
                        }

                        break;
                    }
                }
            }
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_USE_ACTOR, [](librg_message *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto action = librg_data_ri32(msg->data);
    auto seat_id = librg_data_ri32(msg->data);
    auto unk3 = librg_data_ri32(msg->data);

   if (sender_ent->user_data && vehicle_ent) {

        auto vehicle = (mafia_vehicle *)vehicle_ent->user_data;
        auto sender = (mafia_player*)sender_ent->user_data;

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

        mod_message_send(&network_context, NETWORK_PLAYER_USE_ACTOR, [&](librg_data *data) {
            librg_data_went(data, sender_ent->id);
            librg_data_went(data, vehicle_ent->id);
            librg_data_wi32(data, action);
            librg_data_wi32(data, seat_id);
            librg_data_wi32(data, unk3);
        });

        if (seat_id == 0 && action == 1) {
            librg_entity_control_set(&network_context, vehicle_ent->id, sender_ent->client_peer);
        } else if (seat_id == 0 && action == 2) {
            auto streamer = mod_get_nearest_player(&network_context, vehicle_ent->position);
            if (streamer != nullptr) {
                librg_entity_control_set(&network_context, vehicle_ent->id, streamer->client_peer);
            }
            else {
                librg_entity_control_remove(&network_context, vehicle_ent->id);
            }
        }
    }
});
