typedef struct {
    char name[OAK_PLAYER_NAME_SIZE];
    u64 hwid;
} oak_temp_userdata;

/* PLAYER EVENTS */

void oak_ev_player_requested(librg_event *evnt) {
    auto build_magic = librg_data_ru64(evnt->data);
    auto build_ver = librg_data_ru64(evnt->data);
    auto peer_ip = evnt->peer->address;

    oak_temp_userdata temp = {0};
    char hostname[128] = { 0 };
    enet_address_get_host_ip(&peer_ip, hostname, 128);

    if (build_magic != OAK_BUILD_MAGIC || build_ver != OAK_BUILD_VERSION) {
        oak_log("Connection for '%s' has been rejected!\nOur magic: %X\tTheir magic: %X\nOur version: %X\tTheir version: %X\n", hostname, OAK_BUILD_MAGIC, build_magic, OAK_BUILD_VERSION, build_ver);
        librg_event_reject(evnt);

        librg_send_to(&network_context, NETWORK_SEND_REJECTION, evnt->peer, data, {
            librg_data_wu32(&data, REJECTION_VERSION);
        });
        return;
    }

    auto hwid = librg_data_ru64(evnt->data);

    librg_data_rptr(evnt->data, temp.name, sizeof(char) * OAK_PLAYER_NAME_SIZE);

    // TODO: Apart from local ban database, fetch global bans as well
    {
        b32 isBanned = oak_access_bans_get(hwid);

        if (isBanned) {
            oak_log("Connection for %s'%s' has been rejected!\nPlayer is banned! GUID: %llu\n", temp.name, hostname, hwid);
            librg_event_reject(evnt);

            librg_send_to(&network_context, NETWORK_SEND_REJECTION, evnt->peer, data, {
                librg_data_wu32(&data, REJECTION_BANNED);
            });
            return;
        }
    }

    if (oak_access_wh_state_get()) {
        b32 isExempted = oak_access_wh_get(hwid);

        if (!isExempted) {
            oak_log("Connection for %s o'%s' has been rejected!\nPlayer is not whitelisted! GUID: %llu\n", temp.name, hostname, hwid);
            librg_event_reject(evnt);

            librg_send_to(&network_context, NETWORK_SEND_REJECTION, evnt->peer, data, {
                librg_data_wu32(&data, REJECTION_WH);
            });
            return;
        }
    }

    /* NOTE: dont forget to free the temp data ptr */
    temp.hwid = hwid;
    evnt->user_data = zpl_malloc(sizeof(oak_temp_userdata));
    zpl_memcopy(evnt->user_data, &temp, sizeof(temp));
}

void oak_ev_player_connected(librg_event *e) {
    /* change default timeout */
    enet_peer_timeout(e->peer, 10, 5000, 10000);

    /* create our player */
    auto id = oak_player_create(e);
    auto player = oak_entity_player_get(id);
    auto temp = (oak_temp_userdata *)e->user_data;

    zpl_memcopy(player->name, temp->name, OAK_PLAYER_NAME_SIZE);
    player->hwid = temp->hwid;
    zpl_mfree(temp);

    oak_log("[info] player '%s'(%llu) has been connected!\n", player->name, player->hwid);

    // oak_player_position_set(id, {61.4763f, 4.72524f, 107.708f});
    // oak_player_spawn(id);

    oak_bridge_event_player_connect(id);
}

void oak_ev_player_disconnected(librg_event *e) {
    auto player = oak_entity_player_get_from_librg(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    /* remove player from vehicle if any */

    // oak_vehicle_player_remove(oak_player_vehicle_inside(player->oak_id), player->oak_id);
    oak_log("[info] player '%s'(%d) %x has been disconnected!\n", player->name, player->oak_id, e->peer);

    oak_bridge_event_player_disconnect(player->oak_id);
    oak_player_destroy(e);
}

/* GENERAL ENTITY EVENTS */

void oak_ev_player_create(librg_event *e) {
    auto player = oak_entity_player_get_from_librg(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    librg_data_wi32(e->data, player->vehicle_id);
    librg_data_wi32(e->data, player->streamer_entity_id);
    librg_data_wptr(e->data, &player->position, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wptr(e->data, player->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
    librg_data_wptr(e->data, player->name, sizeof(char) * OAK_PLAYER_NAME_SIZE);
    librg_data_wu8(e->data, player->is_crouching);
    librg_data_wu8(e->data, player->is_aiming);
    librg_data_wptr(e->data, &player->inventory, sizeof(player_inventory));
    librg_data_wu32(e->data, player->current_weapon_id);
    librg_data_wf32(e->data, player->health);
    librg_data_wu8(e->data, player->is_visible_on_map);
    librg_data_wu8(e->data, player->has_visible_nameplate);
}

void oak_ev_player_remove(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_player_update(librg_event *e) {
    auto player = oak_entity_player_get_from_librg(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    librg_data_wptr(e->data, &player->position, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wf32(e->data, player->health);
    librg_data_wu8(e->data, player->animation_state);
    librg_data_wu8(e->data, player->is_crouching);
    librg_data_wu8(e->data, player->is_aiming);
    librg_data_wu32(e->data, player->aiming_time);
    librg_data_wf32(e->data, player->aim);
    librg_data_wu32(e->data, e->peer->lastRoundTripTime);
}

void oak_ev_player_client_add(librg_event *e) {
    // NOTE: not implmented
}

void oak_ev_player_client_update(librg_event *e) {
    auto player = oak_entity_player_get_from_librg(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    /*if (!player) {
        usize size = librg_data_get_rpos(e->data);

        size += sizeof(zpl_vec3);
        size += sizeof(zpl_vec3);
        size += sizeof(zpl_vec3);
        size += sizeof(f32);
        size += sizeof(u8);
        size += sizeof(u8);
        size += sizeof(u8);
        size += sizeof(f32);
        size += sizeof(u64);

        librg_data_set_rpos(e->data, size);
        librg_event_reject(e);
        return;
    }*/

    librg_data_rptr(e->data, &player->position, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &player->pose, sizeof(zpl_vec3));
    player->health = librg_data_rf32(e->data);
    player->animation_state = librg_data_ru8(e->data);
    player->is_crouching = librg_data_ru8(e->data);
    player->is_aiming = librg_data_ru8(e->data);
    player->aim = librg_data_rf32(e->data);
    player->aiming_time = librg_data_ru64(e->data);
}

void oak_ev_player_client_remove(librg_event *e) {
    // NOTE: not implmented
}

#if 0
    inline auto drop_entitycreate(librg_event* evnt) -> void {
        auto drop = (mafia_weapon_drop *)evnt->entity->user_data;
        librg_data_wptr(evnt->data, drop->model, sizeof(char) * 32);
        librg_data_wptr(evnt->data, &drop->weapon, sizeof(inventory_item));
    }


    inline auto connection_disconnect(librg_event* evnt) -> void {
        if (evnt->entity && evnt->entity->user_data) {
            auto player = (mafia_player*)evnt->entity->user_data;
            if (player->vehicle_id != -1) {
                auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
                if (vehicle_ent && vehicle_ent->user_data) {
                    auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                    for (int i = 0; i < 4; i++) {
                        if (vehicle->seats[i] == evnt->entity->id) {
                            vehicle->seats[i] = -1;
                            player->vehicle_id = -1;

                            if (i == 0) {
                                mod_vehicle_assign_nearest_player(&network_context, vehicle_ent, evnt->entity->id);
                                librg_send_all(&network_context, NETWORK_VEHICLE_PLAYER_DISCONNECT, data, {
                                    librg_data_went(&data, vehicle_ent->id);
                                });
                            }
                        }
                    }
                }
            }
        }
    }
#endif

/* PLAYER MESSAGES */

int oak_player_register() {
    librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message *msg) {
        oak_player pid = (oak_player)librg_entity_find(&network_context, msg->peer)->user_data;
        oak_player_kill(pid);
        oak_bridge_event_player_death(pid);
    });

    /*librg_network_add(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);

        if (sender_ent->user_data && sender_ent->type == TYPE_PLAYER) {
            auto player = (mafia_player *)sender_ent->user_data;
            librg_data_rptr(msg->data, &player->inventory, sizeof(player_inventory));
            inventory_send(sender_ent);
        }
    });*/

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

        oak_player pid = (oak_player)sender_ent->user_data;
        oak_player aid = (oak_player)librg_entity_fetch(msg->ctx, attacker_id)->user_data;

        float current_health = oak_player_health_get(pid);

        oak_player_health_set(pid, health);
        oak_bridge_event_player_hit(pid, aid, current_health - health);

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

    librg_network_add(&network_context, NETWORK_PLAYER_KEY_PRESS, [](librg_message* msg) {
        auto player_entity = librg_entity_find(&network_context, msg->peer);
        auto is_pressed = librg_data_ru8(msg->data);
        auto key = librg_data_ru32(msg->data);

        oak_player pid = (oak_player)player_entity->user_data;
        oak_bridge_event_player_key(pid, key);
    });

    return 0;
}
