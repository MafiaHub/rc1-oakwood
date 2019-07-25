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
    b32 whitelistOnly;
    std::vector<IDBind> whitelisted, banned;
} GlobalConfig;

// TODO: rework this to avoid data race
struct _request_player_data {
    char name[32];
    u64 hwid;
} request_player_data;

librg_ctx network_context = { 0 };
std::vector<librg_entity*> connected_players;

extern void store_bans();
extern void store_wh();
extern void add_ban(IDBind id);
extern void remove_ban(u64 id);
extern void add_wh(IDBind id);
extern void remove_wh(u64 id);
extern void toggle_wh(b32 state);
extern void execute_command(std::string msg);
