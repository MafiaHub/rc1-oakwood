#pragma once

/*
* Includes for mod DLL
*/
#ifndef OAK_INTERNAL

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
#include <clocale>

#include "librg/librg_ext.h"

#include "helpers.hpp"
#include "messages.hpp"
#include "structs.hpp"
#include "utils.hpp"

#endif

struct mode_event {
    librg_entity_t *player_ent;
	librg_event_t *evnt;
	mafia_player *player;
} mode_data;

enum mod_mode_events {
    MODE_ON_PLAYER_CONNECTED,
    MODE_ON_PLAYER_DISCONNECTED,
    MODE_ON_PLAYER_DIED,
};

/*
* Prototypes
*/

#define OAK_REGISTER_EVENT(name) void name(u64 type, zpl_event_cb cb)
typedef OAK_REGISTER_EVENT(oak_register_event_ptr);

#define OAK_PRINT_TEST(name) void name()
typedef OAK_PRINT_TEST(oak_print_test_ptr);

#define OAK_BROADCAST_MSG(name) void name(const char* text)
typedef OAK_BROADCAST_MSG(oak_broadcast_msg_ptr);

#define OAK_BROADCAST_MSG_COLOR(name) void name(const char* text, u32 color)
typedef OAK_BROADCAST_MSG_COLOR(oak_broadcast_msg_color_ptr);

//

#define OAK_PLAYER_INVENTORY_ADD(name) void name(librg_entity_t *entity, inventory_item *item)
typedef OAK_PLAYER_INVENTORY_ADD(oak_player_inventory_add_ptr);

#define OAK_PLAYER_SPAWN(name) void name(librg_entity_t *entity)
typedef OAK_PLAYER_SPAWN(oak_player_spawn_ptr);

#define OAK_PLAYER_RESPAWN(name) void name(librg_entity_t *entity)
typedef OAK_PLAYER_RESPAWN(oak_player_respawn_ptr);

//

#define OAK_DROP_SPAWN(name) librg_entity_t* name(zpl_vec3 position, char *model, inventory_item item)
typedef OAK_DROP_SPAWN(oak_drop_spawn_ptr);


/*
* Mod API
*/

struct oak_api {
    std::string name;
    std::string author;
    std::string version;

    struct {
        // General
        oak_register_event_ptr *register_event;
        oak_print_test_ptr *print_test;
        oak_broadcast_msg_ptr *broadcast_msg;
        oak_broadcast_msg_color_ptr *broadcast_msg_color;

        // Player
        oak_player_spawn_ptr *player_spawn;
        oak_player_respawn_ptr *player_respawn;
        oak_player_inventory_add_ptr *player_inventory_add;

        // Weapon drop
        oak_drop_spawn_ptr *drop_spawn;
    } vtable;
};

#define OAK_MOD_ENTRYPOINT_NAME oak_mod_main
#define OAK_MOD_ENTRYPOINT(name) void name(oak_api *mod)
#define OAK_MOD_MAIN extern "C" ZPL_DLL_EXPORT OAK_MOD_ENTRYPOINT(OAK_MOD_ENTRYPOINT_NAME)
typedef OAK_MOD_ENTRYPOINT(oak_mod_entrypoint_ptr);
