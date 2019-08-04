zpl_global librg_ctx network_context = {0};

#define OAK_ROUTER_CASE(TYPE, NAME, EVENT, EVENT_NAME) \
    librg_event_add(&network_context,EVENT,[](librg_event*e){if(e->entity->type==TYPE){ZPL_JOIN4(oak_ev_,NAME,_,EVENT_NAME)(e);}});

#define OAK_ROUTER_HANDLER(TYPE, NAME) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_ENTITY_CREATE, create) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_ENTITY_UPDATE, update) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_ENTITY_REMOVE, remove) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_CLIENT_STREAMER_ADD, client_add) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_CLIENT_STREAMER_UPDATE, client_update) \
    OAK_ROUTER_CASE(TYPE, NAME, LIBRG_CLIENT_STREAMER_REMOVE, client_remove)

int oak_network_init() {
    oak_log("[info] starting network...\n");

    /* set up defaults */
    network_context.max_entities        = OAK_MAX_ENTITIES;
    network_context.max_connections     = GlobalConfig.max_players;
    network_context.mode                = LIBRG_MODE_SERVER;
    network_context.tick_delay          = OAK_TICK_DELAY;
    network_context.world_size          = { 32000.0f, 32000.0f, 32000.0f };

    librg_init(&network_context);

    /* attach general handlers */
    librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, oak_ev_player_requested);
    librg_event_add(&network_context, LIBRG_CONNECTION_ACCEPT, oak_ev_player_connected);
    librg_event_add(&network_context, LIBRG_CONNECTION_DISCONNECT, oak_ev_player_disconnected);

    /* attach handlers for our entities */
    OAK_ROUTER_HANDLER(OAK_PLAYER, player);
    OAK_ROUTER_HANDLER(OAK_VEHICLE, vehicle);
    OAK_ROUTER_HANDLER(OAK_DOOR, door);

    oak_player_register();
    oak_vehicle_register();
    oak_door_register();
    oak_vehicle_player_register();
    oak_weapon_register();
    oak_chat_register();

    librg_address addr = { (i32)GlobalConfig.port };
    librg_network_start(&network_context, addr);

    return 0;
}

int oak_network_tick() {
    librg_tick(&network_context);

    // TODO: add event trigger

    return 0;
}

int oak_network_free() {
    oak_log("[info] stopping network...\n");

    librg_network_stop(&network_context);
    librg_free(&network_context);

    return 0;
}

#undef OAK_ROUTER_CASE
#undef OAK_ROUTER_HANDLER
