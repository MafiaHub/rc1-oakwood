/**
 * Send a message to a specific player
 * @param  text
 * @param  id
 * @return
 */
int oak_chat_send(oak_player id, const char *text, int length) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    librg_send_to(&network_context, NETWORK_SEND_CHAT_MSG, entity->librg_entity->client_peer, data, {
        librg_data_wu16(&data, length);
        librg_data_wptr(&data, (void *)text, length);
    });

    return 0;
}

/**
 * Sned a message to all players
 * @param  text
 * @return
 */
int oak_chat_broadcast(const char *text, int length) {
    librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
        librg_data_wu16(&data, length);
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
int oak_chat_broadcast_color(const char *text, int length, int color) {
    librg_send(&network_context, NETWORK_SEND_CONSOLE_MSG, data, {
        librg_data_wu32(&data, length);
        librg_data_wu32(&data, color);
        librg_data_wptr(&data, (void*)text, length);
    });

    return 0;
}
