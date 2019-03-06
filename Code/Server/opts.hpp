#pragma once

namespace opts {
    zpl_opts cli={0};
    
    void init(int argc, char **argv) {
        zpl_opts_init(&cli, zpl_heap(), argv[0]);
        zpl_opts_add(&cli, "gamemode", "gm", "gamemode to load.", ZPL_OPTS_STRING);
        zpl_opts_add(&cli, "config", "cfg", "config to load.", ZPL_OPTS_STRING);
        zpl_opts_add(&cli, "hidden", "hid", "set server visibility in masterlist.", ZPL_OPTS_FLAG);
        zpl_opts_add(&cli, "host", "ip", "server hostname.", ZPL_OPTS_STRING);
        zpl_opts_add(&cli, "name", "n", "server title.", ZPL_OPTS_STRING);
        zpl_opts_add(&cli, "port", "p", "server port.", ZPL_OPTS_INT);
        zpl_opts_add(&cli, "max-players", "mp", "max players to host.", ZPL_OPTS_INT);
        zpl_opts_add(&cli, "help", "h", "display this help screen.", ZPL_OPTS_FLAG);
        zpl_opts_positional_add(&cli, "gamemode");

        b32 ok = zpl_opts_compile(&cli, argc, argv);

        if (!ok) {
            zpl_opts_print_errors(&cli);
            zpl_opts_print_help(&cli);
            zpl_exit(-1);
        }
        else if (zpl_opts_has_arg(&cli, "help")) {
            zpl_opts_print_help(&cli);
            zpl_exit(2);
        }

        config::file_name = zpl_opts_string(&cli, "config", config::file_name);
    }

    void replace() {
        b32 hide_server = zpl_opts_has_arg(&cli, "hidden");
        if (hide_server) GlobalConfig.visible = false;

        GlobalConfig.max_players = zpl_opts_integer(&cli, "max-players", GlobalConfig.max_players);
        GlobalConfig.port = zpl_opts_integer(&cli, "port", GlobalConfig.port);
        GlobalConfig.host = zpl_opts_string(&cli, "host", GlobalConfig.host.c_str());
        GlobalConfig.name = zpl_opts_string(&cli, "name", GlobalConfig.name.c_str());
        GlobalConfig.gamemode = zpl_opts_string(&cli, "gamemode", GlobalConfig.gamemode.c_str());
    }

    void free() {
        zpl_opts_free(&cli);
    }
}