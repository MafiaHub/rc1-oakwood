librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message_t* msg) {
    char chat_line[128] = "";

    auto chat_len = librg_data_ru8(msg->data);
    librg_data_rptr(msg->data, chat_line, chat_len);
    chat::chat_messages.push_back(std::make_pair(ImVec4(1.0 / 250.0 * 150, 1.0f, 1.0f, 1.0f), chat_line));
});

librg_network_add(&network_context, NETWORK_SEND_FADEOUT, [](librg_message_t* msg) {

	auto do_fade = librg_data_ru8(msg->data);
    auto duration = librg_data_ru32(msg->data);
    auto color = librg_data_ru32(msg->data);
	MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(do_fade, duration, color);
});