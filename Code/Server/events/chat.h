int oak_chat_register() {
    librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message* msg) {
        char text[128] = { 0 };

        auto entity = librg_entity_find(&network_context, msg->peer);
        auto pid = (oak_player)entity->user_data;
        auto player = oak_entity_player_get(pid);

        auto text_len = librg_data_ru16(msg->data);
        text_len = zpl_min(text_len, 128);
        librg_data_rptr(msg->data, text, text_len);

        auto is_handled = false;

        oak_bridge_event_player_chat(pid, text);
    });

    return 0;
}
