#include "config_reader.hpp"

static const char *oak__config_file_name = "config/server.json";
static const char *oak__config_mod_default = "\n"\
    "max_players = 64\n"\
    "name = \"default oakwood server\"\n"\
    "host = \"\"\n"\
    "password = \"\"\n"\
    "visible = true\n"\
    "port = 27010\n"\
    "mapname = \"freeride\""\
    "whitelist = false\n";

struct _GlobalConfig {
    std::string name;
    std::string host;
    i64 port;
    i64 players;
    i64 max_players;
    std::string mapname;
    std::string gamemode;
    b32 visible;
} GlobalConfig;

int oak_config_init() {
    oak_log("Loading config...\n");

    auto json = config_get(oak__config_file_name, oak__config_mod_default);
    b32 whOnly = false;

    json_apply(json, GlobalConfig.host, host, string, "");
    json_apply(json, GlobalConfig.name, name, string, "default oakwood server");
    json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
    json_apply(json, GlobalConfig.mapname, mapname, string, "freeride");
    json_apply(json, GlobalConfig.port, port, integer, 27010);
    json_apply(json, GlobalConfig.visible, visible, constant, ZPL_JSON_CONST_TRUE);
    json_apply(json, whOnly, whitelist, constant, ZPL_JSON_CONST_FALSE);

    if (GlobalConfig.visible == ZPL_JSON_CONST_FALSE)
        GlobalConfig.visible = false;

    oak_access_wh_state_set(whOnly);

    oak_access_wh_load();
    oak_access_bans_load();

    oak_log("================================\n");
    oak_log("Name: %s\n", GlobalConfig.name.c_str());
    oak_log("Max players: %d\n", (u32)GlobalConfig.max_players);
    oak_log("Port: %d\n", (u32)GlobalConfig.port);
    oak_log("Visible: %s\n", GlobalConfig.visible ? "yes" : "no");
    oak_log("================================\n");

    return 0;
}

int oak_config_port_get() {
    return GlobalConfig.port;
}

int oak_config_maxplayers_get() {
    return GlobalConfig.max_players;
}

int oak_config_visible_get() {
    return GlobalConfig.visible;
}

const char *oak_config_name_get() {
    return oak__config_file_name;
}

void oak_config_name_set(const char *filename) {
    oak__config_file_name = filename;
}

const char *oak_config_host_get() {
    return GlobalConfig.host.c_str();
}

const char *oak_config_mapname_get() {
    return GlobalConfig.mapname.c_str();
}

