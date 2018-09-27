librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message_t* msg) {
    char username[32] = "";
    char text[128] = "";

    auto text_len = librg_data_ru8(msg->data);
    librg_data_rptr(msg->data, text, text_len);

    auto username_len = librg_data_ru8(msg->data);
    librg_data_rptr(msg->data, username, username_len);

    auto chat_line = "<" + std::string(username) + "> " + std::string(text);

    librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
        librg_data_wu8(&data, chat_line.size());
        librg_data_wptr(&data, (void *)chat_line.c_str(), chat_line.size());
    });

    printf("[CHAT] %s\n", chat_line.c_str());
});