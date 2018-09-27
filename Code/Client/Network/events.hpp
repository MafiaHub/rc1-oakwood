#pragma once

#include "Events/local_player.hpp"
#include "Events/player.hpp"
#include "Events/weapon_drop.hpp"

void on_librg_connect(librg_event_t* evnt) {
	
	MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();

	auto local_player_data = new mafia_player;
	evnt->entity->user_data = local_player_data;
	local_player.entity = *evnt->entity;
}

void on_librg_entity_create(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entitycreate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			drop_entitycreate(evnt);
		} break;
	}
}

void on_librg_entity_update(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entityupdate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			
		} break;
	}
}

void on_librg_entity_remove(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entityremove(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			drop_entityremove(evnt);
		} break;
	}
}

void on_librg_clientstreamer_update(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_clientstreamer_update(evnt);
		} break;
		case TYPE_WEAPONDROP: {
		} break;
	}
}

auto mod_add_network_events() -> void {
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
	librg_event_add(&network_context, LIBRG_ENTITY_CREATE, on_librg_entity_create);
	librg_event_add(&network_context, LIBRG_ENTITY_UPDATE, on_librg_entity_update);
	librg_event_add(&network_context, LIBRG_ENTITY_REMOVE, on_librg_entity_remove);
	librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);

    mod_player_add_events();
}
