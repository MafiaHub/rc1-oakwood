/**
 * Make a fadeout so player will see a nice effect
 * @param  id
 * @param  fadeout
 * @param  duration
 * @param  color
 * @return
 */
int oak_hud_fadeout(oak_player id, int fadeout, int duration, int color) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);

    librg_send_to(&network_context, NETWORK_HUD_FADEOUT, player->native_entity->client_peer, data, {
        librg_data_wu8(&data, fadeout);
        librg_data_wu32(&data, duration);
        librg_data_wu32(&data, color);
    });

    return 0;
}

/**
 * Show a countdown for a player
 * @param  id
 * @param  number
 * @return
 */
int oak_hud_countdown(oak_player id, int number) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);

    // wrap the number around
    number = (3 - number % 4);

    librg_send_to(&network_context, NETWORK_HUD_COUNTDOWN, player->native_entity->client_peer, data, {
        librg_data_wu32(&data, number);
    });

    return 0;
}

/**
 * Show a nice alert/splash message in the center of the screen
 * @param  id
 * @param  text
 * @param  duration
 * @return
 */
int oak_hud_announce(oak_player id, const char *text, int length, float duration) {
    if (oak_player_invalid(id)) return -1;
    auto player = oak_entity_player_get(id);

    librg_send_to(&network_context, NETWORK_HUD_ALERT, player->native_entity->client_peer, data, {
        librg_data_wu32(&data, length);
        librg_data_wf32(&data, duration);
        librg_data_wptr(&data, (void *)text, length);
    });

    return 0;
}

/**
 * Send a message to all players with specific color
 * @param  text
 * @param  color
 * @return
 */
int oak_hud_message(oak_player id, const char *text, int length, int color) {
    if (oak_player_invalid(id)) return -1;
    auto player = oak_entity_player_get(id);

    librg_send_to(&network_context, NETWORK_SEND_CONSOLE_MSG, player->native_entity->client_peer, data, {
        librg_data_wu32(&data, length);
        librg_data_wu32(&data, color);
        librg_data_wptr(&data, (void*)text, length);
    });

    return 0;
}
