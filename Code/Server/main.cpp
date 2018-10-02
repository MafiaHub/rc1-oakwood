#define LIBRG_IMPLEMENTATION
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
* Shared
*/
#include "helpers.hpp"
#include "structs.hpp"
#include "messages.hpp"

librg_ctx_t network_context = { 0 };

#include "mode.hpp"
#include "commands.hpp"
#include "Network/base.hpp"

#include "natives.hpp"

auto main() -> int {

	std::setlocale(LC_ALL, "C");
	SetConsoleOutputCP(CP_UTF8);

	init_api();

	mod_log("Initializing server ...");
	mod_init_networking();
	
	librg_address_t addr = { 27010 };
	librg_network_start(&network_context, addr);
	mod_log("Server started");

	mod_log("Loading gamemode...");

	// TODO use config system to load DLL
	load_dll("SampleMod.dll");

	bool running = true;
	while (running) {
		librg_tick(&network_context);

        if (gm.on_server_tick)
            gm.on_server_tick();

		zpl_sleep_ms(2);
	}

	librg_network_stop(&network_context);
	librg_free(&network_context);
	free_dll();
	return 0;
}