#pragma once

// todo
KeyToggle myDebugKey(VK_F2);

/* 
* Simple file replacment router
* Needs to be moved into separated file for later ussage
*/
typedef DWORD(_stdcall *DtaOpen_t) (const char* filename, DWORD params);
DtaOpen_t DtaOpen = nullptr;

auto _stdcall dta_open_hook(const char* filename, DWORD params) -> DWORD {

	if (strstr(filename, "mainmenu.mnu")){
		return DtaOpen("oakwood/Files/main.mnu", params);
	}

	return DtaOpen(filename, params);
}

auto mod_init_patches() -> void {

	menu::init();
	HMODULE rw_data = GetModuleHandleA("rw_data.dll");
	DtaOpen = (DtaOpen_t)DetourFunction((PBYTE)GetProcAddress(rw_data, "_dtaOpen@8"), (PBYTE)dta_open_hook);
	
	MafiaSDK::C_Indicators_Hooks::HookAfterDrawAll([&]() {
		render_nicknames();

		if (myDebugKey) {
			input::toggle_block_input();
		}
	});

	MafiaSDK::C_Game_Patches::PatchDisableLogos();
	MafiaSDK::C_Game_Patches::PatchDisablePleaseWait();
	MafiaSDK::C_Game_Patches::PatchDisableSuspendProcess();
	MafiaSDK::C_Game_Patches::PatchDisableGameScripting();
	MafiaSDK::C_Game_Patches::PatchCustomPlayerRespawning();
	MafiaSDK::C_Game_Patches::PatchRemovePlayer();
}