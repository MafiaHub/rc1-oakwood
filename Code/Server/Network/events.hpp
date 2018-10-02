#pragma once

#include "event_data.hpp"
#include "Events/player.hpp"

auto on_librg_connection_request(librg_event_t* evnt) -> void {
	librg_data_rptr(evnt->data, request_player_data.name, sizeof(char) * 32);
}

auto on_librg_connection_accept(librg_event_t* evnt) -> void {
	evnt->entity->type = TYPE_PLAYER;
	auto ped = new mafia_player;
	strcpy(ped->name, request_player_data.name);

	evnt->entity->user_data = ped;

	librg_entity_control_set(evnt->ctx, evnt->entity->id, evnt->peer);

    if (gm.on_player_connected)
        gm.on_player_connected(evnt, evnt->entity, ped);
}

auto on_librg_connection_disconnect(librg_event_t* evnt) -> void {
    if (gm.on_player_disconnected)
        gm.on_player_disconnected(evnt, evnt->entity);
}

auto on_librg_clientstreamer_update(librg_event_t* evnt) -> void {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_clientstreamer_update(evnt);
		} break;
		case TYPE_WEAPONDROP: {
		} break;
	}
}

auto on_librg_entityupdate(librg_event_t* evnt) -> void {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entityupdate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
		} break;
	}
}

auto on_librg_entitycreate(librg_event_t* evnt) -> void {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entitycreate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			drop_entitycreate(evnt);
		} break;
	}
}
