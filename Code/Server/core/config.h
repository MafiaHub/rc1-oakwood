// #pragma once

// #include "config_reader.hpp"

// static const char* file_name = "config/server.json";
// static const char *mod_default_config = "max_players = 16\n"\
//     "name = \"default oakwood server\"\n"\
//     "host = \"\"\n"\
//     "password = \"\"\n"\
//     "visible = true\n"\
//     "port = 27010\n"\
//     "mapname = \"freeride\""\
//     "whitelist = false\n";


// auto init() {
//     mod_log("Loading config...");

//     auto json = config_get(file_name, mod_default_config);

//     json_apply(json, GlobalConfig.host, host, string, "");
//     json_apply(json, GlobalConfig.name, name, string, "default oakwood server");
//     json_apply(json, GlobalConfig.max_players, max_players, integer, 16);
//     json_apply(json, GlobalConfig.mapname, mapname, string, "freeride");
//     json_apply(json, GlobalConfig.gamemode, gamemode, string, default_gamemode);
//     json_apply(json, GlobalConfig.port, port, integer, 27010);
//     json_apply(json, GlobalConfig.visible, visible, constant, ZPL_JSON_CONST_TRUE);
//     json_apply(json, GlobalConfig.whitelistOnly, whitelist, constant, ZPL_JSON_CONST_FALSE);

//     if (GlobalConfig.visible == ZPL_JSON_CONST_FALSE)
//         GlobalConfig.visible = false;

//     if (GlobalConfig.whitelistOnly == ZPL_JSON_CONST_FALSE)
//         GlobalConfig.whitelistOnly = false;

//     if (GlobalConfig.whitelistOnly) {
//         std::string currentLine;
//         std::ifstream inputFile("config/whitelist.txt");
//         while (!inputFile.fail() && !inputFile.eof()) {
//             std::getline(inputFile, currentLine);
//             if (currentLine.length() == 0) continue;
//             u64 hwid;
//             char name[32] = { 0 };
//             ::sscanf(currentLine.c_str(), "%llu %s", &hwid, name);
//             GlobalConfig.whitelisted.push_back(std::make_pair(hwid, std::string(name)));
//         }
//         inputFile.close();
//     }

//     // handle bans
//     {
//         std::string currentLine;
//         std::ifstream inputFile("config/banlist.txt");
//         while (!inputFile.fail() && !inputFile.eof()) {
//             std::getline(inputFile, currentLine);
//             if (currentLine.length() == 0) continue;
//             u64 hwid;
//             char name[32] = { 0 };
//             ::sscanf(currentLine.c_str(), "%llu %s", &hwid, name);
//             GlobalConfig.banned.push_back(std::make_pair(hwid, std::string(name)));
//         }
//         inputFile.close();
//     }

//     zpl_printf("================================\n");
//     zpl_printf("Name: %s\n", GlobalConfig.name.c_str());
//     zpl_printf("Module: %s\n", GlobalConfig.gamemode.c_str());
//     zpl_printf("Max players: %d\n", GlobalConfig.max_players);
//     zpl_printf("Port: %d\n", GlobalConfig.port);
//     zpl_printf("Visible: %s\n", GlobalConfig.visible ? "yes" : "no");
//     zpl_printf("================================\n");
// }
