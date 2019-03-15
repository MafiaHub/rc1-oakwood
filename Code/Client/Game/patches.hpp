#pragma once
/* 
* Simple file replacment router
* Needs to be moved into separated file for later ussage
*/
typedef DWORD(_stdcall *DtaOpen_t) (const char* filename, DWORD params);
DtaOpen_t DtaOpen = nullptr;

auto _stdcall dta_open_hook(const char* filename, DWORD params) -> DWORD {
    // printf("requesting: %s\n", filename);

    if (filename) {
        if (strstr(filename, "mainmenu.mnu")) {
            return DtaOpen((GlobalConfig.localpath + "files/main.mnu").c_str(), params);
        }

        if (strstr(filename, "online.tga")) {
            return DtaOpen((GlobalConfig.localpath + "files/online.tga").c_str(), params);
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
    MafiaSDK::C_Game_Patches::PatchDisableGameScripting();
    MafiaSDK::C_Game_Patches::PatchCustomPlayerRespawning();
    MafiaSDK::C_Game_Patches::PatchRemovePlayer();
    MafiaSDK::C_Game_Patches::PatchDisableSuspendProcess();
    MafiaSDK::C_Game_Patches::PatchDisableInventory();
    // 0004E034A
    // Force update car physics
    // BYTE patchCarPhysics[] = "\xE9\xF1\x00\x00\x00\x90";
    // MemoryPatcher::PatchAddress(0x0004E034A, patchCarPhysics, sizeof(patchCarPhysics));
}
