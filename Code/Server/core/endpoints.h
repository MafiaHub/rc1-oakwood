/* INFO RESPONSE */

#define OAK_ENDP_INFO_FORMAT "{\n\"host\": \"%s\",\n\"name\": \"%s\",\n\"players\": %d,\n\"maxPlayers\": %d,\n\"pass\": %s,\n\"port\": \"%d\",\n\"version\": \"%llx\",\n\"mapname\": \"%s\"\n}"

int oak_endp_payload_info(char *buf) {
    return sprintf(buf, OAK_ENDP_INFO_FORMAT,
        GlobalConfig.host.c_str(),
        GlobalConfig.name.c_str(),
        (int)GlobalConfig.players,
        (int)GlobalConfig.max_players,
        "false",
        (int)GlobalConfig.port,
        OAK_BUILD_VERSION,
        GlobalConfig.mapname.c_str()
    );
}
