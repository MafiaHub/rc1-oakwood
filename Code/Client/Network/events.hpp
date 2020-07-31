#pragma once
inline auto mod_librg_connect() -> void;

void on_librg_connect(librg_event *evnt)
{

    enet_peer_timeout(evnt->peer, 10, 5000, 10000);

    MafiaSDK::GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
    MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();

    local_player.entity_id = evnt->entity->id;

    auto new_player = new mafia_player();
    strcpy(new_player->name, GlobalConfig.username);
    evnt->entity->type = TYPE_PLAYER;
    evnt->entity->user_data = (void *)new_player;
}


void handle_disconnection()
{
    //chat::add_message("Disconnected from " + std::string(GlobalConfig.server_address) + ".");

    loadingscreen::changeTex(GlobalConfig.localpath + "\\files\\oakwood.png");

    modules::chat::set_chat_y(1);
    modules::chat::set_chat_ysize(400);

    modpath = "";

    if (modules::audiostream::is_playing())
        modules::audiostream::stop();

    auto player = modules::player::get_local_player();
    if (player && player->ped)
    {
        modules::player::despawn(player->ped);
        player->ped = nullptr;
        zpl_zero_item(player);
    }

    zpl_zero_item(&local_player);
    local_player.spec_id = -1;
    local_player.last_spec_id = -1;

    for (auto car_to_remove : car_delte_queue)
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(car_to_remove);

    car_delte_queue.clear();

    GlobalConfig.alreadyHasPassword = false;
    GlobalConfig.needToDownload = false;

    //librg_free(&network_context);
    //mod_init_networking();
}

void on_librg_disconnect(librg_event *evnt)
{
    if (clientActiveState == ClientState_Infobox)
    {
        return;
    }

    int status = (int)evnt->user_data;

    /* handle timeout (hacky; librg sux) */
    if (status == 1)
    {
        return;
    }

    handle_disconnection();

    menuActiveState = Menu_Chat;
    MafiaSDK::GetMission()->MapLoad("tutorial");
    switchClientState(ClientState_Browser);
}

void on_librg_timeout(librg_event *evnt)
{
    if (clientActiveState == ClientState_Infobox)
    {
        return;
    }

    handle_disconnection();

    MafiaSDK::GetIndicators()->ConsoleAddText(zpl_bprintf("Trying to connect to %s:%d...", ServerInfo::lastServer.server_ip.c_str(), ServerInfo::lastServer.port), 0xFFFFFFFF);
    ServerInfo::join_last_server(clientActiveState == ClientState_Connected);
}

void on_librg_entity_create(librg_event *evnt)
{
    switch (evnt->entity->type)
    {
    case TYPE_PLAYER:
    {
        modules::player::entitycreate(evnt);
    }
    break;
    case TYPE_WEAPONDROP:
    {
        //entitycreate(evnt);
    }
    break;
    case TYPE_VEHICLE:
    {
        modules::vehicle::entitycreate(evnt);
    }
    break;
    case TYPE_DOOR:
    {
        modules::door::entitycreate(evnt);
    }
    break;
    }
}

void on_librg_entity_update(librg_event *evnt)
{
    switch (evnt->entity->type)
    {
    case TYPE_PLAYER:
    {
        modules::player::entityupdate(evnt);
    }
    break;
    case TYPE_VEHICLE:
    {
        modules::vehicle::entityupdate(evnt);
    }
    break;
    }
}

void on_librg_entity_remove(librg_event *evnt)
{
    switch (evnt->entity->type)
    {
    case TYPE_PLAYER:
    {
        modules::player::entityremove(evnt);
    }
    break;
    case TYPE_WEAPONDROP:
    {
        //drop_entityremove(evnt);
    }
    break;
    case TYPE_VEHICLE:
    {
        modules::vehicle::entityremove(evnt);
    }
    break;
    case TYPE_DOOR:
    {
        modules::door::entityremove(evnt);
    }
    break;
    }
}

void on_librg_clientstreamer_update(librg_event *evnt)
{
    switch (evnt->entity->type)
    {
    case TYPE_PLAYER:
    {
        modules::player::clientstreamer_update(evnt);
    }
    break;
    case TYPE_VEHICLE:
    {
        modules::vehicle::clientstreamer_update(evnt);
    }
    break;
    case TYPE_DOOR:
    {
        modules::door::clientstreamer_update(evnt);
    }
    break;
    }
}

void on_librg_clientstreamer_add(librg_event *evnt)
{
    evnt->entity->flags &= ~ENTITY_INTERPOLATED;
}

void on_librg_clientstreamer_remove(librg_event *evnt)
{
    evnt->entity->flags |= ENTITY_INTERPOLATED;
}

auto mod_add_network_events()
{
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
    librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, on_librg_disconnect);
    librg_event_add(&network_context, LIBRG_CONNECTION_TIMEOUT, on_librg_timeout);
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
        librg_data_wu8(evnt->data, OAK_VERSION_MAJOR);
        librg_data_wu8(evnt->data, OAK_VERSION_MINOR);
        librg_data_wu8(evnt->data, OAK_VERSION_PATCH);
        librg_data_wu8(evnt->data, OAK_BUILD_CHANNEL);
        librg_data_wu64(evnt->data, hwid::getID());
        librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);

        if (GlobalConfig.passworded) {
            librg_data_wptr(evnt->data, GlobalConfig.password, sizeof(char) * 32);
        }

#ifdef OAKWOOD_DEBUG
        zpl_printf("GUID: %llu\n", hwid::getID());
#endif
    });
}
