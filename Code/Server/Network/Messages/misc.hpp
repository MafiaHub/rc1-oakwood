librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message_t* msg) {
    char text[128] = "";

    auto entity = librg_entity_find(&network_context, msg->peer);
    auto player = (mafia_player *)entity->user_data;

    auto text_len = librg_data_ru16(msg->data);
    librg_data_rptr(msg->data, text, text_len);

    auto is_handled = false;

    if (gm.on_player_chat) {
        is_handled = gm.on_player_chat(entity, std::string(text));
    }

    if (!is_handled) {
        auto chat_line = "<" + std::string(player->name) + "> " + std::string(text);

        librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
            librg_data_wu16(&data, chat_line.size());
            librg_data_wptr(&data, (void *)chat_line.c_str(), chat_line.size());
        });

        printf("[CHAT] %s\n", chat_line.c_str());
    }
});