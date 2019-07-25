#pragma once
#include "config_reader.hpp"

namespace config {
    constexpr const char* config_file_name = "multiplayer.json";
    static const char *mod_default_config = R"foo(
    view_distance = 0
    )foo";

    auto init() {
        auto json = config_get(config_file_name, mod_default_config);
        json_apply(json, GlobalConfig.view_distance, view_distance, integer, 0);
    }
}
