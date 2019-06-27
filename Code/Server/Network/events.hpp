#pragma once

auto on_librg_connection_request(librg_event* evnt) -> void {
    auto build_magic = librg_data_ru64(evnt->data);
    auto build_ver = librg_data_ru64(evnt->data);

    if (build_magic != OAK_BUILD_MAGIC || build_ver != OAK_BUILD_VERSION) {
        mod_log(zpl_bprintf("Connection has been rejected!\nOur magic: %X\tTheir magic: %X\nOur version: %X\tTheir version: %X\n", OAK_BUILD_MAGIC, build_magic, OAK_BUILD_VERSION, build_ver));
        librg_event_reject(evnt);

        librg_send_to(&network_context, NETWORK_SEND_REJECTION, evnt->peer, data, {});
    }

    librg_data_rptr(evnt->data, request_player_data.name, sizeof(char) * 32);
}

auto on_librg_connection_accept(librg_event* evnt) -> void {
    evnt->entity->type = TYPE_PLAYER;
    GlobalConfig.players++;

    auto ped = new mafia_player;
    strcpy(ped->name, request_player_data.name);

    evnt->entity->user_data = ped;

    librg_entity_control_set(evnt->ctx, evnt->entity->id, evnt->peer);
    enet_peer_timeout(evnt->peer, 10, 5000, 10000);
    evnt->entity->stream_range = 500.0f;

    if (gm.on_player_connected)
        gm.on_player_connected(evnt, evnt->entity, ped);

    mod_log(zpl_bprintf("Player '%s' has been connected!", ped->name));
    connected_players.push_back(evnt->entity);
}

auto on_librg_connection_disconnect(librg_event* evnt) -> void {

    if (evnt->entity && evnt->entity->type == TYPE_PLAYER)
    {
        auto player = (mafia_player *)evnt->entity->user_data;
        ZPL_ASSERT_NOT_NULL(player);

        connected_players.erase(std::remove(connected_players.begin(), connected_players.end(), evnt->entity));

        if (gm.on_player_disconnected)
            gm.on_player_disconnected(evnt, evnt->entity);

        mod_log(zpl_bprintf("Player '%s' has been disconnected!", player->name));
        modules::player::connection_disconnect(evnt);
        GlobalConfig.players--;
    }
}

auto on_librg_clientstreamer_update(librg_event* evnt) -> void {
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

auto on_librg_entityupdate(librg_event* evnt) -> void {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::entityupdate(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::entityupdate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
        } break;
    }
}

auto on_librg_entitycreate(librg_event* evnt) -> void {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            modules::player::entitycreate(evnt);
        } break;
        case TYPE_VEHICLE: {
            modules::vehicle::entitycreate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            modules::player::drop_entitycreate(evnt);
        } break;
        case TYPE_DOOR: {
            modules::door::entitycreate(evnt);
        } break;
    }
}
