#pragma once
namespace menu {

    enum Component {

        //Profile select
        FirstProfile = 100,

        //LoadingGame
        LoadingImage = 101,
        LoadingText = 200,

        //Game
        ExitGame = 102,
        ExitGameYes = 138,

        //Server Window
        ServerImage = 800,
        ServerIP = 801,
        ServerMaxPlayers = 802,
        ServerCurPlayers = 803,
        //Multiplayer Window
        DummyServer = 1200,
        ConnectIP = 1001,
        ConnectButton = 1002,
        ServerBrowserTitle = 1301,
        QuickConnectTitle = 1302,
        ServerIPLabel = 1303
    };

    struct ServerData {
        std::string server_name;
        std::string server_ip;
        std::string max_players;
        std::string current_players;
    };

    std::vector<ServerData> servers;

    #define set_text_if_found(ID, TEXT) \
        if(menu->FindComponentByID(ID)) \
            menu->SetText(ID, TEXT);

    #define set_string_if_found(ID, TEXT) \
        if(menu->FindComponentByID(ID)) \
            menu->SetString(ID, TEXT);


    inline auto replace_default_texts(MafiaSDK::GM_Menu* menu) -> void {
        
        //main menu
        set_text_if_found(Component::ServerBrowserTitle, "Server browser");
        set_text_if_found(Component::QuickConnectTitle, "Quck connect");
        set_text_if_found(Component::ConnectButton, "Connect");
        set_text_if_found(Component::ServerIPLabel,	"Enter IP:");
        set_text_if_found(Component::ConnectIP, GlobalConfig.server_address);
        set_string_if_found(Component::ServerImage, "freeride0.bmp");

        //game menu
        set_text_if_found(Component::ExitGame, "Disconnect");

        //loading
        set_text_if_found(Component::LoadingText, "Multiplayer");
        set_string_if_found(Component::LoadingImage, "online.tga");
    };

#define MASTER_PULL_TIMEOUT 3.0f
#define MASTER_RETRIES_MAX 3

    inline auto fetch_master_server() -> std::string {
        zpl_local_persist f32 fetch_time = 0.0f;
        zpl_local_persist u8 retries_count = 0;
        http_t* request = http_get("http://oakmaster.madaraszd.net/fetch", NULL);
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

        auto to_return = std::string(static_cast<const char*>(request->response_data));
        http_release(request);
        return to_return;
    }

    inline auto generate_browser_list(MafiaSDK::GM_Menu* menu) -> void {

        servers.clear();

        auto fetched_list = fetch_master_server();		
        if (!fetched_list.empty()) {

            u8 failed = 0;
            zpl_string json_config_data = zpl_string_make(zpl_heap(), fetched_list.c_str());
            zpl_json_object json_master_data = { 0 };

            zpl_json_parse(&json_master_data, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &failed);

            if (!failed) {

                zpl_json_object *server_property;
                zpl_json_find(&json_master_data, "server", false, &server_property);

                if (!server_property)
                    return;

                for (i32 i = 0, comp_id = Component::DummyServer - 1; 
                    i < zpl_array_count(server_property->nodes); ++i, ++comp_id) {
                    
                    zpl_json_object *server_node = (server_property->nodes + i);

                    zpl_json_object *server_property;
                    ServerData new_server_data;
                    
                    zpl_json_find(server_node, "name", false, &server_property);
                    new_server_data.server_name = std::string(server_property->string);

                    int next_id = comp_id + 1;
                    menu->PridejComponentPod(comp_id, next_id, 0, 0, 0, 0);
                    menu->SetText(next_id, server_property->string);
                    
                    zpl_json_find(server_node, "host", false, &server_property);
                    new_server_data.server_ip = std::string(server_property->string);
                    
                    zpl_json_find(server_node, "maxPlayers", false, &server_property);
                    new_server_data.max_players = "Max players: " + std::to_string(server_property->integer);

                    zpl_json_find(server_node, "players", false, &server_property);
                    new_server_data.current_players = "Players: " + std::to_string(server_property->integer);

                    servers.push_back(new_server_data);
                }
            }
        }
    }

    inline auto init() {

        MafiaSDK::GM_Menu_Hooks::HookOnMenuItemClick([&](unsigned int component_id) {

            if (component_id >= (int)Component::FirstProfile && 
                component_id <= ((int)Component::FirstProfile + 5)) {

                DWORD profile_ptr = 0x006D4628;
                auto first_elm = *(WORD*)(profile_ptr + 0x8);
                auto last_elm = *(WORD*)(profile_ptr + 0x4);
                u32 profile_count = (first_elm - last_elm) / 84;
            
                if (component_id <= Component::FirstProfile + profile_count) {
                    strcpy(GlobalConfig.username, MafiaSDK::GetGMMenu()->GetText(component_id));
                }
            }

            if (component_id == Component::ConnectButton) {
              
                strcpy(GlobalConfig.server_address, MafiaSDK::GetGMMenu()->GetText(Component::ConnectIP));
                if (strlen(GlobalConfig.server_address)) {
                    
                    auto player = get_local_player();
                    if (player) {
                        zpl_zero_item(player);
                    }

                    zpl_zero_item(&local_player);
                    librg_free(&network_context);
                    mod_init_networking();
                    MafiaSDK::GetGMMenu()->ReturnFromMenuExecute(29);
                }
            }

            if (component_id >= Component::DummyServer && component_id < Component::DummyServer + servers.size()) {
                auto server_item_idx = (component_id - Component::DummyServer);
                strcpy(GlobalConfig.server_address, servers.at(server_item_idx).server_ip.c_str());
                if (strlen(GlobalConfig.server_address)) {
                    MafiaSDK::GetGMMenu()->ReturnFromMenuExecute(29);
                }
            }

            //TODO: change this with confirm yes button later
            if (component_id == Component::ExitGameYes) {
                if(librg_is_connected(&network_context)) {
                    librg_network_stop(&network_context);
                    librg_free(&network_context);
                    mod_init_networking();
                }
            }
        });

        MafiaSDK::GM_Menu_Hooks::HookOnMenuItemHover([&](MafiaSDK::GM_Menu* menu, unsigned long component) {

            if (menu) {
                auto comp_id = menu->GetComponentID(component);
                if (comp_id >= Component::DummyServer && comp_id < Component::DummyServer + servers.size()) {
                    auto server_item_idx = (comp_id - Component::DummyServer);
                    menu->SetText(Component::ServerIP, servers.at(server_item_idx).server_ip.c_str());
                    menu->SetText(Component::ServerMaxPlayers, servers.at(server_item_idx).max_players.c_str());
                    menu->SetText(Component::ServerCurPlayers, servers.at(server_item_idx).current_players.c_str());
                }
            }
        });

        MafiaSDK::GM_Menu_Hooks::HookOnMenuCreated([&](MafiaSDK::GM_Menu* menu) {

            if (menu) {
                replace_default_texts(menu);

                if (menu->FindComponentByID(Component::ConnectButton)) {
                    generate_browser_list(menu);
                    if (servers.size()) {
                        menu->SetText(Component::ServerIP, servers.at(0).server_ip.c_str());
                        menu->SetText(Component::ServerMaxPlayers, servers.at(0).max_players.c_str());
                        menu->SetText(Component::ServerCurPlayers, servers.at(0).current_players.c_str());
                    }
                }
            }
        });
    }
}
