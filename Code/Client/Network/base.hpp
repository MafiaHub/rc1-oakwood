#pragma once

#include "config.hpp"
#include "events.hpp"
#include "messages.hpp"

/*
*  Librg connect to server
*/
inline auto mod_librg_connect() -> void {
	librg_address_t addr = { 27010, (char*)GlobalConfig.server_address.c_str()};
	librg_network_start(&network_context, addr);
}

inline auto mod_init_networking() -> void {
    network_context.mode = LIBRG_MODE_CLIENT;
	network_context.max_entities = 16;
	network_context.tick_delay = 32;
	network_context.world_size = { 5000.0f, 5000.0f, 0.0f };

	librg_init(&network_context);
    mod_add_network_events();
	mod_add_network_messages();
}