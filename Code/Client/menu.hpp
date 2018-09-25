#pragma once
namespace menu {

	#define COMPONENT_DUMMY_SERVER		1200
	#define COMPONENT_IP				1001
	#define COMPONENT_CONNECT			1002
	#define COMPONENT_SERVER_BROWSER    1003
	#define COMPONENT_MP_CONSOLE		1301
	#define COMPONENT_QUCK_CONNECT_TILE 1302
	#define COMPONENT_ENTER_IP_LABEL    1303

	#define set_text_if_found(ID, TEXT) \
		if(menu->FindComponentByID(ID)) \
			menu->SetText(ID, TEXT);

	inline auto replace_default_texts(MafiaSDK::GM_Menu* menu) -> void {
		
		set_text_if_found(COMPONENT_MP_CONSOLE,				"Server browser");
		set_text_if_found(COMPONENT_QUCK_CONNECT_TILE,		"Quck connect");
		set_text_if_found(COMPONENT_CONNECT,				"Connect");
		set_text_if_found(COMPONENT_ENTER_IP_LABEL,			"Enter IP:");
		set_text_if_found(COMPONENT_IP, GlobalConfig.server_address.c_str());
	}

	inline auto generate_browser_list(MafiaSDK::GM_Menu* menu) -> void {

		const char* NameArray[10] = { "Cukier server", "Zaklaus linux server", "Your mom's server", "Change name", "itachi", "tobi", "madara", "naruto", "danzou", "kakashi" };

		int id = COMPONENT_DUMMY_SERVER;
		for (int i = 0; i < 9; i++) {
			int next_id = id + 1;
			menu->PridejComponentPod(id, next_id, 0, 0, 0, 0);
			menu->SetText(next_id, NameArray[i]);
			id++;
		}
	}

	inline auto init() -> void {

		MafiaSDK::GM_Menu_Hooks::HookOnMenuItemClick([&](unsigned int component_id) {

			if (component_id == COMPONENT_CONNECT) {
				GlobalConfig.server_address = std::string((const char*)MafiaSDK::GetGMMenu()->GetText(COMPONENT_IP));
				if (!GlobalConfig.server_address.empty()) {
					MafiaSDK::GetGMMenu()->ReturnFromMenuExecute(29);
				}
			}
		});

		MafiaSDK::GM_Menu_Hooks::HookOnMenuCreated([&](MafiaSDK::GM_Menu* menu) {

			if (menu) {
				replace_default_texts(menu);

				if (menu->FindComponentByID(COMPONENT_SERVER_BROWSER))
					generate_browser_list(menu);
			}
		});
	}
}