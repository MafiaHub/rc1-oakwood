#pragma once

struct _GlobalConfig {
    std::string name;
    std::string host;
    i64 port;
    i64 players;
    i64 max_players;
    std::string gamemode;
    b32 visible;
} GlobalConfig;

struct _request_player_data {
    char name[32];
} request_player_data;

librg_ctx network_context = { 0 };
