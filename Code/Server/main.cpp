#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#define OAKWOOD_SERVER 1
#define OAKGEN_NATIVE()
#define OAKGEN_FORWARD()
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
#include <vector>
#include <thread>

#include "librg/librg_ext.h"

/*
* HTTP lib
*/
#define HTTP_IMPLEMENTATION
#include "http/http.h"
#include "http/mongoose.h"

/*
* Console stuff
*/
#include "console.hpp"
#include "signal_handling.hpp"

/* 
* Shared
*/
#include "version.hpp"
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

#define OAK_CHAT_DISABLE_STYLING

/* Useful when debugging crashes. */
// #define OAK_DISABLE_SIGNAL_HANDLING

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
    console::printf("Build version: %d\n", OAK_BUILD_VERSION);
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

    while (true) {
        network::update();
        misc::vehicles_streamer_update(); 
        misc::console_update_stats();
        masterlist::update();
        zpl_yield();
    }

    return 0;
}

void shutdown_server() {
    mod_log("Server is shutting down...");
    gamemode::free_dll();
    webserver::stop();
    network::shutdown();

#ifndef OAK_DISABLE_SIGNAL_HANDLING
    unregister_console_events();
#endif
    
    opts::free();
    zpl_exit(0);
}
