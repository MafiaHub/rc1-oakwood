int oak_dialog_register()
{
    librg_network_add(&network_context, NETWORK_DIALOG_DONE, [](librg_message* msg) {
        oak_player pid = (oak_player)librg_entity_find(&network_context, msg->peer)->user_data;
        if (oak_player_invalid(pid)) {
            return;
        }

        auto player = oak_entity_player_get(pid);

        int id = librg_data_ri32(msg->data);
        int sel = librg_data_ri32(msg->data);
        char text[128];
        librg_data_rptr(msg->data, &text, 128);

        if (GlobalConfig.api_type == "internal")
            oak_angel_event_dialog_done(pid, id, sel, text);
        else
            oak_bridge_event_dialog_done(pid, id, sel, text);
    });

    return 0;
}
