/* DOOR EVENTS */

void oak_ev_door_create(librg_event *e) {
    auto door = oak_entity_door_get((oak_door)e->entity->user_data);
    ZPL_ASSERT_NOT_NULL(door);

    auto name_len = strlen(door->name);

    librg_data_wu32(e->data, name_len);
    librg_data_wptr(e->data, door->name, name_len);
    librg_data_wf32(e->data, door->angle);
    librg_data_wu8(e->data, door->open_side);
}

void oak_ev_door_update(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_door_remove(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_door_client_add(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_door_client_update(librg_event *e) {
    auto door = oak_entity_door_get((oak_door)e->entity->user_data);
    ZPL_ASSERT_NOT_NULL(door);

    door->angle = librg_data_rf32(e->data);
    door->open_side = librg_data_ru8(e->data);
}

void oak_ev_door_client_remove(librg_event *e) {
    // NOTE: not implemented
}

/* DOOR MESSAGES */

int oak_door_register() {
    librg_network_add(&network_context, NETWORK_PLAYER_USE_DOORS, [](librg_message *msg) {
        auto sender_ent = librg_entity_find(&network_context, msg->peer);
        auto door_name_len = librg_data_ru32(msg->data);
        char door_name[32];

        door_name_len = zpl_min(door_name_len, 32);
        librg_data_rptr(msg->data, door_name, door_name_len);
        door_name[door_name_len] = '\0';

        auto door_state = librg_data_ru32(msg->data);
        librg_entity* door_ent = nullptr;
        for (u32 i = 0; i < network_context.max_entities; i++) {
            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (!entity) continue;
            if (entity->type == TYPE_DOOR) {
                if(strcmp(oak_door_name_get((oak_door)entity->user_data), door_name) == 0) {
                    door_ent = entity;
                    break;
                }
            }
        }

        //If doors doesen't exists yet we create new one
        if (door_ent == nullptr) {
            auto newdoor = oak_door_create(door_name, zpl_strlen(door_name));
            door_ent = oak_entity_door_get(newdoor)->librg_entity;
        }

        librg_entity_control_set(&network_context, door_ent->id, sender_ent->client_peer);

        // NOTE: add event trigger

        mod_message_send(&network_context, NETWORK_PLAYER_USE_DOORS, [&](librg_data *data) {
            librg_data_went(data, sender_ent->id);
            librg_data_wu32(data, door_name_len);
            librg_data_wptr(data, door_name, door_name_len);
            librg_data_wu32(data, door_state);
        });
    });

    return 0;
}
