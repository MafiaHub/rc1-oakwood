#pragma once

namespace input {
    void block_input(bool);
}

void mod_librg_connect();

namespace ServerInfo {
#define MASTER_PULL_TIMEOUT 3.0f
#define MASTER_RETRIES_MAX 3

    struct ServerData {
        std::string server_name;
        std::string server_ip;
        std::string max_players;
        std::string current_players;
        std::string mapname;
        int port;
        bool valid;
    };

    ServerData lastServer;

    /*
    * Fetch all the servers
    */
    inline auto fetch_master_server() -> std::string {
        zpl_local_persist f32 fetch_time = 0.0f;
        zpl_local_persist u8 retries_count = 0;
        http_t* request = http_get("http://master.oakwood-mp.net/fetch", NULL);
        if (!request) {
            mod_log("[ServerBrowser] Invalid request.\n");
            MessageBoxW(0, L"Please, contact the developers! It could also be a connectivity issue between the server and you, make sure your connection is stable.", L"Invalid request!", MB_OK);
            return "{}";
        }

        fetch_time = zpl_time_now();
        http_status_t status = HTTP_STATUS_PENDING;
        int prev_size = -1;
        while (status == HTTP_STATUS_PENDING) {
            status = http_process(request);
            if (prev_size != (int)request->response_size) {
                prev_size = (int)request->response_size;
            }
            if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT) {
                if (retries_count >= MASTER_RETRIES_MAX) {
                    printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
                    MessageBoxW(0, L"Please, contact the developers! It could also be a connectivity issue between the server and you, make sure your connection is stable.", L"Master server is down!", MB_OK);
                    http_release(request);
                    return "{}";
                }
                retries_count++;
                return fetch_master_server();
            }
        }

        if (status == HTTP_STATUS_FAILED) {
            printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
            MessageBoxW(0, L"Master server is down!", L"Please, contact the developers!", MB_OK);
            http_release(request);
            return "{}";
        }

        fetch_time = 0.0f;
        retries_count = 0;

        auto val = std::string(static_cast<const char*>(request->response_data));
        http_release(request);
        return val;
    }

    inline auto fetch_server_info(std::string address, int port) -> std::string {
        zpl_local_persist f32 fetch_time = 0.0f;
        zpl_local_persist u8 retries_count = 0;
        http_t* request = http_get(zpl_bprintf("http://%s:%d/info", address.c_str(), port), NULL);
        if (!request) {
            return "{}";
        }

        fetch_time = zpl_time_now();
        http_status_t status = HTTP_STATUS_PENDING;
        int prev_size = -1;
        while (status == HTTP_STATUS_PENDING) {
            status = http_process(request);
            if (prev_size != (int)request->response_size) {
                prev_size = (int)request->response_size;
            }
            if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT) {
                http_release(request);
                return "{}";
            }
        }

        if (status == HTTP_STATUS_FAILED) {
            http_release(request);
            return "{}";
        }

        fetch_time = 0.0f;
        retries_count = 0;

        auto val = std::string(static_cast<const char*>(request->response_data));
        http_release(request);
        return val;
    }

    inline ServerData populate_server_data(zpl_json_object* server_node) {
        if (server_node == nullptr) {
            ServerData invalid_data;
            invalid_data.valid = false;
            return invalid_data;
        }

        zpl_json_object* server_property;
        ServerInfo::ServerData new_server_data;

        zpl_json_find(server_node, "name", false, &server_property);
        new_server_data.server_name = std::string(server_property->string);

        zpl_json_find(server_node, "host", false, &server_property);
        new_server_data.server_ip = std::string(server_property->string);

        zpl_json_find(server_node, "maxPlayers", false, &server_property);
        new_server_data.max_players = std::to_string(server_property->integer);

        zpl_json_find(server_node, "players", false, &server_property);
        new_server_data.current_players = std::to_string(server_property->integer);

        zpl_json_find(server_node, "port", false, &server_property);
        new_server_data.port = (int)std::atoi(server_property->string);

        zpl_json_find(server_node, "mapname", false, &server_property);
        new_server_data.mapname = std::string(server_property->string);

        return new_server_data;
    }

    inline ServerData fetch_server_data(std::string addr, int port) {
        auto info = fetch_server_info(addr, port);

        if (info == "{}") {
            return ServerData{};
        }

        zpl_string buf = zpl_string_make(zpl_heap(), info.c_str());
        zpl_json_object server_data = { 0 };
        zpl_json_parse(&server_data, zpl_string_length(buf), buf, zpl_heap(), false, NULL);
        ServerData data = populate_server_data(&server_data);
        zpl_string_free(buf);
        zpl_json_free(&server_data);
        return data;
    }

    inline void join_server(ServerInfo::ServerData server, b32 forceMapReload=true) {
        // server connection data is invalid, assume the server is offline
        if (!server.valid) {
            return;
        }

        strcpy(GlobalConfig.server_address, server.server_ip.c_str());
        GlobalConfig.port = server.port;
        lastServer = server;

        // NOTE: mapname is missing, use placeholder
        if (server.mapname == "") {
            server.mapname = "freeride";
        }

        input::block_input(false);
        
        if (forceMapReload)
            MafiaSDK::GetMission()->MapLoad(server.mapname.c_str());

        mod_librg_connect();
    }

    inline void join_last_server(b32 forceMapReload=true) {
        join_server(lastServer, forceMapReload);
    }
}