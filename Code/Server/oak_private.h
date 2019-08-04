#ifndef OAK_PRIVATE_H
#define OAK_PRIVATE_H

/* LOGGER */

int oak_log_init();
int oak_log_free();
int oak_log(const char *fmt, ...);

/* CONFIG */

int oak_config_init();
int oak_config_free();
int oak_config_port_get();
int oak_config_maxplayers_get();
int oak_config_visible_get();
int oak_config_whitelistonly_get();
const char *oak_config_name_get();
const char *oak_config_host_get();
const char *oak_config_mapname_get();
const char *oak_config_gamemode_get();

/* BRIDGE */

int oak_bridge_init();
int oak_bridge_free();
int oak_bridge_tick();
int oak_bridge_router(const char *, usize);

void oak_bridge_event_player_connect(oak_player player);
void oak_bridge_event_player_disconnect(oak_player player);
void oak_bridge_event_player_death(oak_player player);
void oak_bridge_event_player_hit(oak_player player, oak_player attacker, float damage);
void oak_bridge_event_player_key(oak_player player, int key);
void oak_bridge_event_player_chat(oak_player player, oak_string text);
void oak_bridge_event_vehicle_destroy(oak_vehicle vehicle);

/* NETWORK */

int oak_network_init();
int oak_network_free();
int oak_network_tick();

#define OAK_NETOWORK_DECLS(TYPE) \
    void ZPL_JOIN3(oak_ev_,TYPE,_create)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_update)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_remove)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_add)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_update)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_remove)(librg_event *);

void oak_ev_player_requested(librg_event *);
void oak_ev_player_connected(librg_event *);
void oak_ev_player_disconnected(librg_event *);

OAK_NETOWORK_DECLS(player);
OAK_NETOWORK_DECLS(vehicle);
OAK_NETOWORK_DECLS(door);

#undef OAK_NETOWORK_DECLS

int oak_player_register();
int oak_vehicle_register();
int oak_door_register();
int oak_vehicle_player_register();
int oak_weapon_register();
int oak_chat_register();

/* ENTITIES */

int oak_entities_init();
u32 oak_entity_next(oak_type type);
int oak_entity_free(oak_type type);
int oak_entity_invalid(oak_type type, u32);
oak_object *oak_entity_get(oak_type, u32 id);

mafia_player *oak_entity_player_get(oak_player);
mafia_vehicle *oak_entity_vehicle_get(oak_vehicle);
mafia_door *oak_entity_door_get(oak_door);

/* VEHICLES */

oak_player oak_vehicle_streamer_get(oak_vehicle);
int oak_vehicle_streamer_set(oak_vehicle, oak_player);

/* SEMI PUBLIC METHODS */

oak_player oak_player_create(librg_event *);
int oak_player_destroy(librg_event *);



#endif // OAK_PRIVATE_H
