// #pragma once

// #include "events.hpp"
// #include "messages.hpp"

// namespace network {
//     void update() {
//         librg_tick(&network_context);

//         if (gm.on_server_tick)
//             gm.on_server_tick();
//     }

//     inline auto init() {
//         mod_log("Initializing librg service...");
//         network_context.max_entities		= OAK_MAX_ENTITIES;
//         network_context.max_connections		= GlobalConfig.max_players;
//         network_context.mode				= LIBRG_MODE_SERVER;
//         network_context.tick_delay			= OAK_TICK_DELAY;
//         network_context.world_size			= { 32000.0f, 32000.0f, 32000.0f };
        
//         librg_init(&network_context);
//         librg_option_set(LIBRG_DEFAULT_CLIENT_TYPE, TYPE_PLAYER);
//         librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, on_librg_connection_request);
//         librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, on_librg_connection_accept);
//         librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, on_librg_connection_disconnect);
//         librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
//         librg_event_add(&network_context, LIBRG_ENTITY_UPDATE, on_librg_entityupdate);
//         librg_event_add(&network_context, LIBRG_ENTITY_CREATE, on_librg_entitycreate);
        
//         mod_add_network_messages();

// #ifdef LIBRG_DEBUG
//         librg_event_add(&network_context, LIBRG_ENTITY_CREATE, [](librg_event *evnt) {
//             printf("LIBRG_ENTITY_CREATE: %d\n", evnt->entity->id);
//         });
//         librg_event_add(&network_context, LIBRG_ENTITY_REMOVE, [](librg_event *evnt) {
//             printf("LIBRG_ENTITY_REMOVE: %d\n", evnt->entity->id);
//         });
//         librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_ADD, [](librg_event *evnt) {
//             printf("LIBRG_CLIENT_STREAMER_ADD: %d\n", evnt->entity->id);
//         });
//         librg_event_add(&network_context, LIBRG_CLIENT_STREAMER_REMOVE, [](librg_event *evnt) {
//             printf("LIBRG_CLIENT_STREAMER_ADD: %d\n", evnt->entity->id);
//         });
// #endif

//         librg_address addr = { (i32)GlobalConfig.port };
//         librg_network_start(&network_context, addr);
//         GlobalConfig.players = 0;
//     }

//     auto shutdown() {
//         mod_log("Stopping librg service...");
//         librg_network_stop(&network_context);
//         librg_free(&network_context);
//     }
// }
