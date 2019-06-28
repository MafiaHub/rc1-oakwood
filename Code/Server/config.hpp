#pragma once

#include "config_reader.hpp"

namespace config {
    #ifdef _WIN32 
        #define default_gamemode "SampleMod.dll"
    #elif __linux__
        #define default_gamemode "libSampleMod.so"
    #elif __APPLE__
        #define default_gamemode "SampleMod.dylib"
    #endif

    static const char* file_name = "config/server.json";
    static const char *mod_default_config = "max_players = 16\n"\
        "name = \"default oakwood server\"\n"\
        "host = \"\"\n"\
        "password = \"\"\n"\
        "visible = true\n"\
        "port = 27010\n"\
        "mapname = \"freeride\""\
        "gamemode = \"" default_gamemode "\"\n";



    auto init() {
        mod_log("Loading config...");
        
        auto json = config_get(file_name, mod_default_config);
        
        json_apply(json, GlobalConfig.host, host, string, "");
        json_apply(json, GlobalConfig.name, name, string, "default oakwood server");
        json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
        json_apply(json, GlobalConfig.mapname, mapname, string, "freeride");
        json_apply(json, GlobalConfig.gamemode, gamemode, string, default_gamemode);
        json_apply(json, GlobalConfig.port, port, integer, 27010);
        json_apply(json, GlobalConfig.visible, visible, constant, ZPL_JSON_CONST_TRUE);

        if (GlobalConfig.visible == ZPL_JSON_CONST_FALSE)
            GlobalConfig.visible = false;

        zpl_printf("================================\n");
        zpl_printf("Name: %s\n", GlobalConfig.name.c_str());
        zpl_printf("Module: %s\n", GlobalConfig.gamemode.c_str());
        zpl_printf("Max players: %d\n", GlobalConfig.max_players);
        zpl_printf("Port: %d\n", GlobalConfig.port);
        zpl_printf("Visible: %s\n", GlobalConfig.visible ? "yes" : "no");
        zpl_printf("================================\n");
    }
}
