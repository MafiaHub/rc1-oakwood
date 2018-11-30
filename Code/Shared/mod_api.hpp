#pragma once

#include "librg.h"

/*
* STD Includes
*/
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <fstream>
#include <ostream>
#include <vector>
#include <clocale>

/*
* Includes for mod DLL
*/

#include "structs.hpp"
#include "utils.hpp"

#ifndef OAK_INTERNAL

#include "librg_ext.h"

#include "messages.hpp"
#include "helpers.hpp"
#endif

#include "mod_api_types.generated.hpp"


/*
* Mod API
*/

struct oak_api {
    std::string name;
    std::string author;
    std::string version;

    struct {
        #include "mod_api_vtable.generated.hpp"
    } vtable;

    std::function<void(librg_event* evnt, librg_entity* entity, mafia_player* ped)> on_player_connected;
    std::function<void(librg_event* evnt, librg_entity* entity)> on_player_disconnected;
    std::function<void(librg_entity* entity, mafia_player* ped)> on_player_died;
    std::function<bool(librg_entity* entity, std::string msg)> on_player_chat;
    std::function<void(librg_entity* attacker, librg_entity* victim, float damage)> on_player_hit;
    std::function<void(librg_entity* vehicle)> on_vehicle_destroyed;
    std::function<void()> on_server_tick;

    inline ~oak_api() {
        zpl_zero_item(this);
    }
};

#define OAK_MOD_ENTRYPOINT_NAME oak_mod_main
#define OAK_MOD_ENTRYPOINT_PROC(name) void name(oak_api *mod)
#define OAK_MOD_MAIN ZPL_DLL_EXPORT OAK_MOD_ENTRYPOINT_PROC(OAK_MOD_ENTRYPOINT_NAME)
typedef OAK_MOD_ENTRYPOINT_PROC(oak_mod_entrypoint_ptr);

#define OAK_MOD_SHUTDOWN_NAME oak_mod_shutdown
#define OAK_MOD_SHUTDOWN_PROC(name) void name(oak_api *mod)
#define OAK_MOD_SHUTDOWN ZPL_DLL_EXPORT OAK_MOD_SHUTDOWN_PROC(OAK_MOD_SHUTDOWN_NAME)
typedef OAK_MOD_SHUTDOWN_PROC(oak_mod_shutdown_ptr);
