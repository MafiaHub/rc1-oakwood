/*
* OAKWOOD MULTIPLAYER - CLIENT
* (C) 2019 Oakwood Team. All Rights Reserved.
*/

// #define OAK_FEATURE_VEHICLE_CACHE

#define ZPL_IMPLEMENTATION
#include "librg/zpl.h"

#define LIB_INTER_IMPLEMENTATION
#include "librg/lib_inter.h"

#include "version.h"
#include "multiplayer.hpp"

/*
* Networking library
*/

//#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#define LIBRG_FEATURE_ENTITY_VISIBILITY
#define LIBRG_DISABLE_FEATURE_OCTREE_CULLER
#include "librg/librg.h"

/*
* STL Includes
*/

#include <iostream>
#include <regex>
#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <clocale>
#include <queue>
#include <mutex>
#include <atomic>
#include <psapi.h>

/*
* STL-powered includes
*/

#include "librg/librg_ext.h"

/*
* Stack walker
*/

#include "Game/CrashHandler/StackWalker/StackWalker.h"

/*
* Dear ImGUI
*/

#include "Graphics/imgui/imgui.h"
#include "Graphics/imgui/imgui_impl_dx9.h"
#include "Graphics/imgui/imgui_impl_win32.h"

/*
* Mafia SDK
*/

#define MAFIA_SDK_IMPLEMENTATION
#include "mafia-sdk/MafiaSDK/MafiaSDK.h"

/*
* HTTP library
*/

#define HTTP_IMPLEMENTATION
#include "http/http.h"

/*
* Detours
*/

#include "detours/detours.h"

/*
* Direct3D and DirectInput
*/

#include <d3d9.h>
#include <d3d9/include/d3dx9.h>
#include <dinput.h>

/*
* Shared
*/

#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"

/*
* Globals
*/

#include "client.hpp"

/*
* Switches
*/

#if OAK_BUILD_CHANNEL == 1
#define OAKWOOD_DEBUG
#endif

/*
* Core
*/

#include "hwid.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "profile.hpp"
#include "serverInfo.hpp"
#include "Input/input.hpp"
#include "modules.hpp"
#include "Network/base.hpp"
#include "Graphics/base.hpp"
#include "Graphics/elements.hpp"
#include "Game/base.hpp"
#include "stateManager.hpp"

ZPL_DLL_EXPORT void oakwood_start(const char *localpath, const char *gamepath, const char* ip, int port)
{
    semver_parse(OAK_VERSION, &OAK_VERSION_SEMVER);

    GlobalConfig.localpath = localpath;
    GlobalConfig.gamepath = gamepath;

    mod_init_game();
    mod_init_patches();
}
