#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
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

/* 
* Shared
*/
#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"

struct _GlobalConfig {
	i64 port;
	i64 max_players;
	std::string gamemode;
} GlobalConfig;

librg_ctx network_context = { 0 };

#include "config.hpp"
#include "mode.hpp"
#include "commands.hpp"
#include "Network/base.hpp"

#define OAKGEN_NATIVE()
#define OAKGEN_FORWARD()

#include "natives.hpp"

const char *jebe = R"foo(
 .88888.   .d888888  dP     dP dP   dP   dP  .88888.   .88888.  888888ba     8888ba.88ba   888888ba  
d8'   `8b d8'    88  88   .d8' 88   88   88 d8'   `8b d8'   `8b 88    `8b    88  `8b  `8b  88    `8b 
88     88 88aaaaa88a 88aaa8P'  88  .8P  .8P 88     88 88     88 88     88    88   88   88 a88aaaa8P' 
88     88 88     88  88   `8b. 88  d8'  d8' 88     88 88     88 88     88    88   88   88  88        
Y8.   .8P 88     88  88     88 88.d8P8.d8P  Y8.   .8P Y8.   .8P 88    .8P    88   88   88  88        
 `8888P'  88     88  dP     dP 8888' Y88'    `8888P'   `8888P'  8888888P     dP   dP   dP  dP        
                                                                                                     
)foo";

auto main() -> int {

	#ifdef _WIN32  
		std::setlocale(LC_ALL, "C");
		SetConsoleOutputCP(CP_UTF8);
	#endif
	
	zpl_printf("%s", jebe);

	init_config();
	init_api();

	mod_log("Initializing server ...");
	mod_init_networking();
	
	librg_address addr = { (i32)GlobalConfig.port };
	librg_network_start(&network_context, addr);
	mod_log("Server started");
	mod_log("Loading gamemode...");

	load_dll(GlobalConfig.gamemode.c_str());

	f64 last_streamers_selection = 0.0f;

	bool running = true;
	while (running) {
		librg_tick(&network_context);

		if (gm.on_server_tick)
			gm.on_server_tick();

		// if noone is in vehicle 
		// set streamer to closest player
		if (zpl_time_now() - last_streamers_selection > 2.0f) {
			librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | TYPE_VEHICLE), [](librg_ctx *ctx, librg_entity *entity) {
				if (entity->user_data && entity->type & TYPE_VEHICLE) {
					auto vehicle = (mafia_vehicle*)entity->user_data;
					
					if (vehicle->seats[0] == -1) {
						auto streamer = mod_get_nearest_player(&network_context, entity->position);
						if (streamer != nullptr) {
							librg_entity_control_set(&network_context, entity->id, streamer->client_peer);
						}
						else {
							librg_entity_control_remove(&network_context, entity->id);
						}
					}
				}
			});

			last_streamers_selection = zpl_time_now();
		}

		zpl_sleep_ms(1);
	}

	librg_network_stop(&network_context);
	librg_free(&network_context);
	free_dll();
	return 0;
}