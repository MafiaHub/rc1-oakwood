#pragma once


void store_bans() {
    std::ofstream out("config/banlist.txt");
    for (auto id : GlobalConfig.banned) {
        char line[128] = { 0 };
        ::sprintf(line, "%llu %s", id.first, id.second.c_str());
        out.write(line, strlen(line));
    }
    out.close();
}

void store_wh() {
    std::ofstream out("config/whitelist.txt");
    for (auto id : GlobalConfig.whitelisted) {
        char line[128] = { 0 };
        ::sprintf(line, "%llu %s", id.first, id.second.c_str());
        out.write(line, strlen(line));
    }
    out.close();
}

void add_ban(IDBind hwid) {
    for (auto id : GlobalConfig.banned) {
        if (id.first == hwid.first) {
            return;
        }
    }

    GlobalConfig.banned.push_back(hwid);
    store_bans();
}

void remove_ban(u64 hwid) {
    std::vector<IDBind> newList;

    for (auto id : GlobalConfig.banned) {
        if (id.first != hwid) {
            newList.push_back(id);
        }
    }

    GlobalConfig.banned = newList;
    store_bans();
}

void add_wh(IDBind hwid) {
    for (auto id : GlobalConfig.whitelisted) {
        if (id.first == hwid.first) {
            return;
        }
    }

    GlobalConfig.whitelisted.push_back(hwid);
    store_wh();
}

void remove_wh(u64 hwid) {
    std::vector<IDBind> newList;

    for (auto id : GlobalConfig.whitelisted) {
        if (id.first != hwid) {
            newList.push_back(id);
        }
    }

    GlobalConfig.whitelisted = newList;
    store_wh();
}

void toggle_wh(b32 state) {
    GlobalConfig.whitelistOnly = state;
}

