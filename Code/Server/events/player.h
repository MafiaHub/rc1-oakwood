typedef struct {
    char name[OAK_PLAYER_NAME_SIZE];
    u64 hwid;
} oak_temp_userdata;

/* PLAYER EVENTS */

void oak_ev_player_send_rejection(u32 type, librg_event *evnt) {
    librg_event_reject(evnt);

    librg_send_to(&network_context, NETWORK_SEND_REJECTION, evnt->peer, data, {
        librg_data_wu32(&data, type);
    });
}

void oak_ev_player_requested(librg_event *evnt) {
    auto peer_ip = evnt->peer->address;

    oak_temp_userdata temp = {0};
    char hostname[128] = { 0 };
    enet_address_get_host_ip(&peer_ip, hostname, 128);

    auto build_major = librg_data_ru8(evnt->data);
    auto build_minor = librg_data_ru8(evnt->data);
    auto build_patch = librg_data_ru8(evnt->data);
    auto build_channel = librg_data_ru8(evnt->data);

    if (build_major != OAK_VERSION_MAJOR || build_minor != OAK_VERSION_MINOR || build_channel != OAK_BUILD_CHANNEL) {
        oak_log("Connection for '%s' has been rejected!\nOur version: %s\tTheir version: %d.%d.%d\n", hostname, OAK_VERSION, build_major, build_minor, build_patch);
        oak_ev_player_send_rejection(REJECTION_VERSION, evnt);
        return;
    }

    auto hwid = librg_data_ru64(evnt->data);

    librg_data_rptr(evnt->data, temp.name, sizeof(char) * OAK_PLAYER_NAME_SIZE);

    if (GlobalConfig.password.size() != 0) {
        if (evnt->data->capacity == evnt->data->read_pos) {
            oak_log("Connection for '%s' has been rejected!\nIncorrect password!\n", hostname);
            oak_ev_player_send_rejection(REJECTION_PASSWORD, evnt);
            return;
        }

        char prompt_pass[32] = "";
        librg_data_rptr(evnt->data, prompt_pass, sizeof(char) * 32);

        if (std::string(prompt_pass) != GlobalConfig.password) {
            oak_log("Connection for '%s' has been rejected!\nIncorrect password!\n", hostname);
            oak_ev_player_send_rejection(REJECTION_PASSWORD, evnt);
            return;
        }
    }

    // TODO: Apart from local ban database, fetch global bans as well
    {
        b32 isBanned = oak_access_bans_get(hwid);

        if (isBanned) {
            oak_log("Connection for %s'%s' has been rejected!\nPlayer is banned! GUID: %llu\n", temp.name, hostname, hwid);
            oak_ev_player_send_rejection(REJECTION_BANNED, evnt);
            return;
        }
    }

    if (oak_access_wh_state_get()) {
        b32 isExempted = oak_access_wh_get(hwid);

        if (!isExempted) {
            oak_log("Connection for %s o'%s' has been rejected!\nPlayer is not whitelisted! GUID: %llu\n", temp.name, hostname, hwid);
            oak_ev_player_send_rejection(REJECTION_WH, evnt);
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
    char ip[24] = {0};
    enet_address_get_host_ip(&e->peer->address, ip, 24);
    oak_log("[info] player '%s'(%llu|%s) has been connected!\n", player->name, player->hwid, ip);

    oak_bridge_event_player_connect(id);
    GlobalConfig.players++;
}

void oak_ev_player_disconnected(librg_event *e) {
    auto player = oak_entity_player_get_from_native(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    /* remove player from vehicle if any */

    // oak_vehicle_player_remove(oak_player_vehicle_inside(player->oak_id), player->oak_id);
    oak_log("[info] player '%s'(%d) %x has been disconnected!\n", player->name, player->oak_id, e->peer);

    oak_bridge_event_player_disconnect(player->oak_id);
    oak_player_destroy(e);
    GlobalConfig.players--;
}

/* GENERAL ENTITY EVENTS */

void oak_ev_player_create(librg_event *e) {
    auto player = oak_entity_player_get_from_native(e->entity);
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
    auto player = oak_entity_player_get_from_native(e->entity);
    ZPL_ASSERT_NOT_NULL(player);

    librg_data_wptr(e->data, &player->position, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wf32(e->data, player->health);
    librg_data_wu8(e->data, player->animation_state);
    librg_data_wu8(e->data, player->is_shooting);
    librg_data_wptr(e->data, &player->aim_vector, sizeof(zpl_vec3));
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
    auto player = oak_entity_player_get_from_native(e->entity);

    /* handle cases when entity is already deleted on the server */
    /* but we are still receiving updates from the clients */
    if (!player) {
        librg_event_reject(e);
        return;
    }

    librg_data_rptr(e->data, &player->position, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &player->pose, sizeof(zpl_vec3));
    player->health = librg_data_rf32(e->data);
    player->animation_state = librg_data_ru8(e->data);
    player->is_shooting = librg_data_ru8(e->data);
    librg_data_rptr(e->data, &player->aim_vector, sizeof(zpl_vec3));

    player->is_crouching = librg_data_ru8(e->data);
    player->is_aiming = librg_data_ru8(e->data);
    player->aim = librg_data_rf32(e->data);
    player->aiming_time = librg_data_ru64(e->data);
}

void oak_ev_player_client_remove(librg_event *e) {
    // NOTE: not implmented
}

/* PLAYER MESSAGES */
int oak_player_register() {

    librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message *msg) {
        oak_player pid = (oak_player)librg_entity_find(&network_context, msg->peer)->user_data;
        if (oak_player_invalid(pid)) {
            return;
        }

        auto player = oak_entity_player_get(pid);

        if (librg_entity_valid(oak_network_ctx_get(), player->vehicle_id)) {
            auto vid = oak_entity_vehicle_get_from_native(
                librg_entity_fetch(oak_network_ctx_get(), player->vehicle_id))->oak_id;

            oak_vehicle_player_remove(vid, player->oak_id);
        }

        oak_bridge_event_player_death(pid);
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

        oak_player pid = (oak_player)sender_ent->user_data;
        oak_player aid = (oak_player)librg_entity_fetch(msg->ctx, attacker_id)->user_data;

        float current_health = oak_player_health_get(pid);

        oak_player_health_set(pid, health);
        oak_bridge_event_player_hit(pid, aid, current_health - health);

        /*if (current_health <= 0.0) {
            mod_message_send_except(&network_context, NETWORK_PLAYER_HIT, msg->peer, [&](librg_data* data) {
                librg_data_went(data, sender_ent->id);
                librg_data_went(data, attacker_id);
                librg_data_wu32(data, hit_type);
                librg_data_wptr(data, (void*)& unk1, sizeof(zpl_vec3));
                librg_data_wptr(data, (void*)& unk2, sizeof(zpl_vec3));
                librg_data_wptr(data, (void*)& unk3, sizeof(zpl_vec3));
                librg_data_wf32(data, damage);
                librg_data_wf32(data, health);
                librg_data_wu32(data, player_part);
            });
        }*/
    });

    librg_network_add(&network_context, NETWORK_PLAYER_KEY_PRESS, [](librg_message* msg) {
        auto player_entity = librg_entity_find(&network_context, msg->peer);
        auto is_pressed = librg_data_ru8(msg->data);
        auto key = librg_data_ru32(msg->data);

        oak_player pid = (oak_player)player_entity->user_data;
        oak_bridge_event_player_key(pid, key, is_pressed);
    });

    return 0;
}
