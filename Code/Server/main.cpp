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
zpl_event_pool gamemode_events = { 0 };

#include "mode.hpp"
#include "Network/base.hpp"

// TODO replace by plugin loader
#include "freeride.hpp"

auto main() -> int {

	std::setlocale(LC_ALL, "C");
	SetConsoleOutputCP(CP_UTF8);

	init_event_handler();
	freeride_events_add();

	mod_log("Initializing server ...");
	mod_init_networking();
	
	librg_address_t addr = { 27010 };
	librg_network_start(&network_context, addr);
	mod_log("Server started");

	bool running = true;
	while (running) {
		librg_tick(&network_context);
		zpl_sleep_ms(2);
	}

	librg_network_stop(&network_context);
	librg_free(&network_context);
	zpl_event_destroy(&gamemode_events);
	return 0;
}