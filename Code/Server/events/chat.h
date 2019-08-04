int oak_chat_register() {
    // librg_network_add(&network_context, NETWORK_NPC_CREATE, [](librg_message* msg) {
    //     auto entity = librg_entity_find(&network_context, msg->peer);
    //     auto creator_player = (mafia_player *)entity->user_data;

    //     if(creator_player) {
    //         auto new_npc_entity = librg_entity_create(&network_context, TYPE_PLAYER);

    //         if(new_npc_entity) {

    //             auto new_npc_userdata = new mafia_player();
    //             new_npc_userdata->streamer_entity_id = entity->id;
    //             strcpy(new_npc_userdata->name, "Borat");
    //             strcpy(new_npc_userdata->model, "Tommy.i3d");
    //             new_npc_entity->user_data = (void*)new_npc_userdata;

    //             librg_entity_control_set(&network_context, new_npc_entity->id, msg->peer);
    //             new_npc_entity->position = entity->position;
    //         }
    //     }
    // });

    librg_network_add(&network_context, NETWORK_SEND_CHAT_MSG, [](librg_message* msg) {
        char text[128] = { 0 };


        auto entity = librg_entity_find(&network_context, msg->peer);
        auto pid = (oak_player)entity->user_data;
        auto player = oak_entity_player_get(pid);

        auto text_len = librg_data_ru16(msg->data);
        text_len = zpl_min(text_len, 128);
        librg_data_rptr(msg->data, text, text_len);

        auto is_handled = false;

        if (zpl_strcmp(text, "suka") == 0) {
            oak_log("player %d wants to spawn a car\n", entity->id);

            auto car = oak_vehicle_spawn("FThot00.i3d", zpl_strlen("FThot00.i3d"));
            auto vec = oak_player_position_get(pid); {
                vec.y += 1.0f;
                vec.z += 5.0f;
            }

            oak_vehicle_position_set(car, vec);
            oak_log("player spawned a car %d at: %f %f %f\n", car, EXPLODE_VEC(vec));
        }

        oak_bridge_event_player_chat(pid, text);
    });

    librg_network_add(&network_context, NETWORK_SEND_VOIP_DATA, [](librg_message* msg) {

        auto entity = librg_entity_find(&network_context, msg->peer);
        u32 encoded_buffer_size = librg_data_ru32(msg->data);
        void* buffer_data = malloc(encoded_buffer_size);
        librg_data_rptr(msg->data, buffer_data, encoded_buffer_size);

        if(entity) {
            mod_message_send_except(&network_context, NETWORK_SEND_VOIP_DATA, msg->peer, [&](librg_data *data) {
                librg_data_went(data, entity->id);
                librg_data_wf32(data, zpl_time_now());
                librg_data_wu32(data, encoded_buffer_size);
                librg_data_wptr(data, buffer_data, encoded_buffer_size);
            });
        }

        free(buffer_data);
    });

    return 0;
}
