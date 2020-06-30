/* INFO RESPONSE */

#include <algorithm>
#define OAK_ENDP_INFO_FORMAT "{\n\"host\": \"%s\",\n\"name\": \"%s\",\n\"players\": %d,\n\"maxPlayers\": %d,\n\"pass\": %s,\n\"port\": \"%d\",\n\"version\": \"%s\",\n\"mapname\": \"%s\"\n\"download-id\": \"%s\"\n\"download-json\": \"%s\"\n}"

std::string random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

int oak_endp_payload_info(char *buf) {
    return sprintf(buf, OAK_ENDP_INFO_FORMAT,
        GlobalConfig.host.c_str(),
        GlobalConfig.name.c_str(),
        (int)GlobalConfig.players,
        (int)GlobalConfig.max_players,
        (GlobalConfig.password.size()) ? "true" : "false",
        (int)GlobalConfig.port,
        OAK_VERSION,
        GlobalConfig.mapname.c_str(),
        GlobalConfig.download_id.c_str(),
        GlobalConfig.download_url.c_str()
    );
}
