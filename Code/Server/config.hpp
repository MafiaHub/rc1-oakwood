#pragma once

#include "config_reader.hpp"

constexpr const char* config_file_name = "server.json";

static const char *mod_default_config = R"foo(
max_players = 16
port = 27010
gamemode = "SampleMod.dll"
)foo";

auto init_config() {
    mod_log("Loading config...");
    
    auto json = config_get(config_file_name, mod_default_config);
    
    json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
    json_apply(json, GlobalConfig.gamemode, gamemode, string, "SampleMod.dll");
    json_apply(json, GlobalConfig.port, port, integer, 27010);

    zpl_printf("================================\n");
    zpl_printf("Module: %s\n", GlobalConfig.gamemode.c_str());
    zpl_printf("Max players: %d\n", GlobalConfig.max_players);
    zpl_printf("Port: %d\n", GlobalConfig.port);
    zpl_printf("================================\n");
}
