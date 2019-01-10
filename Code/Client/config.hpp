#pragma once
#include "config_reader.hpp"

constexpr const char* config_file_name = "multiplayer.json";
static const char *mod_default_config = R"foo(
username = ""
ip = "127.0.0.1"
view_distance = 0
)foo";

auto init_config() {
    auto json = config_get(config_file_name, mod_default_config);

    std::string server_addr, cur_username;

    json_apply(json, server_addr, ip, string, 0);
    json_apply(json, cur_username, username, string, 0);
    json_apply(json, GlobalConfig.view_distance, view_distance, integer, 0);

    strcpy(GlobalConfig.username, cur_username.c_str());
    strcpy(GlobalConfig.server_address, server_addr.c_str());
}
