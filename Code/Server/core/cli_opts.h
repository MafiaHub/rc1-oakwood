zpl_opts oak__cli={0};

void oak_cli_init(int argc, char **argv) {
    zpl_opts_init(&oak__cli, zpl_heap(), argv[0]);
    zpl_opts_add(&oak__cli, "config", "cfg", "config to load.", ZPL_OPTS_STRING);
    zpl_opts_add(&oak__cli, "hidden", "hid", "set server visibility in masterlist.", ZPL_OPTS_FLAG);
    zpl_opts_add(&oak__cli, "host", "ip", "server hostname.", ZPL_OPTS_STRING);
    zpl_opts_add(&oak__cli, "name", "n", "server title.", ZPL_OPTS_STRING);
    zpl_opts_add(&oak__cli, "port", "p", "server port.", ZPL_OPTS_INT);
    zpl_opts_add(&oak__cli, "max-players", "mp", "max players to host.", ZPL_OPTS_INT);
    zpl_opts_add(&oak__cli, "help", "h", "display this help screen.", ZPL_OPTS_FLAG);

    b32 ok = zpl_opts_compile(&oak__cli, argc, argv);

    if (!ok) {
        zpl_opts_print_errors(&oak__cli);
        zpl_opts_print_help(&oak__cli);
        zpl_exit(-1);
    }
    else if (zpl_opts_has_arg(&oak__cli, "help")) {
        zpl_opts_print_help(&oak__cli);
        zpl_exit(-2);
    }

    oak_config_name_set(zpl_opts_string(&oak__cli, "config", oak_config_name_get()));
}

void oak_cli_replace() {
    b32 hide_server = zpl_opts_has_arg(&oak__cli, "hidden");
    if (hide_server) GlobalConfig.visible = false;

    GlobalConfig.max_players = zpl_opts_integer(&oak__cli, "max-players", GlobalConfig.max_players);
    GlobalConfig.port = zpl_opts_integer(&oak__cli, "port", GlobalConfig.port);
    GlobalConfig.host = zpl_opts_string(&oak__cli, "host", GlobalConfig.host.c_str());
    GlobalConfig.name = zpl_opts_string(&oak__cli, "name", GlobalConfig.name.c_str());
}

void oak_cli_free() {
    zpl_opts_free(&oak__cli);
}
