/**
 * Send a message to a specific player
 * @param  text
 * @param  id
 * @return
 */
int oak_chat_send(oak_player id, const char *text, int length) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    librg_send_to(&network_context, NETWORK_SEND_CHAT_MSG, entity->native_entity->client_peer, data, {
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
