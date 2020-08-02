#pragma once

/*
    Sevenisko: Moved the hook into seperate file (for usage in both modules and code itself)
*/

typedef DWORD(_stdcall* DtaOpen_t) (const char* filename, DWORD params);
DtaOpen_t DtaOpen = nullptr;

std::string modpath = "";

inline void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

bool fileExists(const std::string& name) {
    if (FILE* file = fopen(name.c_str(), "rb")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

auto _stdcall dta_open_hook(const char* filename, DWORD params) -> DWORD
{
    if (modpath == "" || GlobalConfig.noNeedToLoad)
    {
        DWORD res = DtaOpen(filename, params);
        if (res < 0)
        {
            printf("Failed to load file (%d): %s\n", res, filename);
        }
        return res;
    }

    std::string newfile = modpath + "\\" + std::string(filename);

    std::string gpath = GlobalConfig.gamepath;

    replaceAll(gpath, "Game.exe", "");
    replaceAll(gpath, "/", "\\");

    std::string checkfile = gpath + modpath + "\\" + std::string(filename);

    DWORD res;

    if (fileExists(checkfile))
    {
        auto base = GetModuleHandle("rw_data.dll");

        *(BYTE*)(((DWORD)base + 0x12C98)) = 0;

        res = DtaOpen(newfile.c_str(), params);
        if (res < 0)
        {
            printf("Failed to load file (%d): %s\n", res, newfile.c_str());
        }
        else
        {
            printf("Loading modded file: %s -> %s\n", filename, newfile.c_str());
        }

        return res;
    }
    else
    {
        auto base = GetModuleHandle("rw_data.dll");

        *(BYTE*)(((DWORD)base + 0x12C98)) = 1;

        res = DtaOpen(filename, params);
        if (res < 0)
        {
            printf("Failed to load file (%d): %s\n", res, filename);
        }
    }

    return res;
}

auto mod_init_fs()
{
    HMODULE rw_data = GetModuleHandleA("rw_data.dll");
    DtaOpen = (DtaOpen_t)DetourFunction((PBYTE)GetProcAddress(rw_data, "_dtaOpen@8"), (PBYTE)dta_open_hook);
}
