void add_messages() {
    librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message* msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);

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

    librg_network_add(&network_context, NETWORK_PLAYER_USE_DOORS, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);
        auto door_name_len = librg_data_ru32(msg->data);
        char door_name[32];
        librg_data_rptr(msg->data, door_name, door_name_len);
        door_name[door_name_len] = '\0';

        auto door_state = librg_data_ru32(msg->data);
        librg_entity* door_ent = nullptr;
        for (u32 i = 0; i < network_context.max_entities; i++) {
            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (!entity) continue;
            if (entity->type == TYPE_DOOR) {
                auto current_door = (mafia_door*)entity->user_data;
                if(strcmp(current_door->name, door_name) == 0) {
                    door_ent = entity;
                    break;
                }
            }     
        }

        //If doors doesen't exists yet we create new one
        if (door_ent == nullptr) {
            auto door_user_data = new mafia_door();
            memcpy(door_user_data->name, door_name, door_name_len);
            door_user_data->name[door_name_len] = '\0';

            door_ent = librg_entity_create(&network_context, TYPE_DOOR);
            door_ent->user_data = door_user_data;
        }

        librg_entity_control_set(&network_context, door_ent->id, sender_ent->client_peer);

        mod_message_send(&network_context, NETWORK_PLAYER_USE_DOORS, [&](librg_data *data) {
            librg_data_went(data, sender_ent->id);
            librg_data_wu32(data, door_name_len);
            librg_data_wptr(data, door_name, door_name_len);
            librg_data_wu32(data, door_state);
        });
    });

    /* Weapon system */

    librg_network_add(&network_context, NETWORK_PLAYER_SHOOT, [](librg_message* msg) {
        
        zpl_vec3 pos;
        auto entity = librg_entity_find(&network_context, msg->peer);
        librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

        mod_message_send_except(&network_context, NETWORK_PLAYER_SHOOT, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
            librg_data_wptr(data, &pos, sizeof(zpl_vec3));
        });
    });

    librg_network_add(&network_context, NETWORK_PLAYER_THROW_GRENADE, [](librg_message* msg) {
        
        zpl_vec3 pos;
        auto entity = librg_entity_find(&network_context, msg->peer);
        librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

        printf("Debug player throw grenade: %f %f %f\n", pos.x, pos.y, pos.z);
        mod_message_send_except(&network_context, NETWORK_PLAYER_THROW_GRENADE, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
            librg_data_wptr(data, &pos, sizeof(zpl_vec3));
        });
    });

    librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message* msg) {

        auto entity = librg_entity_find(&network_context, msg->peer);
        if (!entity) return;
        u32 index = librg_data_ru32(msg->data);
        
        //set serverside current weapon index for player
        auto player = (mafia_player*)entity->user_data;
        player->current_weapon_id = index;

        mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
            librg_data_wu32(data, index);
        });
    });

    librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, [](librg_message* msg) {

        auto entity = librg_entity_find(&network_context, msg->peer);
        if (!entity) return;
        
        //process inventory here :) TODO !
        mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
        });
    });

    librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, [](librg_message* msg) {

        auto entity = librg_entity_find(&network_context, msg->peer);
        if (!entity) return;
        
        //process inventory here :) TODO !
        mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
        });
    });

    librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_DROP, [](librg_message* msg) {

        auto entity = librg_entity_find(&network_context, msg->peer);
        inventory_item item = {0};
        char model[32] = "";
        
        librg_data_rptr(msg->data, &item, sizeof(inventory_item));
        librg_data_rptr(msg->data, model, sizeof(char) * 32);
        
        spawn_weapon_drop(entity->position, model, item);
        
        inventory_remove(entity, item.weaponId, true, true);
    });

    librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message* msg) {

        auto player_entity		= librg_entity_find(&network_context, msg->peer);
        librg_entity_id id		= librg_data_rent(msg->data);
        auto entity				= librg_entity_fetch(&network_context, id);
        auto weapon_drop		= (mafia_weapon_drop*)entity->user_data;

        inventory_add(player_entity, &weapon_drop->weapon, true, true);

        if (entity) {
            if (entity->user_data) 
                delete entity->user_data;
            librg_entity_destroy(&network_context, id);
        }
    });

}
