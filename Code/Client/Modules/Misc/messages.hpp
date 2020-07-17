void add_messages()
{
    librg_network_add(&network_context, NETWORK_CREATE_EXPL, [](librg_message* msg) {
        auto ent = librg_entity_fetch(&network_context, librg_data_rent(msg->data));
        zpl_vec3 pos = { 0 };
        float radius = librg_data_rf32(msg->data);
        float force = librg_data_rf32(msg->data);
        librg_data_rptr(msg->data, &pos, sizeof(pos));

        auto data = (mafia_player*)ent->user_data;

        S_vector vec = EXPAND_VEC(pos);

        MafiaSDK::C_Actor *act = MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::GhostObject);

        MafiaSDK::GetMission()->GetGame()->NewExplosion(act, vec, radius, force, TRUE, TRUE, FALSE, 2);

        MafiaSDK::GetMission()->DelActor(act);
    });

    librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message *msg) {
        auto chat_len = librg_data_ru16(msg->data);
        zpl_string chat_line = zpl_string_make_reserve(zpl_heap(), chat_len);
        librg_data_rptr(msg->data, chat_line, chat_len);
        chat::add_message(chat_line);
        printf("%s\n", chat::chat_messages[chat::chat_messages.size() - 1].second.c_str());
        zpl_string_free(chat_line);
    });

    librg_network_add(&network_context, NETWORK_CLEAR_CHAT, [](librg_message* msg) {
        chat::clear_messages();
    });

    librg_network_add(&network_context, NETWORK_HUD_FADEOUT, [](librg_message *msg) {
        auto do_fade = librg_data_ru8(msg->data);
        auto duration = librg_data_ru32(msg->data);
        auto color = librg_data_ru32(msg->data);
        MafiaSDK::GetIndicators()->FadeInOutScreen(do_fade, duration, color);
    });

    librg_network_add(&network_context, NETWORK_SEND_CONSOLE_MSG, [](librg_message *msg) {
        u16 msg_size = librg_data_ru16(msg->data);
        u32 msg_color = librg_data_ru32(msg->data);

        zpl_string msg_buf = zpl_string_make_reserve(zpl_heap(), msg_size);
        librg_data_rptr(msg->data, msg_buf, msg_size);

        MafiaSDK::GetIndicators()->ConsoleAddText(reinterpret_cast<const char *>(msg_buf), msg_color);

        zpl_string_free(msg_buf);
    });

    librg_network_add(&network_context, NETWORK_KICK, [](librg_message* msg) {
        u16 len = librg_data_ru16(msg->data);
        zpl_string reason = zpl_string_make_reserve(zpl_heap(), len);
        librg_data_rptr(msg->data, reason, len);

        if (clientActiveState == ClientState_Connected) {
            switchClientState(ClientState_Infobox);
            std::string txt = "You have been kicked from this server!\n\nReason: " + std::string(reason);
            modules::infobox::displayError(txt.c_str());
            librg_network_stop(&network_context);
        }
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
            GlobalConfig.alreadyHasPassword = false;
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
