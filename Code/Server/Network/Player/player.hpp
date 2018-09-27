librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message_t* msg) {
    auto sender_ent = librg_entity_find(&network_context, msg->peer);

    mod_message_send_except(&network_context, NETWORK_PLAYER_DIE, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, sender_ent->id);
    });

    //remove old player entity and make new :) 
    if (sender_ent->user_data) {
        auto player = (mafia_player*)sender_ent->user_data;

        //change health
        player->health = 0.0f;

        //clear player inventory
        for (size_t i = 0; i < 8; i++)
            player->inventory.items[i] = { -1, 0, 0, 0 };
    }

    mode_player_died(sender_ent);
});