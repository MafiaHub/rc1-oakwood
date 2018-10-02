librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message_t* msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    mod_message_send_except(&network_context, NETWORK_PLAYER_DIE, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, sender_ent->id);
    });

    if (sender_ent->user_data) {
        auto player = (mafia_player*)sender_ent->user_data;

        //change health
        player->health = 0.0f;

        if (gm.on_player_died)
            gm.on_player_died(sender_ent, player);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, [](librg_message_t *msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    if (sender_ent->user_data && sender_ent->type == TYPE_PLAYER) {
        auto player = (mafia_player *)sender_ent->user_data;

        librg_data_rptr(msg->data, &player->inventory, sizeof(player_inventory));

        player_inventory_send(sender_ent);
    }
});