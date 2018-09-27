#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "librg/librg.h"

/*
* STD Includes
*/
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <ostream>
#include <vector>
#include "librg/librg_ext.h"

/* 
* Shared
*/
#include "helpers.hpp"
#include "structs.hpp"
#include "messages.hpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Winmm.lib")
librg_ctx_t ctx = { 0 };

#include "player.hpp"
#include "weapon_drop.hpp"
#include "mode.hpp"

struct _request_player_data {
	char name[32];
} request_player_data;

auto on_librg_connection_request(librg_event_t* evnt) -> void {
	librg_data_rptr(evnt->data, request_player_data.name, sizeof(char) * 32);
}

auto on_librg_connection_accept(librg_event_t* evnt) -> void {
	evnt->entity->type = TYPE_PLAYER;
	auto ped = new mafia_player;
	strcpy(ped->name, request_player_data.name);

	evnt->entity->user_data = ped;

	librg_entity_control_set(evnt->ctx, evnt->entity->id, evnt->peer);
	mode_player_connected(evnt, ped);
}

auto on_librg_connection_disconnect(librg_event_t* evnt) -> void {
	mode_player_disconnected(evnt->entity);
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

auto main() -> int {

	mod_log("Initializing server ...");
	ctx.max_entities		= 16;
	ctx.max_connections		= 16;
	ctx.mode				= LIBRG_MODE_SERVER;
	ctx.tick_delay			= 32;
	ctx.world_size			= { 5000.0f, 5000.0f, 0.0f };

	librg_init(&ctx);
	librg_option_set(LIBRG_DEFAULT_CLIENT_TYPE, TYPE_PLAYER);
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_librg_connection_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_librg_connection_accept);
	librg_event_add(&ctx, LIBRG_CONNECTION_DISCONNECT, on_librg_connection_disconnect);
	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_librg_entityupdate);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_librg_entitycreate);

	//---- net messages
	librg_network_add(&ctx, NETWORK_PLAYER_SHOOT, [](librg_message_t* msg) {
		
		zpl_vec3 pos;
		auto entity = librg_entity_find(&ctx, msg->peer);
		librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

		mod_message_send_except(&ctx, NETWORK_PLAYER_SHOOT, msg->peer, [&](librg_data_t *data) {
			librg_data_went(data, entity->id);
			librg_data_wptr(data, &pos, sizeof(zpl_vec3));
		});
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message_t* msg) {

		auto entity = librg_entity_find(&ctx, msg->peer);
		if (!entity) return;
		u32 index = librg_data_ru32(msg->data);
		
		//set serverside current weapon index for player
		auto player = (mafia_player*)entity->user_data;
		player->current_weapon_id = index;

		mod_message_send_except(&ctx, NETWORK_PLAYER_WEAPON_CHANGE, msg->peer, [&](librg_data_t *data) {
			librg_data_went(data, entity->id);
			librg_data_wu32(data, index);
		});
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_DROP, [](librg_message_t* msg) {

		auto entity = librg_entity_find(&ctx, msg->peer);
		auto new_weapon_drop = new mafia_weapon_drop;

		librg_data_rptr(msg->data, &new_weapon_drop->weapon, sizeof(inventory_item));
		librg_data_rptr(msg->data, new_weapon_drop->model, sizeof(char) * 32);
		
		auto new_weapon_entity			= librg_entity_create(&ctx, TYPE_WEAPONDROP);
		new_weapon_entity->position		= entity->position;
		new_weapon_entity->position.y	+= 0.7f;
		new_weapon_entity->user_data	= new_weapon_drop;

		player_inventory_remove(entity, new_weapon_drop->weapon.weaponId, true, true);
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message_t* msg) {

		auto player_entity		= librg_entity_find(&ctx, msg->peer);
		librg_entity_id id		= librg_data_rent(msg->data);
		auto entity				= librg_entity_fetch(&ctx, id);
		auto weapon_drop		= (mafia_weapon_drop*)entity->user_data;

		player_inventory_add(player_entity, &weapon_drop->weapon, true, true);

		if (entity) {
			if (entity->user_data) 
				delete entity->user_data;
			librg_entity_destroy(&ctx, id);
		}
	});

	librg_network_add(&ctx, NETWORK_PLAYER_HIT, [](librg_message_t* msg) {

		auto sender_ent = librg_entity_find(&ctx, msg->peer);
		librg_entity_id target_id = librg_data_rent(msg->data);
		u32 hit_type = librg_data_ru32(msg->data);
		zpl_vec3 unk1, unk2, unk3;
		librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));
		f32 damage = librg_data_rf32(msg->data);
		u32 player_part = librg_data_ru32(msg->data);

		auto player = (mafia_player*)sender_ent->user_data;
		if (player) {
			if (player->health - damage < 0.0f)
				player->health = 0.0f;
			else player->health -= damage;
		}

		mod_message_send_except(&ctx, NETWORK_PLAYER_HIT, msg->peer, [&](librg_data_t *data) {
			librg_data_went(data, sender_ent->id);
			librg_data_wu32(data, hit_type);
			librg_data_wptr(data, (void*)&unk1, sizeof(zpl_vec3));
			librg_data_wptr(data, (void*)&unk2, sizeof(zpl_vec3));
			librg_data_wptr(data, (void*)&unk3, sizeof(zpl_vec3));
			librg_data_wf32(data, damage);
			librg_data_wu32(data, player_part);
		});
	});

	librg_network_add(&ctx, NETWORK_PLAYER_DIE, [](librg_message_t* msg) {
		auto sender_ent = librg_entity_find(&ctx, msg->peer);

		mod_message_send_except(&ctx, NETWORK_PLAYER_DIE, msg->peer, [&](librg_data_t *data) {
			librg_data_went(data, sender_ent->id);
		});

		//remove old player entity and make new :) 
		if (sender_ent->user_data) {
			auto player = (mafia_player*)sender_ent->user_data;

			//change health
			player->health = 0.0f;

			//clear player inventory
			for (size_t i = 0; i < 8; i++)
				player->inventory.items[i] = { -1, 0, 0, 0 };
		}

		mode_player_died(sender_ent);
	});

	librg_address_t addr = { 27010 };
	librg_network_start(&ctx, addr);
	mod_log("Server started");

	bool running = true;
	while (running) {
		librg_tick(&ctx);
		zpl_sleep_ms(2);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
	return 0;
}