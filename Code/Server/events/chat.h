int oak_chat_register() {
    librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message* msg) {
        char text[OAK_MAX_CHAT] = { 0 };

        auto entity = librg_entity_find(&network_context, msg->peer);
        auto pid = (oak_player)entity->user_data;
        auto player = oak_entity_player_get(pid);

        auto text_len = librg_data_ru16(msg->data);
        text_len = zpl_min(text_len, OAK_MAX_CHAT);
        librg_data_rptr(msg->data, text, text_len);

        if (GlobalConfig.api_type == "internal")
            oak_angel_event_player_chat(pid, text);
        else
            oak_bridge_event_player_chat(pid, text);
    });

    return 0;
}
