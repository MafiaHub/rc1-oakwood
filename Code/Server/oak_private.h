#ifndef OAK_PRIVATE_H
#define OAK_PRIVATE_H

/* LOGGER */

int oak_log_init();
int oak_log_free();
int oak_log(const char *fmt, ...);

/* CONFIG */

int oak_config_init();

int oak_config_port_get();
int oak_config_maxplayers_get();
int oak_config_visible_get();
void oak_config_name_set(const char *);
const char *oak_config_name_get();
const char *oak_config_host_get();
const char *oak_config_mapname_get();

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
void oak_bridge_event_player_chat(oak_player player, const char *text);
void oak_bridge_event_vehicle_destroy(oak_vehicle vehicle);
void oak_bridge_event_console(const char *text);

/* NETWORK */

int oak_network_init();
int oak_network_free();
int oak_network_tick();

// todo: find a better place for the ctx
librg_ctx *oak_network_ctx_get();

#define OAK_NETWORK_DECLS(TYPE) \
    void ZPL_JOIN3(oak_ev_,TYPE,_create)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_update)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_remove)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_add)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_update)(librg_event *); \
    void ZPL_JOIN3(oak_ev_,TYPE,_client_remove)(librg_event *);

void oak_ev_player_requested(librg_event *);
void oak_ev_player_connected(librg_event *);
void oak_ev_player_disconnected(librg_event *);

OAK_NETWORK_DECLS(player);
OAK_NETWORK_DECLS(vehicle);
OAK_NETWORK_DECLS(door);

#undef OAK_NETWORK_DECLS

int oak_player_register();
int oak_vehicle_register();
int oak_door_register();
int oak_vehicle_player_register();
int oak_weapon_register();
int oak_chat_register();

/* ENTITIES */

int oak_entities_init();
int oak_entities_count_get(oak_type);
oak_object **oak_entity_list(oak_type, int *);
u32 oak_entity_next(oak_type type);
int oak_entity_free(oak_type type);
int oak_entity_invalid(oak_type type, u32);
oak_object *oak_entity_get(oak_type, u32 id);
int oak_entity_get_id_from_librg(librg_entity *);

mafia_player *oak_entity_player_get(oak_player);
mafia_vehicle *oak_entity_vehicle_get(oak_vehicle);
mafia_door *oak_entity_door_get(oak_door);

#define OAK_ENTITY_LIBRG_GETTER(NAME, ID) \
ZPL_JOIN2(mafia_,NAME) *ZPL_JOIN3(oak_entity_,NAME,_get_from_librg)(librg_entity *entity) { \
    ZPL_ASSERT_NOT_NULL(entity); \
    return (ZPL_JOIN2(mafia_,NAME) *)oak_entity_get(ID, oak_entity_get_id_from_librg(entity)); \
}

OAK_ENTITY_LIBRG_GETTER(player, OAK_PLAYER)
OAK_ENTITY_LIBRG_GETTER(vehicle, OAK_VEHICLE)
OAK_ENTITY_LIBRG_GETTER(door, OAK_DOOR)

#undef OAK_ENTITY_LIBRG_GETTER

/* VEHICLES */

oak_player oak_vehicle_streamer_get(oak_vehicle);
int oak_vehicle_streamer_set(oak_vehicle, oak_player);
int oak_vehicle_streamer_assign_nearest(oak_vehicle, int exception=-1);
int oak_vehicle_seat_assign(oak_vehicle, oak_player, oak_seat_id);

/* MASTERLIST */

void oak_masterlist_update();

/* WEBSERVER */

void oak_webserver_init();
void oak_webserver_stop();

/* SEMI PUBLIC METHODS */

oak_player oak_player_create(librg_event *);
int oak_player_destroy(librg_event *);

/* GAMEMAP & SCOREBOARD */
int oak_gamemap_update();
int oak_scoreboard_update();

/* PEER ACCESS CONTROL */

void oak_access_bans_store();
void oak_access_wh_store();
void oak_access_bans_load();
void oak_access_wh_load();
void oak_access_bans_add(IDBind hwid);
void oak_access_bans_remove(u64 hwid);
void oak_access_wh_add(IDBind hwid);
void oak_access_wh_remove(u64 hwid);
void oak_access_wh_state_set(b32 state);
int  oak_access_wh_state_get();
int  oak_access_bans_get(u64 hwid);
int  oak_access_wh_get(u64 hwid);

/* HTTP ENDPOINTS */

int oak_endp_payload_info(char *buf);

/* CONSOLE */

void oak_console_init();
void oak_console_input_handler_init();
void oak_console_input_handler_destroy();
void oak_console_draw(const char *format, ...);
void oak_console_printf(const char *format, ...);
char oak_console_update_loader();
void oak_console_block_input(int);
void oak_console_console_update_stats();

/* SIGNAL HANDLER */

void oak_sighandler_register();
void oak_sighandler_unregister();

/* CLI OPTIONS */

void oak_cli_init(int argc, char **argv);
void oak_cli_replace();
void oak_cli_free();

#endif // OAK_PRIVATE_H
