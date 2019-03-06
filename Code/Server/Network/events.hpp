#pragma once

#include "event_data.hpp"

auto on_librg_connection_request(librg_event* evnt) -> void {
    auto build_ver = librg_data_ru16(evnt->data);

    if (build_ver != OAK_BUILD_VERSION) {
        librg_event_reject(evnt);
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

    mod_log(zpl_bprintf("Player '%s' has been connected!\n", ped->name));
}

auto on_librg_connection_disconnect(librg_event* evnt) -> void {

    if (evnt->entity && evnt->entity->type == TYPE_PLAYER)
    {
        auto player = (mafia_player *)evnt->entity->user_data;

        ZPL_ASSERT_NOT_NULL(player);

        if (gm.on_player_disconnected)
            gm.on_player_disconnected(evnt, evnt->entity);

        mod_log(zpl_bprintf("Player '%s' has been disconnected!\n", player->name));
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
