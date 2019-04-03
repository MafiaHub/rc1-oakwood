#pragma once

#include "events.hpp"
#include "messages.hpp"

/*
*  Librg connect to server
*/
inline auto mod_librg_connect() -> void {
    mod_log(zpl_bprintf("Connecting to %s:%d...", GlobalConfig.server_address, GlobalConfig.port));
    librg_address addr = { GlobalConfig.port, GlobalConfig.server_address};
    librg_network_start(&network_context, addr);
}

inline auto mod_init_networking() {
    network_context.mode = LIBRG_MODE_CLIENT;
    network_context.max_entities = OAK_MAX_ENTITIES;
    network_context.tick_delay = OAK_TICK_DELAY;
    network_context.world_size = { 5000.0f, 5000.0f, 0.0f };

    librg_init(&network_context);
    mod_add_network_events();
    mod_add_network_messages();
}
