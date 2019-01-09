#pragma once

#include "events.hpp"
#include "messages.hpp"

inline auto mod_init_networking() {
    network_context.max_entities		= 1024;
    network_context.max_connections		= GlobalConfig.max_players;
    network_context.mode				= LIBRG_MODE_SERVER;
    network_context.tick_delay			= 32;
    network_context.world_size			= { 32000.0f, 32000.0f, 32000.0f };
    
    librg_init(&network_context);
    librg_option_set(LIBRG_DEFAULT_CLIENT_TYPE, TYPE_PLAYER);
    librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, on_librg_connection_request);
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connection_accept);
    librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, on_librg_connection_disconnect);
    librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
    librg_event_add(&network_context, LIBRG_ENTITY_UPDATE, on_librg_entityupdate);
    librg_event_add(&network_context, LIBRG_ENTITY_CREATE, on_librg_entitycreate);
    
    mod_add_network_messages();
}
