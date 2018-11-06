librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message* msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    mod_message_send_except(&network_context, NETWORK_PLAYER_DIE, msg->peer, [&](librg_data *data) {
        librg_data_went(data, sender_ent->id);
    });

    if (sender_ent->user_data) {
        auto player = (mafia_player*)sender_ent->user_data;
        player->health = 0.0f;

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

librg_network_add(&network_context, NETWORK_PLAYER_USE_ACTOR, [](librg_message *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto action = librg_data_ri32(msg->data);
    auto seat_id = librg_data_ri32(msg->data);

   if (sender_ent->user_data && vehicle_ent) {

        auto vehicle = (mafia_vehicle *)vehicle_ent->user_data;

        if(action == 1) {
            vehicle->seats[seat_id] = sender_ent->id;
        } else if (action == 2) {
			vehicle->seats[seat_id] = -1;
        }

        mod_message_send_except(&network_context, NETWORK_PLAYER_USE_ACTOR, msg->peer, [&](librg_data *data) {
            librg_data_went(data, sender_ent->id);
            librg_data_went(data, vehicle_ent->id);
            librg_data_wi32(data, action);
            librg_data_wi32(data, seat_id);
        });

        if (seat_id == 0 && action == 1) {
			librg_entity_control_set(&network_context, vehicle_ent->id, sender_ent->client_peer);
		} else if (seat_id == 0 && action == 2) {
            librg_entity_control_remove(&network_context, vehicle_ent->id);
        }
    }
});