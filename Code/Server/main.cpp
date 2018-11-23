#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
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

/*
* HTTP lib
*/
#define HTTP_IMPLEMENTATION
#include "http/http.h"
#include "http/mongoose.h"

/* 
* Shared
*/
#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"

struct _GlobalConfig {
	std::string name;
	std::string host;
	i64 port;
	i64 players;
	i64 max_players;
	std::string gamemode;
	b32 visible;
} GlobalConfig;

librg_ctx network_context = { 0 };

#include "config.hpp"
#include "mode.hpp"
#include "commands.hpp"
#include "Network/base.hpp"

#define OAKGEN_NATIVE()
#define OAKGEN_FORWARD()

#include "Workers/masterlist.hpp"
#include "Workers/webserver.hpp"
#include "Workers/misc.hpp"
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
	GlobalConfig.players = 0;
	mod_log("Server started");
	mod_log("Loading gamemode...");

	load_dll(GlobalConfig.gamemode.c_str());

	webserver_start();

	bool running = true;
	while (running) {
		librg_tick(&network_context);

		if (gm.on_server_tick)
			gm.on_server_tick();

		masterlist_update();
		vehicles_streamer_update();

		zpl_sleep_ms(1);
	}

	webserver_stop();
	librg_network_stop(&network_context);
	librg_free(&network_context);
	free_dll();
	return 0;
}