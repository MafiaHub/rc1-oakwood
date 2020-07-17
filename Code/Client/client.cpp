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
* BASS Audio Library
*/
#include "bass.h"

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
* CURL
*/

#include <curl/curl.h>

/*
* MD5 Library
*/
#include <md5/md5.h>

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
#include "Game/filesystem.hpp"
#include "modules.hpp"
#include "Network/base.hpp"
#include "Graphics/base.hpp"
#include "Graphics/elements.hpp"
#include "Game/base.hpp"
#include "stateManager.hpp"

DWORD JumpBackMenu = 0x00594896;
__declspec(naked) void HookMultipleMenus() {
    __asm {
        MOV EAX, 0x0A9
        jmp JumpBackMenu
    }
}

__declspec(naked) void RETN4() {
    __asm retn 0x4
}

auto mod_init_patches() {
    MafiaSDK::C_Game_Patches::PatchDisableLogos();
    MafiaSDK::C_Game_Patches::PatchDisableGameScripting();
    MafiaSDK::C_Game_Patches::PatchCustomPlayerRespawning();
    MafiaSDK::C_Game_Patches::PatchRemovePlayer();
    MafiaSDK::C_Game_Patches::PatchDisableSuspendProcess();
    MafiaSDK::C_Game_Patches::PatchDisableInventory();
    MafiaSDK::C_Game_Patches::PatchDisablePauseMenu();
    MafiaSDK::C_Game_Patches::PatchJumpToGame("tutorial");

    MemoryPatcher::InstallJmpHook(0x005EA7E0, (DWORD)&RETN4);

    // 0004E034A
    // Force update car physics
    BYTE patchCarPhysics[] = "\xE9\xF1\x00\x00\x00\x90";
    MemoryPatcher::PatchAddress(0x0004E034A, patchCarPhysics, sizeof(patchCarPhysics));

    //NOTE(DavoSK): Remove multiple AB selection menu
    MemoryPatcher::InstallJmpHook(0x00594885, (DWORD)&HookMultipleMenus);
}

ZPL_DLL_EXPORT void oakwood_start(const char *localpath, const char *gamepath, const char* ip, int port)
{
    SetConsoleCP(CP_UTF8);
    semver_parse(OAK_VERSION, &OAK_VERSION_SEMVER);

    GlobalConfig.localpath = localpath;
    GlobalConfig.gamepath = gamepath;

    mod_init_game();
    mod_init_patches();
    mod_init_fs();

    modules::audiostream::init();
}
