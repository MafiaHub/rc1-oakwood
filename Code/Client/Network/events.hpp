#pragma once

#include "Events/local_player.hpp"
#include "Events/player.hpp"
#include "Events/weapon_drop.hpp"

inline auto mod_librg_connect() -> void;

void on_librg_connect(librg_event_t* evnt) {
	
	MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
	MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();
	chat::chat_messages.push_back(std::make_pair(ImVec4(1.0, 1.0, 1.0, 1.0), "Connected to " + GlobalConfig.server_address));

	auto local_player_data = new mafia_player;
	evnt->entity->user_data = local_player_data;
	local_player.entity = *evnt->entity;
}

void on_librg_disconnect(librg_event_t* evnt) {

	chat::chat_messages.push_back(std::make_pair(ImVec4(1.0, 1.0, 1.0, 1.0), "Disconnected from " + GlobalConfig.server_address + "."));
	if(local_player.ped) {
		player_despawn(local_player.ped);
		local_player.ped = nullptr;
	}
	mod_librg_connect();
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

auto mod_add_network_events() {
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
	librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, on_librg_disconnect);
	librg_event_add(&network_context, LIBRG_ENTITY_CREATE, on_librg_entity_create);
	librg_event_add(&network_context, LIBRG_ENTITY_UPDATE, on_librg_entity_update);
	librg_event_add(&network_context, LIBRG_ENTITY_REMOVE, on_librg_entity_remove);
	librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);

    mod_player_add_events();
}
