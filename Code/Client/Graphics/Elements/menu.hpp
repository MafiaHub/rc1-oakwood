#pragma once
namespace menu {

	enum Component {
		//Game
		ExitGame = 102,
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
		set_text_if_found(Component::ConnectIP, GlobalConfig.server_address.c_str());
		set_string_if_found(Component::ServerImage, "freeride0.bmp");

		//game menu
		set_text_if_found(Component::ExitGame, "Disconnect");
	}

	inline auto fetch_master_server() -> std::string {

		http_t* request = http_get("http://localhost:3000/", NULL);
		if (!request) {
			mod_log("[ServerBrowser] Invalid request.\n");
			MessageBoxW(0, L"No to ma pojeb", L"Nebezi ti master ty kokotko", MB_OK);
			exit(EXIT_FAILURE);
			return "";
		}

		http_status_t status = HTTP_STATUS_PENDING;
		int prev_size = -1;
		while (status == HTTP_STATUS_PENDING) {
			status = http_process(request);
			if (prev_size != (int)request->response_size) {
				prev_size = (int)request->response_size;
			}
		}

		if (status == HTTP_STATUS_FAILED) {
			printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
            MessageBoxW(0, L"No to ma pojeb", L"Nebezi ti master ty kokotko", MB_OK);
            exit(EXIT_FAILURE);
			http_release(request);
			return "";
		}

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
				zpl_json_find(&json_master_data, "servers", false, &server_property);

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
					
					zpl_json_find(server_node, "ip", false, &server_property);
					new_server_data.server_ip = std::string(server_property->string);
					
					zpl_json_find(server_node, "players_max", false, &server_property);
					new_server_data.max_players = "Max players: " + std::to_string(server_property->integer);

					zpl_json_find(server_node, "players_now", false, &server_property);
					new_server_data.current_players = "Players: " + std::to_string(server_property->integer);

					servers.push_back(new_server_data);
				}
			}
		}
	}

	inline auto init() {

		MafiaSDK::GM_Menu_Hooks::HookOnMenuItemClick([&](unsigned int component_id) {

			if (component_id == Component::ConnectButton) {
				GlobalConfig.server_address = std::string((const char*)MafiaSDK::GetGMMenu()->GetText(Component::ConnectIP));
				if (!GlobalConfig.server_address.empty()) {
					MafiaSDK::GetGMMenu()->ReturnFromMenuExecute(29);
				}
			}

			if (component_id >= Component::DummyServer && component_id < Component::DummyServer + servers.size()) {
				auto server_item_idx = (component_id - Component::DummyServer);
				GlobalConfig.server_address = std::string(servers.at(server_item_idx).server_ip.c_str());
				if (!GlobalConfig.server_address.empty()) {
					MafiaSDK::GetGMMenu()->ReturnFromMenuExecute(29);
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

				if (menu->FindComponentByID(Component::ConnectButton))
					generate_browser_list(menu);
			}
		});
	}
}