#pragma once

#include "Events/local_player.hpp"
#include "Events/player.hpp"
#include "Events/weapon_drop.hpp"
#include "Events/vehicle.hpp"
#include "Events/door.hpp"

inline auto mod_librg_connect() -> void;

void on_librg_connect(librg_event* evnt) {
    
    enet_peer_timeout(evnt->peer, 10, 5000, 10000);

    MafiaSDK::GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
    MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();

    effects::is_enabled = false;  
    cefgui::main_browser->visible = true;

    local_player.entity_id = evnt->entity->id;
    
    auto new_player = new mafia_player;
    strcpy(new_player->name, GlobalConfig.username.c_str());
    evnt->entity->type = TYPE_PLAYER;
    evnt->entity->user_data = (void*)new_player;

}

void on_librg_disconnect(librg_event* evnt) {

    cefgui::add_message("Disconnected from " + GlobalConfig.server_address + ".");
    auto player = get_local_player();
    if(player && player->ped) {
        player_despawn(player->ped);
        player->ped = nullptr;
    }

    librg_network_stop(evnt->ctx);
    mod_librg_connect();
}

void on_librg_entity_create(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_entitycreate(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            drop_entitycreate(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_entitycreate(evnt);
        } break;
        case TYPE_DOOR: {
            door_entitycreate(evnt);
        } break;
    }
}

void on_librg_entity_update(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_entityupdate(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_entityupdate(evnt);
        } break;
    }
}

void on_librg_entity_remove(librg_event* evnt) {
    switch (evnt->entity->type) {
        case TYPE_PLAYER: {
            player_entityremove(evnt);
        } break;
        case TYPE_WEAPONDROP: {
            drop_entityremove(evnt);
        } break;
        case TYPE_VEHICLE: {
            vehicle_entityremove(evnt);
        } break;
        case TYPE_DOOR: {
            door_entityremove(evnt);
        } break;
    }
}

void on_librg_clientstreamer_update(librg_event* evnt) {
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
    mod_player_add_events();
}
