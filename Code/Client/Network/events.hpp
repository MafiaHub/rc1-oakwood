#pragma once
inline auto mod_librg_connect() -> void;

void on_librg_connect(librg_event* evnt) {
    
    enet_peer_timeout(evnt->peer, 10, 5000, 10000);

    MafiaSDK::GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
    MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();

    local_player.entity_id = evnt->entity->id;
    
    auto new_player = new mafia_player();
    strcpy(new_player->name, GlobalConfig.username);
    evnt->entity->type = TYPE_PLAYER;
    evnt->entity->user_data = (void*)new_player;
}

void on_librg_disconnect(librg_event* evnt) {

    //chat::add_message("Disconnected from " + std::string(GlobalConfig.server_address) + ".");
    auto player = modules::player::get_local_player();
    if(player && player->ped) {
        modules::player::despawn(player->ped);
        player->ped = nullptr;
    }
    
    car_delte_queue.clear();
    MafiaSDK::GetMission()->MapLoad("freeride");
    librg_network_stop(evnt->ctx);
    mod_librg_connect();
}

void on_librg_entity_create(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::entitycreate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            //entitycreate(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::entitycreate(evnt);
        } break;
        case TYPE_DOOR: {
            modules::door::entitycreate(evnt);
        } break;
    }
}

void on_librg_entity_update(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::entityupdate(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::entityupdate(evnt);
        } break;
    }
}

void on_librg_entity_remove(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::entityremove(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            //drop_entityremove(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::entityremove(evnt);
        } break;
        case TYPE_DOOR: {
            modules::door::entityremove(evnt);
        } break;
    }
}

void on_librg_clientstreamer_update(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::clientstreamer_update(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::clientstreamer_update(evnt);
        } break;
        case TYPE_DOOR: {
            modules::door::clientstreamer_update(evnt);
        } break;
    }
}

void on_librg_clientstreamer_add(librg_event* evnt) {
    evnt->entity->flags &= ~ENTITY_INTERPOLATED;
}

void on_librg_clientstreamer_remove(librg_event* evnt) {
    evnt->entity->flags |= ENTITY_INTERPOLATED;
}

auto mod_add_network_events() {
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
    librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, on_librg_disconnect);
    librg_event_add(&network_context, LIBRG_ENTITY_CREATE, on_librg_entity_create);
    librg_event_add(&network_context, LIBRG_ENTITY_UPDATE, on_librg_entity_update);
    librg_event_add(&network_context, LIBRG_ENTITY_REMOVE, on_librg_entity_remove);
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_ADD, on_librg_clientstreamer_add);
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_REMOVE, on_librg_clientstreamer_remove);

#ifdef LIBRG_DEBUG
    librg_event_add(&network_context, LIBRG_ENTITY_CREATE, [](librg_event *evnt) {
        printf("LIBRG_ENTITY_CREATE: %d\n", evnt->entity->id);
    });
    librg_event_add(&network_context, LIBRG_ENTITY_REMOVE, [](librg_event *evnt) {
        printf("LIBRG_ENTITY_REMOVE: %d\n", evnt->entity->id);
    });
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_ADD, [](librg_event *evnt) {
        printf("LIBRG_CLIENT_STREAMER_ADD: %d\n", evnt->entity->id);
    });
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_REMOVE, [](librg_event *evnt) {
        printf("LIBRG_CLIENT_STREAMER_ADD: %d\n", evnt->entity->id);
    });
#endif

    librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, [](librg_event *evnt) {
        char nickname[32];
        strcpy(nickname, GlobalConfig.username);
        librg_data_wu64(evnt->data, OAK_BUILD_MAGIC);
        librg_data_wu64(evnt->data, OAK_BUILD_VERSION);
        librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);
    });
}
