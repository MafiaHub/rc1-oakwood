librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message_t* msg) {
    char chat_line[128] = "";

    auto chat_len = librg_data_ru8(msg->data);
    librg_data_rptr(msg->data, chat_line, chat_len);
    chat::chat_messages.push_back(chat_line);
});