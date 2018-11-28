/* core libraries */
#include "zpl.h"
#include "loader/exeldr.h"

/* system libraries */
#include <string>

/* temp storage */
static std::string g_gamepath;
static std::string g_localpath;
typedef void(oakwood_proc)(const char *, const char *);

/* internal methods */
const char *launcher_localpath() {
    wchar_t buf[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, buf, MAX_PATH);

    //append string terminator
    for (size_t i = wcslen(buf); i > 0; --i) {
        if (buf[i] == '\\') { buf[i + 1] = 0; break; }
    }

    return (const char *)zpl_ucs2_to_utf8_buf((const u16 *)buf);
}

int launcher_abort(const char *msg) {
    auto buffer = (const wchar_t *)zpl_utf8_to_ucs2_buf((const u8 *)msg);
    MessageBoxW(NULL, buffer, L"Oakwood: Error", MB_OK | MB_ICONEXCLAMATION);
    ZPL_PANIC(msg);
    return 1;
}

/* hooked handlers */
LPSTR WINAPI GetCommandLineA_Hook() {
    static bool init = false; if (!init) {
        auto mod = LoadLibraryW(L"oakwood-client.dll"); if (mod) {
            auto oakwood_start = (oakwood_proc *)(GetProcAddress(mod, "oakwood_start")); if (oakwood_start) {
                // TODO: add custom entry point
                //oakwood_start(g_localpath.c_str(), g_gamepath.c_str());
            }
        } else {
            launcher_abort("Cannot find oakwood-client.dll!\n\nMake sure you've installed everything properly.");
        }

        init = true;
    }

    return GetCommandLineA();
}

void WINAPI RaiseException_Hook(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, const ULONG_PTR* lpArguments) {
    zpl_printf("[info] RaiseException hook\n");
    RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}

DWORD WINAPI GetModuleFileNameA_Hook(HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
    /* make sure we return our path to the game when it asks for the path */
    /* TODO: make sure we wont have conflicts in the future with our own calls */
    strcpy_s(lpFilename, nSize, g_gamepath.c_str());
    zpl_printf("[info] GetModuleFileName override: %s\n", lpFilename);

    return (DWORD)strlen(g_gamepath.c_str());
}

HANDLE WINAPI CreateMutexA_Hook(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) {
    if (!_strcmpi(lpName, "Mafia Launcher Super Mutex Shit")) {
         /* removing name allows multiple instances */
        return CreateMutexA(lpMutexAttributes, bInitialOwner, NULL);
    }

    return CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}

/* entry point handling */
int launcher_gameinit(std::string localpath, std::string gamepath) {
    g_localpath = std::string(localpath);
    g_gamepath  = std::string(gamepath);

    FILE* file = _wfopen((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)gamepath.c_str()), L"rb"); if (!file) {
        MessageBoxA(nullptr, "Failed to find executable image", "oh no", MB_ICONERROR);
        return 0;
    }

    // determine file length
    fseek(file, 0, SEEK_END);
    auto length = ftell(file);

    // reserve buffer
    std::vector<uint8_t> data(length);

    // read game into buffer
    fseek(file, 0, SEEK_SET);

    fread(data.data(), 1, length, file);
    fclose(file);

    // zpl_file_contents gamefile = zpl_file_read_contents(zpl_heap(), true, gamepath.c_str());
    zpl_printf("[info] loaded game binary (%lf MB)\n", length / 1024.0f / 1024.0f);

    auto base = GetModuleHandle(nullptr);
    ExecutableLoader loader(data.data());

    loader.SetLibraryLoader([](const char* library) -> HMODULE {
        // zpl_printf("[info] library: %s\n", library);
        return LoadLibraryA(library);
    });

    loader.SetFunctionResolver([](HMODULE hmod, const char* exportFn) -> LPVOID {
        //zpl_printf("[info] -- method: %s\n", exportFn);

        // early init hook
        if (!_strcmpi(exportFn, "GetCommandLineA")) {
            return static_cast<LPVOID>(GetCommandLineA_Hook);
        }

        /* fix for multiwindow game */
        if (!_strcmpi(exportFn, "CreateMutexA")) {
            return static_cast<LPVOID>(CreateMutexA_Hook);
        }

        // TODO: remove? (*temp hack*)
        if (!_strcmpi(exportFn, "RaiseException")) {
            return static_cast<LPVOID>(RaiseException_Hook);
        }

        // *fix pathing*
        if (!_strcmpi(exportFn, "GetModuleFileNameA")) {
            return static_cast<LPVOID>(GetModuleFileNameA_Hook);
        }

        return static_cast<LPVOID>(GetProcAddress(hmod, exportFn));
    });

    loader.LoadIntoModule(base);

    // store entry
    auto entry_point = static_cast<void(*)()>(loader.GetEP());

    // TODO: set hooking memory offset
    // set_base_offset(reinterpret_cast<uintptr_t>(base));

    // invoke original entry point
    entry_point();

    return 0;
}
