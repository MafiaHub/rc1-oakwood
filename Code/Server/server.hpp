#pragma once

using IDBind = std::pair<u64, std::string>;

struct _GlobalConfig {
    std::string name;
    std::string host;
    i64 port;
    i64 players;
    i64 max_players;
    std::string mapname;
    std::string gamemode;
    b32 visible;
} GlobalConfig;

// TODO: rework this to avoid data race
struct _request_player_data {
    char name[32];
    u64 hwid;
} request_player_data;

std::vector<librg_entity*> connected_players;
