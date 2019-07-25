/* 
* OAKWOOD MULTIPLAYER - SERVER
* (C) 2019 Oakwood Team. All Rights Reserved.
*/

#define ZPL_IMPLEMENTATION
#include "librg/zpl.h"

#include "version.hpp"
#include "multiplayer.hpp"
#define OAKWOOD_SERVER 1
#define OAKGEN_NATIVE()
#define OAKGEN_FORWARD()

/* 
* Networking library
*/

//#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#include "librg/librg.h"

/*
* STL includes
*/

#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <ostream>
#include <vector>
#include <clocale>
#include <vector>
#include <thread>

/* 
* STL-powered includes
*/

#include "librg/librg_ext.h"

/*
* HTTP library
*/

#define HTTP_IMPLEMENTATION
#include "http/http.h"
#include "http/mongoose.h"

/*
* Console I/O and signal handling
*/

#include "console.hpp"
#include "signal_handling.hpp"

/* 
* Shared
*/

#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"

/* 
* Globals
*/

#include "server.hpp"

/* 
* Switches
*/

#define OAK_CHAT_DISABLE_STYLING

/* Useful when debugging crashes. */
// #define OAK_DISABLE_SIGNAL_HANDLING

/* 
* Core
*/

#include "peer_control.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "opts.hpp"
#include "mode.hpp"
#include "modules.hpp"
#include "Network/base.hpp"
#include "natives.hpp"

/* 
* Workers
*/

#include "Workers/masterlist.hpp"
#include "Workers/webserver.hpp"
#include "Workers/misc.hpp"

/* 
* Entry point
*/

const char *banner_text = R"foo(
 .88888.   .d888888  dP     dP dP   dP   dP  .88888.   .88888.  888888ba     8888ba.88ba   888888ba  
d8'   `8b d8'    88  88   .d8' 88   88   88 d8'   `8b d8'   `8b 88    `8b    88  `8b  `8b  88    `8b 
88     88 88aaaaa88a 88aaa8P'  88  .8P  .8P 88     88 88     88 88     88    88   88   88 a88aaaa8P' 
88     88 88     88  88   `8b. 88  d8'  d8' 88     88 88     88 88     88    88   88   88  88        
Y8.   .8P 88     88  88     88 88.d8P8.d8P  Y8.   .8P Y8.   .8P 88    .8P    88   88   88  88        
 `8888P'  88     88  dP     dP 8888' Y88'    `8888P'   `8888P'  8888888P     dP   dP   dP  dP        
                                                                                                     
)foo";

int main(int argc, char **argv) {
    console::init();
    console::printf("================================\n");
    console::printf(banner_text);
    console::printf("Build version: %s (%x)\n", OAK_BUILD_VERSION_STR, OAK_BUILD_VERSION);
    console::printf("Build channel: %s\n", oak_build_channel[OAK_BUILD_CHANNEL]);
    console::printf("Build time: %s %s\n", OAK_BUILD_DATE, OAK_BUILD_TIME);
    console::printf("================================\n"); 
    opts::init(argc, argv);

#ifndef OAK_DISABLE_SIGNAL_HANDLING
    register_console_events();
#endif

    config::init();
    opts::replace();
    network::init();
    webserver::init();
    gamemode::init();

    console::init_input_handler();

    while (true) {
        console::console_data.input_block.store(true);
        network::update();
        misc::vehicles_streamer_update(); 
        misc::console_update_stats();
        misc::scoreboard_update();
        misc::gamemap_update();
        masterlist::update();
        console::console_data.input_block.store(false);
        zpl_sleep_ms(1);
    }

    return 0;
}

void shutdown_server() {
    mod_log("Server is shutting down...");
    gamemode::free_dll();
    webserver::stop();
    network::shutdown();
    console::kill_input_handler();

#ifndef OAK_DISABLE_SIGNAL_HANDLING
    unregister_console_events();
#endif
    
    opts::free();
    zpl_exit(0);
}

void execute_command(std::string msg) {
    printf("Executing server command: %s\n", msg.c_str());

    if (gm.on_server_command)
        gm.on_server_command(msg);
}
