#pragma once

#include "event_data.hpp"
#include "Events/player.hpp"
#include "Events/vehicle.hpp"
#include "Events/door.hpp"

auto on_librg_connection_request(librg_event* evnt) -> void {
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

    if (gm.on_player_connected)
        gm.on_player_connected(evnt, evnt->entity, ped);

    printf("Player '%s' has been connected!\n", ped->name);
}

auto on_librg_connection_disconnect(librg_event* evnt) -> void {

    if (evnt->entity && evnt->entity->type == TYPE_PLAYER)
    {
        if (evnt->entity->user_data) {
            auto player = (mafia_player*)evnt->entity->user_data;
            printf("Player '%s' has been disconnected!\n", player->name);
        }

        if (gm.on_player_disconnected)
            gm.on_player_disconnected(evnt, evnt->entity);

        player_connection_disconnect(evnt);
        GlobalConfig.players--;
    }
}

auto on_librg_clientstreamer_update(librg_event* evnt) -> void {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_clientstreamer_update(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_clientstreamer_update(evnt);
        } break;
        case TYPE_DOOR: {
            door_clientstreamer_update(evnt);
        } break;
    }
}

auto on_librg_entityupdate(librg_event* evnt) -> void {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_entityupdate(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_entityupdate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
        } break;
    }
}

auto on_librg_entitycreate(librg_event* evnt) -> void {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_entitycreate(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_entitycreate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            drop_entitycreate(evnt);
        } break;
        case TYPE_DOOR: {
            door_entitycreate(evnt);
        } break;
    }
}
