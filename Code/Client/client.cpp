/* 
* OAKWOOD MULTIPLAYER - CLIENT
* (C) 2019 Oakwood Team. All Rights Reserved.
*/

#include "version.hpp"

/* 
* Networking library
*/

#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#include "librg/librg.h"
#include "Network/interpolation.hpp"

/*
* STL Includes
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
#include <psapi.h>

/*
* STL-powered includes
*/

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
IDirect3DDevice9* global_device = nullptr;

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
* Core
*/

#include "config.hpp"
#include "utils.hpp"
#include "Input/input.hpp"
#include "modules.hpp"
#include "Network/base.hpp"
#include "Graphics/base.hpp"
#include "Graphics/elements.hpp"
#include "Game/base.hpp"

ZPL_DLL_EXPORT void oakwood_start(const char *localpath, const char *gamepath) {
    GlobalConfig.localpath = localpath;
    GlobalConfig.gamepath = gamepath;

    mod_pre_init_game();
    mod_init_patches();
}

#include "stub.hpp"
