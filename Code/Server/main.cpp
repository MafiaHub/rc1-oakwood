#define LIBRG_IMPLEMENTATION
#define LIBRG_NO_DEPRECATIONS
#define OAKWOOD_SERVER 1
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

#ifdef ZPL_SYSTEM_UNIX
#include <signal.h>
#endif

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
#include "mode.hpp"
#include "modules.hpp"
#include "Network/base.hpp"

#define OAKGEN_NATIVE()
#define OAKGEN_FORWARD()

#include "Workers/masterlist.hpp"
#include "Workers/webserver.hpp"
#include "Workers/misc.hpp"
#include "natives.hpp"

const char *banner_text = R"foo(
 .88888.   .d888888  dP     dP dP   dP   dP  .88888.   .88888.  888888ba     8888ba.88ba   888888ba  
d8'   `8b d8'    88  88   .d8' 88   88   88 d8'   `8b d8'   `8b 88    `8b    88  `8b  `8b  88    `8b 
88     88 88aaaaa88a 88aaa8P'  88  .8P  .8P 88     88 88     88 88     88    88   88   88 a88aaaa8P' 
88     88 88     88  88   `8b. 88  d8'  d8' 88     88 88     88 88     88    88   88   88  88        
Y8.   .8P 88     88  88     88 88.d8P8.d8P  Y8.   .8P Y8.   .8P 88    .8P    88   88   88  88        
 `8888P'  88     88  dP     dP 8888' Y88'    `8888P'   `8888P'  8888888P     dP   dP   dP  dP        
                                                                                                     
)foo";


int main() {

    console_init();
    console_printf("================================\n");
    console_printf(banner_text);

#ifndef OAK_DISABLE_SIGNAL_HANDLING
    register_console_events();
#endif

    config::init();
    gamemode::init();
    network::init();
    
    gamemode::load_dll(GlobalConfig.gamemode.c_str());

    webserver::start();

    while (true) {
        network::update();
        misc::vehicles_streamer_update(); 
        masterlist::update();
        misc::console_update_stats();
        zpl_sleep_ms(1);
    }
    return 0;
}

void shutdown_server() {
    mod_log("Server is shutting down...");
    unregister_console_events();
    gamemode::free_dll();
    webserver::stop();
    network::shutdown();
    zpl_exit(0);
}
