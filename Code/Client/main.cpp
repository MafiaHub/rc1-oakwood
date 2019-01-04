/* 
* Librg 
*/
#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#include "librg/librg.h"
#include "Network/interpolation.hpp"

librg_ctx network_context = { 0 };

/*
* Mafia SDK
*/
#define MAFIA_SDK_IMPLEMENTATION
#include "mafia-sdk/MafiaSDK/MafiaSDK.h"

/*
* HTTP lib for server browser 
*/
#define HTTP_IMPLEMENTATION
#include "http/http.h" 

/*
* Detours
*/
#include "detours/detours.h"

/*
* VoIP
*/
#include <opus/include/opus.h>
#include <bass/bass.h>

/*
* Temp json parser
*/
#include "nlohmann/json.hpp"
using json = nlohmann::json;

/*
* STD Includes
*/
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <clocale>
#include <queue>
#include <mutex>
#include <atomic>

/*
* DX & DInput
*/
#include <d3d9.h>
#include <d3d9/include/d3dx9.h>
#include <dinput.h>
IDirect3DDevice9* global_device = nullptr;

/*
* Shared
*/
#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"
#include "utils.hpp"
#include "librg/librg_ext.h"

f64 last_delta  = 0.0f;
f64 last_update = 0.0f;
bool hit_hook_skip = true;

struct _GlobalConfig {
    std::string server_address;
    std::string username;
    float view_distance;
    std::string localpath;
    std::string gamepath;
    // NOTE(DavoSK): Better result with high delay values for vehicle interpolation
    // Possible cuz of overinterpolation protection
    // TODO(DavoSK): Find real good value
    float interp_time_vehicle = 4.0f;
    float interp_time_player = 0.08f;
} GlobalConfig;

/*
* Mod includes 
*/
#include "config.hpp"
#include "Game/VoIP/main.hpp"
#include "Input/input.hpp"
#include "Game/Factory/defs.hpp"
#include "Game/Commands/defs.hpp"
#include "Graphics/elements_pre.hpp"
#include "Network/base.hpp"
#include "Graphics/base.hpp"
#include "Graphics/elements_post.hpp"
#include "Game/base.hpp"

ZPL_DLL_EXPORT void oakwood_start(const char *localpath, const char *gamepath) {
    GlobalConfig.localpath = localpath;
    GlobalConfig.gamepath = gamepath;

    mod_pre_init_game();
    mod_init_patches();
}

#include "stub.hpp"
