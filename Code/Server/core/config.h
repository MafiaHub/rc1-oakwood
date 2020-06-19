#include "config_reader.hpp"

static const char *oak__config_file_name = "config/server.json";
static const char *oak__config_mod_default = "\n"\
    "max_players = 64\n"\
    "name = \"default oakwood server\"\n"\
    "host = \"\"\n"\
    "password = \"\"\n"\
    "visible = true\n"\
    "port = 27010\n"\
    "mapname = \"freeride\"\n"\
    "killbox = -40.0\n"\
    "bridge_inbound = \"ipc://oakwood-inbound\"\n"\
    "bridge_outbound = \"ipc://oakwood-outbound\"\n"\
    "whitelist = false\n";

struct _GlobalConfig {
    std::string name;
    std::string host;
    i64 port;
    i64 players;
    i64 max_players;
    f64 killbox_level;
    std::string mapname;
    std::string password;
    std::string bridge_inbound, bridge_outbound;
    b32 visible;
} GlobalConfig;

int oak_config_init() {
    oak_log("^FLoading config...^R\n");

    auto json = config_get(oak__config_file_name, oak__config_mod_default);
    b32 whOnly = false;

    json_apply(json, GlobalConfig.host, host, string, "");
    json_apply(json, GlobalConfig.name, name, string, "default oakwood server");
    json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
    json_apply(json, GlobalConfig.killbox_level, killbox_level, real, -40.0);
    json_apply(json, GlobalConfig.mapname, mapname, string, "freeride");
    json_apply(json, GlobalConfig.password, password, string, "");
    json_apply(json, GlobalConfig.port, port, integer, 27010);
    json_apply(json, GlobalConfig.visible, visible, constant, ZPL_JSON_CONST_TRUE);
    json_apply(json, whOnly, whitelist, constant, ZPL_JSON_CONST_FALSE);
    json_apply(json, GlobalConfig.bridge_inbound, bridge_inbound, string, "ipc://oakwood-inbound");
    json_apply(json, GlobalConfig.bridge_outbound, bridge_outbound, string, "ipc://oakwood-outbound");

    if (GlobalConfig.visible == ZPL_JSON_CONST_FALSE)
        GlobalConfig.visible = false;

    oak_access_wh_state_set(whOnly);

    oak_access_wh_load();
    oak_access_bans_load();

    oak_log("^B================================^R\n");
    oak_log("^FName: ^A%s^R\n", GlobalConfig.name.c_str());
    oak_log("^FMax players: ^A%d^R\n", (u32)GlobalConfig.max_players);
    oak_log("^FPort: ^A%d^R\n", (u32)GlobalConfig.port);
    oak_log("^FPassworded: ^A%s^R\n", (GlobalConfig.password != "") ? "yes" : "no");
    oak_log("^FPublisher address: ^A%s^R\n", GlobalConfig.bridge_outbound.c_str());
    oak_log("^FListener address: ^A%s^R\n", GlobalConfig.bridge_inbound.c_str());
    oak_log("^FVisible: ^A%s^R\n", GlobalConfig.visible ? "yes" : "no");
    oak_log("^B================================\n");

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

f32 oak_config_killbox_get() {
    return cast(f32)GlobalConfig.killbox_level;
}

int oak_config_killbox_set(f32 level) {
    GlobalConfig.killbox_level = cast(f32)level;

    return 0;
}

const char *oak_config_mapname_get() {
    return GlobalConfig.mapname.c_str();
}

const char *oak_config_bridge_inbound_get() {
    return GlobalConfig.bridge_inbound.c_str();
}

const char *oak_config_bridge_outbound_get() {
    return GlobalConfig.bridge_outbound.c_str();
}
