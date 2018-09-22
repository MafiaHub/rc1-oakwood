/* 
* Librg 
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "librg/librg.h"

/*
* Mafia SDK
*/
#define MAFIA_SDK_IMPLEMENTATION
#include "mafia-sdk/MafiaSDK/MafiaSDK.h"

/*
* Detours
*/
#include "detours/detours.h"

/*
* JSON
*/
#include "json/json.hpp"

/*
* STD Includes
*/
#include <iostream>
#include <string>
#include <functional>
#include <fstream>

/*
* Shared
*/
#include "helpers.hpp"
#include "interpolation.h"
#include "structs.hpp"
#include "messages.hpp"

/*Bones*/
auto mod_init()			-> void;
auto mod_init_patches() -> void;
auto mod_bind_events()  -> void;

f32 last_delta  = 0.0f;
f32 last_update = 0.0f;

librg_ctx_t ctx = { 0 };
bool hit_hook_skip = true;

/*
* Mod includes 
*/
#include "config.hpp"
#include "local_player.hpp"
#include "player.hpp"
#include "weapon_drop.hpp"
#include "mp.hpp"

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		mod_init();
	} break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

auto mod_init() -> void {

	//alloc console
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//get config
	config_get();

	mod_log("Initialization of patches");
	mod_init_patches();

	mod_log("Initialization of librg");
	mod_librg_init_stuff();

	mod_bind_events();
}

void librg_entites_tick(librg_entity_t* ent) {
	*(int*)((DWORD)ent->user_data + 0x4A4) = 50;
}

int menu_skip = 0;
auto mod_bind_events() -> void {

	MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {

		if (!menu_skip) {
			menu_skip = 1;
			return;
		}

		if (menu_skip == 1) {

			// disable traffic
			MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);

			// connecting camera look at the LockAt more cuz it's weird
 			auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
			Vector3D cam_pos = { -1.658540964f, 7.694108963f, 38.52610779f };
			Vector3D cam_rot = {0.5f, 0.5f, 0.5f };
			cam->LockAt(cam_pos, cam_rot);
			
			// welcome msg
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText("Welcome to Mafia DM Alpha 0.1", 0xFF0000);
			std::string connect_string = "Connecting to " + GlobalConfig.server_address + " ...";
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText(connect_string.c_str(), 0xFFFFFF);

			mod_librg_connect();
		}
		menu_skip++;
	});

	local_player_init();
	drop_init();

	MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {
		
		librg_tick(&ctx);
		for (u32 i = 0; i < ctx.max_entities; i++) {
			
			librg_entity_t *entity = librg_entity_fetch(&ctx, i);
			if (!entity || entity->id == local_player.entity.id) continue;

			switch (entity->type) {
				case TYPE_WEAPONDROP: {
					auto weapon_drop = (mafia_weapon_drop*)entity->user_data;
					if (weapon_drop && weapon_drop->weapon_drop_actor)
						drop_game_tick(weapon_drop);

				} break;

				case TYPE_PLAYER: {
					auto player = (mafia_player*)entity->user_data;
					if (player && player->ped) {
						player_game_tick(player);
					}
				} break;
			}
		}
	});	
}

auto mod_init_patches() -> void {

	MafiaSDK::C_Game_Patches::PatchDisableLogos();
	MafiaSDK::C_Game_Patches::PatchDisablePleaseWait();
	MafiaSDK::C_Game_Patches::PatchDisableSuspendProcess();
	MafiaSDK::C_Game_Patches::PatchDisableGameScripting();
	MafiaSDK::C_Game_Patches::PatchCustomPlayerRespawning();
	MafiaSDK::C_Game_Patches::PatchRemovePlayer();
	MafiaSDK::C_Game_Patches::PatchJumpToGame("dm");
}
