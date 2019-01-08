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
#include "commands.hpp"
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

void register_console_events();
void unregister_console_events();

auto main() -> int {

    #ifdef _WIN32  
        std::setlocale(LC_ALL, "C");
        SetConsoleOutputCP(CP_UTF8);
    #endif
    
    zpl_printf("================================\n");  
    zpl_printf("%s", banner_text);

#ifndef OAK_DISABLE_SIGNAL_HANDLING
    register_console_events();
#endif

    init_config();
    init_api();

    mod_log("Initializing librg service...");
    mod_init_networking();
    
    librg_address addr = { (i32)GlobalConfig.port };
    librg_network_start(&network_context, addr);
    GlobalConfig.players = 0;

    mod_log("Loading gamemode...");

    load_dll(GlobalConfig.gamemode.c_str());

    mod_log("Initializing webserver at :"+std::to_string(GlobalConfig.port) + "...");
    webserver_start();

    while (true) {
        librg_tick(&network_context);

        if (gm.on_server_tick)
            gm.on_server_tick();

        masterlist_update();
        debug_bandwidth();
        vehicles_streamer_update();
        zpl_sleep_ms(1);
    }
    return 0;
}

void shutdown_server() {
    mod_log("Server is shutting down...");
    unregister_console_events();

    mod_log("Shutting down the gamemode...");
    free_dll();
    mod_log("Stopping the webserver...");
    webserver_stop();
    mod_log("Stopping librg service...");
    librg_network_stop(&network_context);
    librg_free(&network_context);
    zpl_exit(0);
}

#include <signal.h>
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    #ifdef ZPL_SYSTEM_WINDOWS

    BOOL WINAPI win32_control_handler(DWORD control_type)
    {
        switch (control_type)
        {
        case CTRL_C_EVENT:
        case DBG_CONTROL_C:
            mod_log("Ctrl-C pressed, stopping the server...");
            shutdown_server();
            return 0;
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            shutdown_server();
            return 1;
        }

        return 0;
    }
    #else //POSIX complaint
    void posix_signal_handler(int sig) {
        shutdown_server();
    }
    #endif
#endif

void register_console_events() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    mod_log("Installing signal handlers...");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(win32_control_handler, 1)) {
            mod_log("Could not set up signal handler!");
        }
    }
    #else // POSIX compliant
    signal(SIGINT, &posix_signal_handler);
    signal(SIGTERM, &posix_signal_handler);
    #endif
#endif
}

void unregister_console_events() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    mod_log("Installing signal handlers...");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(win32_control_handler, 0)) {
            mod_log("Could not uninstall signal handler!");
        }
    }
    #else // POSIX compliant
    #endif
#endif
}
