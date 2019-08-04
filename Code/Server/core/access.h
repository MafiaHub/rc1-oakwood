zpl_global std::vector<IDBind> oak__access_whitelisted, oak__access_banned;
zpl_global b32 oak__access_whitelist_only;

void oak_access_bans_store() {
    std::ofstream out("config/banlist.txt");
    for (auto id : oak__access_banned) {
        char line[128] = { 0 };
        ::sprintf(line, "%llu %s\n", id.first, id.second.c_str());
        out.write(line, strlen(line));
    }
    out.close();
}

void oak_access_wh_store() {
    std::ofstream out("config/whitelist.txt");
    for (auto id : oak__access_whitelisted) {
        char line[128] = { 0 };
        ::sprintf(line, "%llu %s\n", id.first, id.second.c_str());
        out.write(line, strlen(line));
    }
    out.close();
}

void oak_access_wh_load() {
    std::string currentLine;
    std::ifstream inputFile("config/whitelist.txt");
    while (!inputFile.fail() && !inputFile.eof()) {
        std::getline(inputFile, currentLine);
        if (currentLine.length() == 0) continue;
        u64 hwid;
        char name[32] = { 0 };
        ::sscanf(currentLine.c_str(), "%llu %s", &hwid, name);
        oak__access_whitelisted.push_back(std::make_pair(hwid, std::string(name)));
    }
    inputFile.close();
}

void oak_access_bans_load() {
    std::string currentLine;
    std::ifstream inputFile("config/banlist.txt");
    while (!inputFile.fail() && !inputFile.eof()) {
        std::getline(inputFile, currentLine);
        if (currentLine.length() == 0) continue;
        u64 hwid;
        char name[32] = { 0 };
        ::sscanf(currentLine.c_str(), "%llu %s", &hwid, name);
        oak__access_banned.push_back(std::make_pair(hwid, std::string(name)));
    }
    inputFile.close();
}

void oak_access_bans_add(IDBind hwid) {
    for (auto id : oak__access_banned) {
        if (id.first == hwid.first) {
            return;
        }
    }

    oak__access_banned.push_back(hwid);
    oak_access_bans_store();
}

void oak_access_bans_remove(u64 hwid) {
    std::vector<IDBind> newList;

    for (auto id : oak__access_banned) {
        if (id.first != hwid) {
            newList.push_back(id);
        }
    }

    oak__access_banned = newList;
    oak_access_bans_store();
}

void oak_access_wh_add(IDBind hwid) {
    for (auto id : oak__access_whitelisted) {
        if (id.first == hwid.first) {
            return;
        }
    }

    oak__access_whitelisted.push_back(hwid);
    oak_access_wh_store();
}

void oak_access_wh_remove(u64 hwid) {
    std::vector<IDBind> newList;

    for (auto id : oak__access_whitelisted) {
        if (id.first != hwid) {
            newList.push_back(id);
        }
    }

    oak__access_whitelisted = newList;
    oak_access_wh_store();
}

void oak_access_wh_state_set(b32 state) {
    oak__access_whitelist_only = state;
}

int oak_access_wh_state_get() {
    return oak__access_whitelist_only;
}

int  oak_access_bans_get(u64 hwid) {
    for (auto id : oak__access_banned) {
        if (id.first == hwid) {
            return 1;
        }
    }

    return 0;
}

int  oak_access_wh_get(u64 hwid) {
    for (auto id : oak__access_whitelisted) {
        if (id.first == hwid) {
            return 1;
        }
    }

    return 0;
}
