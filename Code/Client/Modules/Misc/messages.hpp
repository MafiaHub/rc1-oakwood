void add_messages()
{
    librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message *msg) {
        auto chat_len = librg_data_ru16(msg->data);
        zpl_string chat_line = zpl_string_make_reserve(zpl_heap(), chat_len);
        librg_data_rptr(msg->data, chat_line, chat_len);
        chat::add_message(chat_line);
        zpl_string_free(chat_line);
    });

    librg_network_add(&network_context, NETWORK_CLEAR_CHAT, [](librg_message* msg) {
        chat::clear_messages();
    });

    librg_network_add(&network_context, NETWORK_DIALOG_OPEN, [](librg_message* msg) {
        dialog_data dialog;
        librg_data_rptr(msg->data, &dialog, sizeof(dialog_data));
    });

    librg_network_add(&network_context, NETWORK_HUD_FADEOUT, [](librg_message *msg) {
        auto do_fade = librg_data_ru8(msg->data);
        auto duration = librg_data_ru32(msg->data);
        auto color = librg_data_ru32(msg->data);
        MafiaSDK::GetIndicators()->FadeInOutScreen(do_fade, duration, color);
    });

    librg_network_add(&network_context, NETWORK_SEND_CONSOLE_MSG, [](librg_message *msg) {
        zpl_local_persist char msg_buf[256] = {0};
        zpl_memset(msg_buf, 0, 256);
        u32 msg_size = librg_data_ru32(msg->data);
        u32 msg_color = librg_data_ru32(msg->data);
        librg_data_rptr(msg->data, msg_buf, msg_size < 256 ? msg_size : 256);
        MafiaSDK::GetIndicators()->ConsoleAddText(reinterpret_cast<const char *>(msg_buf), msg_color);
    });

    librg_network_add(&network_context, NETWORK_SEND_REJECTION, [](librg_message *msg) {
        u32 reasonID = librg_data_ru32(msg->data);

        switch (reasonID)
        {
        case REJECTION_VERSION:
            modules::infobox::displayError("The client's version is incompatible with the server!");
            break;

        case REJECTION_PASSWORD:
            modules::infobox::displayError("Server password is incorrect!");
            break;

        case REJECTION_WH:
            modules::infobox::displayError("You are not whitelisted on this server!");
            break;

        case REJECTION_BANNED:
            modules::infobox::displayError("You are banned on this server!");
            break;
        }

        car_delte_queue.clear();
        librg_network_stop(msg->ctx);

        #ifdef OAK_FEATURE_VEHICLE_CACHE
        car_cache.clear();
        #endif
    });
}
