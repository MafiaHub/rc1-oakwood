librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message* msg) {
    auto chat_len = librg_data_ru16(msg->data);
    zpl_string chat_line = zpl_string_make_reserve(zpl_heap(), chat_len);
    librg_data_rptr(msg->data, chat_line, chat_len);
    cefgui::add_message(chat_line);
    zpl_string_free(chat_line);
});

librg_network_add(&network_context, NETWORK_SEND_FADEOUT, [](librg_message* msg) {
    auto do_fade = librg_data_ru8(msg->data);
    auto duration = librg_data_ru32(msg->data);
    auto color = librg_data_ru32(msg->data);
    MafiaSDK::GetIndicators()->FadeInOutScreen(do_fade, duration, color);
});
