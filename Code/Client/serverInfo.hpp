#pragma once

namespace input
{
void block_input(bool);
}

void mod_librg_connect();

namespace ServerInfo
{
#define MASTER_PULL_TIMEOUT 3.0f
#define MASTER_RETRIES_MAX 3
#define MASTER_ERROR_MSG "Master server is down!\nPlease, contact the developers! It could also be a connectivity issue between the server and you,\n make sure your connection is stable."

struct ServerData
{
    std::string server_name;
    std::string server_ip;
    std::string max_players;
    std::string current_players;
    std::string mapname;
    std::string version;
    std::string download_id, download_url;
    int port;
    bool valid;
    bool passworded;
    bool needToDown;
};

ServerData lastServer;

void msFailedExtraCb()
{
    if (ImGui::Button("Quit"))
    {
        exit(0);
    }

    ImGui::Text("Press OK to retry or Quit to exit the game.");
}

/*
    * Fetch all the servers
    */
inline auto fetch_master_server() -> std::string
{
    zpl_local_persist f32 fetch_time = 0.0f;
    zpl_local_persist u8 retries_count = 0;
    http_t *request = http_get("http://master.oakwood-mp.net/fetch", NULL);
    if (!request)
    {
        mod_log("[ServerBrowser] Invalid request.\n");
        modules::infobox::displayError(MASTER_ERROR_MSG, msFailedExtraCb);
        return "{}";
    }

    fetch_time = zpl_time_now();
    http_status_t status = HTTP_STATUS_PENDING;
    int prev_size = -1;
    while (status == HTTP_STATUS_PENDING)
    {
        status = http_process(request);
        if (prev_size != (int)request->response_size)
        {
            prev_size = (int)request->response_size;
        }
        if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT)
        {
            if (retries_count >= MASTER_RETRIES_MAX)
            {
                printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
                modules::infobox::displayError(MASTER_ERROR_MSG, msFailedExtraCb);
                http_release(request);
                return "{}";
            }
            retries_count++;
            return fetch_master_server();
        }
    }

    if (status == HTTP_STATUS_FAILED)
    {
        printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
        modules::infobox::displayError(MASTER_ERROR_MSG, msFailedExtraCb);
        http_release(request);
        return "{}";
    }

    fetch_time = 0.0f;
    retries_count = 0;

    auto val = std::string(static_cast<const char *>(request->response_data));
    http_release(request);
    return val;
}

inline auto fetch_server_info(std::string address, int port) -> std::string
{
    zpl_local_persist f32 fetch_time = 0.0f;
    zpl_local_persist u8 retries_count = 0;
    http_t *request = http_get(zpl_bprintf("http://%s:%d/info", address.c_str(), port), NULL);
    if (!request)
    {
        return "{}";
    }

    fetch_time = zpl_time_now();
    http_status_t status = HTTP_STATUS_PENDING;
    int prev_size = -1;
    while (status == HTTP_STATUS_PENDING)
    {
        status = http_process(request);
        if (prev_size != (int)request->response_size)
        {
            prev_size = (int)request->response_size;
        }
        if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT)
        {
            http_release(request);
            return "{}";
        }
    }

    if (status == HTTP_STATUS_FAILED)
    {
        http_release(request);
        return "{}";
    }

    fetch_time = 0.0f;
    retries_count = 0;

    auto val = std::string(static_cast<const char *>(request->response_data));
    http_release(request);
    return val;
}

inline ServerData populate_server_data(zpl_json_object *server_node)
{
    ServerData invalid_data;
    invalid_data.valid = false;

    if (server_node == nullptr)
    {
        return invalid_data;
    }

    zpl_json_object *server_property;
    ServerInfo::ServerData new_server_data;

    server_property = zpl_json_find(server_node, "version", false);
    new_server_data.version = std::string(server_property->string);

    semver_t ver = {0};
    int serr = semver_parse(new_server_data.version.c_str(), &ver);
    semver_free(&ver); // clean up before return (make sure to handle in case of metadata/prerelease)

    if (!serr && (ver.major != OAK_VERSION_MAJOR || ver.minor != OAK_VERSION_MINOR)) {
        return invalid_data;
    }

    new_server_data.valid = true;

    server_property = zpl_json_find(server_node, "name", false);
    new_server_data.server_name = std::string(server_property->string);

    server_property = zpl_json_find(server_node, "host", false);
    new_server_data.server_ip = std::string(server_property->string);

    server_property = zpl_json_find(server_node, "maxPlayers", false);
    new_server_data.max_players = std::to_string(server_property->integer);

    server_property = zpl_json_find(server_node, "players", false);
    new_server_data.current_players = std::to_string(server_property->integer);

    server_property = zpl_json_find(server_node, "port", false);
    new_server_data.port = (int)std::atoi(server_property->string);

    server_property = zpl_json_find(server_node, "pass", false);
    new_server_data.passworded = server_property->constant == ZPL_JSON_CONST_TRUE;

    std::string w = new_server_data.server_ip + ":" + std::to_string(new_server_data.port);

    md5_state_t state;
    md5_byte_t digest[16];
    char hex_output[16 * 2 + 1];
    int di;

    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)w.c_str(), w.size());
    md5_finish(&state, digest);
    for (di = 0; di < 16; ++di)
        sprintf(hex_output + di * 2, "%02x", digest[di]);

    new_server_data.download_id = std::string(hex_output);
    new_server_data.download_url = "http://" + w + "/files.json";

    return new_server_data;
}

inline ServerData fetch_server_data(std::string addr, int port)
{
    auto info = fetch_server_info(addr, port);

    if (info == "{}")
    {
        return ServerData{};
    }

    zpl_string buf = zpl_string_make(zpl_heap(), info.c_str());
    zpl_json_object server_data = {0};
    zpl_json_parse(&server_data, zpl_string_length(buf), buf, zpl_heap(), false, NULL);
    ServerData data = populate_server_data(&server_data);
    zpl_string_free(buf);
    zpl_json_free(&server_data);
    return data;
}

inline void join_server_wi(ServerInfo::ServerData server, b32 forceMapReload = true)
{
    // server connection data is invalid, assume the server is offline
    if (!server.valid)
    {
        return;
    }

    // Version mismatch, let the user know!
    // if (server.version != OAK_BUILD_VERSION)
    // {
    //     // modules::infobox::displayError("The client's version is incompatible with the server!");
    //     return;
    // }

    if (server.passworded && clientActiveState != ClientState_PasswordPrompt && !GlobalConfig.reconnecting) {
        modules::passwordPrompt::init(server);
        GlobalConfig.passworded = true;
        return;
    }
    else if (!server.passworded) {
        GlobalConfig.passworded = false;
    }

    if (server.download_url.size() && server.needToDown)
    {
        modules::dldialog::init(server);
        GlobalConfig.needToDownload = true;
        return;
    }
    else
    {
        GlobalConfig.needToDownload = false;
    }

    GlobalConfig.reconnecting = false;

    strcpy(GlobalConfig.server_address, server.server_ip.c_str());
    GlobalConfig.port = server.port;
    lastServer = server;

    // NOTE: mapname is missing, use placeholder
    if (server.mapname == "")
    {
        server.mapname = "freeride";
    }

    GlobalConfig.server_map = server.mapname;

    if (forceMapReload)
        MafiaSDK::GetMission()->MapLoad(server.mapname.c_str());

    mod_librg_connect();
}

inline void join_server(ServerInfo::ServerData server, b32 forceMapReload = true)
{
    // server connection data is invalid, assume the server is offline
    if (!server.valid)
    {
        return;
    }

    // Version mismatch, let the user know!
    // if (server.version != OAK_BUILD_VERSION)
    // {
    //     // modules::infobox::displayError("The client's version is incompatible with the server!");
    //     return;
    // }

    if (server.passworded && clientActiveState != ClientState_PasswordPrompt && !GlobalConfig.reconnecting) {
        modules::passwordPrompt::init(server);
        GlobalConfig.passworded = true;
        return;
    } else if (!server.passworded) {
        GlobalConfig.passworded = false;
    }

    if (server.download_url.size() && server.needToDown)
    {
        modules::dldialog::init(server);
        GlobalConfig.needToDownload = true;
        return;
    }
    else
    {
        GlobalConfig.needToDownload = false;
    }

    GlobalConfig.reconnecting = false;

    strcpy(GlobalConfig.server_address, server.server_ip.c_str());
    GlobalConfig.port = server.port;
    lastServer = server;

    // NOTE: mapname is missing, use placeholder
    if (server.mapname == "")
    {
        server.mapname = "freeride";
    }

    GlobalConfig.server_map = server.mapname;

    input::block_input(false);

    if (forceMapReload)
        MafiaSDK::GetMission()->MapLoad(server.mapname.c_str());

    mod_librg_connect();
}

inline void join_last_server(b32 forceMapReload = true)
{
    GlobalConfig.reconnecting = true;
    join_server(lastServer, forceMapReload);
}

} // namespace ServerInfo
