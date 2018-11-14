#pragma once
#include "config_reader.hpp"

constexpr const char* config_file_name = "multiplayer.json";
static const char *mod_default_config = R"foo(
username = "CHANGEME"
ip = "127.0.0.1"
view_distance = 0
)foo";

auto init_config() {
    auto json = config_get(config_file_name, mod_default_config);

    json_apply(json, GlobalConfig.server_address, ip, string, 0);
    json_apply(json, GlobalConfig.username, username, string, 0);
    json_apply(json, GlobalConfig.view_distance, view_distance, integer, 0);
}
