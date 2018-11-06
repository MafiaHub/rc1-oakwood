#pragma once

#include "config_reader.hpp"

#ifdef _WIN32 
    #define default_gamemode "SampleMod.dll"
#elif __linux__
    #define default_gamemode "SampleMod.so"
#elif
    #define default_gamemode "SampleMod.dylib"
#endif

constexpr const char* config_file_name = "server.json";
static const char *mod_default_config = "max_players = 16\n"\
    "port = 27010\n"\
    "gamemode = \"" default_gamemode "\"";



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
