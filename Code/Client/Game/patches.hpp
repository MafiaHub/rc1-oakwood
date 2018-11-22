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

	if (filename) {
		if (strstr(filename, "mainmenu.mnu")) {
			return DtaOpen("main.mnu", params);
		}

		if (strstr(filename, "online.tga")) {
			return DtaOpen("online.tga", params);
		}
	}
	
	return DtaOpen(filename, params);
}

auto mod_init_patches() {

	menu::init();
	HMODULE rw_data = GetModuleHandleA("rw_data.dll");
	DtaOpen = (DtaOpen_t)DetourFunction((PBYTE)GetProcAddress(rw_data, "_dtaOpen@8"), (PBYTE)dta_open_hook);

	MafiaSDK::C_Game_Patches::PatchDisableLogos();
	MafiaSDK::C_Game_Patches::PatchDisablePleaseWait();
	MafiaSDK::C_Game_Patches::PatchDisableSuspendProcess();
	MafiaSDK::C_Game_Patches::PatchDisableGameScripting();
	MafiaSDK::C_Game_Patches::PatchCustomPlayerRespawning();
	MafiaSDK::C_Game_Patches::PatchRemovePlayer();
	
	// 0004E034A
	// Force update car physics
	BYTE patchCarPhysics[] = "\xE9\xF1\x00\x00\x00\x90";
	MemoryPatcher::PatchAddress(0x0004E034A, patchCarPhysics, sizeof(patchCarPhysics));
}