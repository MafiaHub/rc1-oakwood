/* 
* Librg 
*/
#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "librg/librg.h"

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
* STD Includes
*/
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <vector>

/*
* DX & DInput
*/
#include <d3d9.h>
#include <d3dx9.h>
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
#include "Network/interpolation.hpp"
#include "helpers.hpp"
#include "structs.hpp"
#include "messages.hpp"
#include "utils.hpp"

/*Bones*/
auto mod_init()			-> void;
auto mod_init_patches() -> void;
auto mod_bind_events()  -> void;
auto mod_hook_dx()		-> void;

f32 last_delta  = 0.0f;
f32 last_update = 0.0f;

librg_ctx_t network_context = { 0 };
bool hit_hook_skip = true;

/*
* Mod includes 
*/
#include "Network/base.hpp"
#include "Input/input.hpp"
#include "Graphics/base.hpp"
#include "Game/base.hpp"


auto mod_init() -> void {
	mod_pre_init_game();
	alloc_console();
	config_get();
	mod_init_game();
	mod_init_networking();
}

#include "stub.hpp"