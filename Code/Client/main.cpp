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

/*
* Dear ImGUI
*/
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "Graphics/imgui/imgui.h"
#include "Graphics/imgui/imgui_impl_dx9.h"
#include "Graphics/imgui/imgui_impl_win32.h"

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

auto mod_init() {
    mod_pre_init_game();
    init_config();
    mod_init_game();
    mod_init_networking();
}

#include "stub.hpp"