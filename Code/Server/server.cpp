/*
* OAKWOOD MULTIPLAYER - SERVER
* (C) 2019 Oakwood Team. All Rights Reserved.
*/

#define ZPL_IMPLEMENTATION
#include "librg/zpl.h"

#include "version.hpp"
#include "multiplayer.hpp"

#define OAKWOOD_SERVER 1

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
* Shared
*/

#include "structs.hpp"
#include "messages.hpp"
#include "helpers.hpp"

/*
* Core
*/

#include "oak_server.h"
#include "oak_private.h"

#include "core/config.h"
#include "core/signal_handling.h"
#include "core/console.h"
#include "core/cli_opts.h"
#include "core/entities.h"
#include "core/access.h"
#include "core/logger.h"
#include "core/network.h"
#include "core/bridge.h"
#include "core/bridge.generated.h"
#include "core/endpoints.h"
#include "core/webserver.h"

#include "core/tasks/masterlist.h"
#include "core/tasks/gamemap.h"
#include "core/tasks/scoreboard.h"
#include "core/tasks/vehicles.h"

#include "utils.hpp"

#include "api/chat.h"
#include "api/camera.h"
#include "api/hud.h"
#include "api/player.h"
#include "api/vehicle.h"
#include "api/vehicle_player.h"
#include "api/door.h"

#include "events/chat.h"
#include "events/door.h"
#include "events/player.h"
#include "events/vehicle.h"
#include "events/vehicle_player.h"
#include "events/weapons.h"


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

int main(int argc, char **argv)
{
    oak_log_init();

    oak_console_init();
    oak_console_printf("================================\n");
    oak_console_printf(banner_text);
    oak_console_printf("Build version: %s (%x)\n", OAK_BUILD_VERSION_STR, OAK_BUILD_VERSION);
    oak_console_printf("Build channel: %s\n", oak_build_channel[OAK_BUILD_CHANNEL]);
    oak_console_printf("Build time: %s %s\n", OAK_BUILD_DATE, OAK_BUILD_TIME);
    oak_console_printf("================================\n");
    oak_cli_init(argc, argv);

    oak_sighandler_register();

    oak_config_init();
    oak_cli_replace();

    oak_webserver_init();
    oak_bridge_init();
    oak_network_init();
    oak_entities_init();

    oak_console_input_handler_init();

    while (true)
    {
        oak_console_block_input(1);
        oak_network_tick();
        oak_bridge_tick();
        oak_vehicles_update();
        oak_console_console_update_stats();
        oak_scoreboard_update();
        oak_gamemap_update();
        oak_masterlist_update();
        oak_console_block_input(0);
        zpl_sleep_ms(1);
    }

    return 0;
}

void shutdown_server()
{
    oak_log("[info] server is shutting down...\n");

    oak_webserver_stop();
    oak_console_input_handler_destroy();
    oak_bridge_free();
    oak_network_free();
    oak_sighandler_unregister();

    oak_cli_free();
    zpl_exit(0);
}
