/*
* OAKWOOD MULTIPLAYER - SERVER
* (C) 2019 Oakwood Team. All Rights Reserved.
*/
#define OAKWOOD_SERVER 1

#define ZPL_IMPLEMENTATION
#include "librg/zpl.h"

#include "version.h"
#include "multiplayer.hpp"

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
#include <iomanip>
#include <sstream>

#include <filesystem>
#include <md5.h>

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
* AngelScript & Addons
*/
#define AS_USE_STLNAMES 1
#define AS_USE_ACCESSORS 1

#include "angelscript.h"
#include "angelscript/addon/scriptstdstring/scriptstdstring.h"
#include "angelscript/addon/scriptbuilder/scriptbuilder.h"
#include "angelscript/addon/scriptarray/scriptarray.h"
#include "angelscript/addon/scriptany/scriptany.h"
#include "angelscript/addon/scriptmath/scriptmath.h"
#include "angelscript/addon/datetime/datetime.h"
#include "angelscript/addon/scriptfile/scriptfile.h"
#include "angelscript/addon/scriptfile/scriptfilesystem.h"

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
#include "core/consolecmds.h"
#include "core/console.h"
#include "core/cli_opts.h"
#include "core/entities.h"
#include "core/access.h"
#include "core/logger.h"
#include "core/network.h"
#include "core/bridge.h"
#include "core/bridge.generated.h"
#include "core/endpoints.h"
#include "core/files.h"
#include "core/webserver.h"
#include "core/angelapi.h"
#include "core/crashhandler.h"

#include "core/tasks/killbox.h"
#include "core/tasks/masterlist.h"
#include "core/tasks/gamemap.h"
#include "core/tasks/scoreboard.h"
#include "core/tasks/vehicles.h"

#include "utils.hpp"

#include "api/misc.h"
#include "api/chat.h"
#include "api/camera.h"
#include "api/hud.h"
#include "api/player.h"
#include "api/vehicle.h"
#include "api/vehicle_player.h"
#include "api/door.h"

#include "events/chat.h"
#include "events/door.h"
#include "events/dialogs.h"
#include "events/player.h"
#include "events/vehicle.h"
#include "events/vehicle_player.h"
#include "events/weapons.h"


void oak_tasks_process();

/*
* Entry point
*/

const char *banner_text = R"foo(
^9 .88888.   .d888888  dP     dP ^FdP   dP   dP  .88888.   .88888.  888888ba     ^B8888ba.88ba   888888ba^R
^9d8'   `8b d8'    88  88   .d8' ^F88   88   88 d8'   `8b d8'   `8b 88    `8b    ^B88  `8b  `8b  88    `8b^R
^988     88 88aaaaa88a 88aaa8P'  ^F88  .8P  .8P 88     88 88     88 88     88    ^B88   88   88 a88aaaa8P'^R
^988     88 88     88  88   `8b. ^F88  d8'  d8' 88     88 88     88 88     88    ^B88   88   88  88^R
^9Y8.   .8P 88     88  88     88 ^F88.d8P8.d8P  Y8.   .8P Y8.   .8P 88    .8P    ^B88   88   88  88^R
^9 `8888P'  88     88  dP     dP ^F8888' Y88'    `8888P'   `8888P'  8888888P     ^BdP   dP   dP  dP^R

)foo";

int main(int argc, char **argv)
{
#if defined(ZPL_SYSTEM_WINDOWS) && !defined(_DEBUG) && !defined(__GNUC__)
    __try {
#endif
        semver_parse(OAK_VERSION, &OAK_VERSION_SEMVER);

        oak_log_init();

        oak_console_init();
        oak_console_printf("^E================================^R\n");
        oak_console_printf(banner_text);
        oak_console_printf("^FBuild version: ^Av%s^R\n", OAK_VERSION);
        oak_console_printf("^FBuild channel: ^A%s^R\n", OAK_BUILD_TYPE);
        oak_console_printf("^FBuild time: ^A%s %s^R\n", OAK_BUILD_DATE, OAK_BUILD_TIME);
        oak_console_printf("^E================================^R\n");
        oak_cli_init(argc, argv);

        oak_sighandler_register();

        oak_config_init();
        oak_cli_replace();

        geo_ip = GeoIP_open("GeoIP.dat", NULL);

        if (!geo_ip)
        {
            oak_log("^F[^9ERROR^F] Cannot load GeoIP database, GeoIP functions are disabled!^R\n");
        }
        else
        {
            oak_log("^F[^5INFO^F] GeoIP database loaded!^R\n");
            GeoIP_set_charset(geo_ip, GEOIP_CHARSET_UTF8);
        }

        oak_webserver_init();
        if (GlobalConfig.api_type == "internal")
        {
            oak_angel_init();
        }
        else if (GlobalConfig.api_type == "external")
        {
            oak_bridge_init();
        }
        else
        {
            oak_log("^F[^9ERROR^F] Invalid API type!^R\n");
            oak_webserver_stop();
            return EXIT_SUCCESS;
        }

        oak_network_init();
        oak_entities_init();

        generate_list();

        oak_console_input_handler_init();

        registerCmds();

        while (true)
        {
            oak_console_block_input(1);
            oak_network_tick();
            if (GlobalConfig.api_type == "external")
            {
                oak_bridge_tick();
            }
            else
            {
                oak_angel_tick();
            }

            oak_tasks_process();
            oak_console_block_input(0);
            zpl_sleep_ms(1);
        }

        return EXIT_SUCCESS;
#if defined(ZPL_SYSTEM_WINDOWS) && !defined(_DEBUG) && !defined(__GNUC__)
    }
    __except (oak_crash_handler(0, GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
    { 
        return EXIT_FAILURE;
    }
#endif
}

void shutdown_server()
{
    oak_log("^F[^5INFO^F] Server is ^9shutting ^Fdown...^R\n");

    oak_webserver_stop();
    oak_console_input_handler_destroy();
    
    if (GlobalConfig.api_type == "external")
    {
        oak_bridge_free();
    }
    else
    {
        oak_angel_stop();
    }

    oak_network_free();
    oak_sighandler_unregister();

    oak_cli_free();
    zpl_exit(0);
}

void oak_tasks_process() {
    oak_killbox_update();
    oak_vehicles_update();
    oak_console_console_update_stats();
    oak_scoreboard_update();
    oak_gamemap_update();
    oak_masterlist_update();
}
