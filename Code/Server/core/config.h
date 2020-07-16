#include "config_reader.hpp"

static const char *oak__config_file_name = "config/server.json";
static const char *oak__config_mod_default = "\n"\
    "/* How many players will be able to join this server */\nmax_players = 64\n"\
    "/* Server name shown in serverlist */\nname = \"default oakwood server\"\n"\
    "/* Server host (IP) */\nhost = \"\"\n"\
    "/* Server password - leave empty for free access */\npassword = \"\"\n"\
    "/*\n Determines, if server will push requests to masterlist\n Disable in case of local server, etc.\n*/\nvisible = true\n"\
    "/*\n Server port\n Both TCP and UDP must be unblocked and port-forwarded\n*/\nport = 27010\n"\
    "/* Scene name, which will server use */\nmapname = \"freeride\"\n"\
    "/* Height, which will cause player to kill (fell out of the world) */\nkillbox = -40.0\n"\
    "/*\n Script API Type:\n  - internal -> Internal API using AngelScript language\n  - bridge -> External API using bridge\n*/\napi_type = \"internal\"\n"\
    "/* Script file, which the internal API will run on startup */\nscript_file = \"gamemode.as\"\n"\
    "/* Bridge inbound address (for function calls) */\nbridge_inbound = \"ipc://oakwood-inbound\"\n"\
    "/* Bridge outbound address (for event calls)*/\nbridge_outbound = \"ipc://oakwood-outbound\"\n"\
    "/* Determines, if this server will use whitelist */\nwhitelist = false\n";

struct _GlobalConfig {
    std::string name;
    std::string host;
    i64 port;
    i64 players;
    i64 max_players;
    f64 killbox_level;
    std::string mapname;
    std::string password;
    std::string api_type, script_file;
    std::string bridge_inbound, bridge_outbound;
    b32 visible;
} GlobalConfig;

int oak_config_init() {
    oak_log("^FLoading config...^R\n");

    auto json = config_get(oak__config_file_name, oak__config_mod_default);
    b32 whOnly = false;

    json_apply(json, GlobalConfig.host, host, string, "127.0.0.1");
    json_apply(json, GlobalConfig.name, name, string, "default oakwood server");
    json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
    json_apply(json, GlobalConfig.killbox_level, killbox_level, real, -40.0);
    json_apply(json, GlobalConfig.mapname, mapname, string, "freeride");
    json_apply(json, GlobalConfig.password, password, string, "");
    json_apply(json, GlobalConfig.port, port, integer, 27010);
    json_apply(json, GlobalConfig.visible, visible, constant, ZPL_JSON_CONST_TRUE);
    json_apply(json, whOnly, whitelist, constant, ZPL_JSON_CONST_FALSE);
    json_apply(json, GlobalConfig.api_type, api_type, string, "internal");
    json_apply(json, GlobalConfig.script_file, script_file, string, "gamemode.as");
    json_apply(json, GlobalConfig.bridge_inbound, bridge_inbound, string, "ipc://oakwood-inbound");
    json_apply(json, GlobalConfig.bridge_outbound, bridge_outbound, string, "ipc://oakwood-outbound");

    if (GlobalConfig.visible == ZPL_JSON_CONST_FALSE)
        GlobalConfig.visible = false;

    oak_access_wh_state_set(whOnly);

    oak_access_wh_load();
    oak_access_bans_load();

    bool isAnyHost = (GlobalConfig.host == "") || (GlobalConfig.host == "127.0.0.1");

    oak_log("^B================================^R\n");
    oak_log("^FName: ^A%s^R\n", GlobalConfig.name.c_str());
    oak_log("^FMax players: ^A%d^R\n", (u32)GlobalConfig.max_players);
    oak_log("^FHost IP: ^A%s^R\n", isAnyHost ? "(local)" : GlobalConfig.host.c_str());
    oak_log("^FPort: ^A%d^R\n", (u32)GlobalConfig.port);
    oak_log("^FPassworded: ^A%s^R\n", (GlobalConfig.password != "") ? "yes" : "no");
    oak_log("^FAPI Type: ^A%s^R\n", GlobalConfig.api_type.c_str());
    if (GlobalConfig.api_type == "external")
    {
        oak_log("^FPublisher address: ^A%s^R\n", GlobalConfig.bridge_outbound.c_str());
        oak_log("^FListener address: ^A%s^R\n", GlobalConfig.bridge_inbound.c_str());
    }
    else
    {
        oak_log("^FScript file: ^A%s^R\n", GlobalConfig.script_file.c_str());
    }
    oak_log("^FVisible: ^A%s^R\n", GlobalConfig.visible ? "yes" : "no");
    oak_log("^B================================^R\n");

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
