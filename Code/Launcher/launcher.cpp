/* core library */
#define ZPL_IMPL
#include "zpl.h"

/* system libraries */
#include <string>

/* settings */
#define OAKWOOD_CONSOLE 1
constexpr const char *OAKWOOD_CONFIG_NAME = "config.json";
constexpr const char *OAKWOOD_CONFIG_DATA = R"foo(
{
    /* path to game folder */
    "gamepath": "C:\\Program Files\\Steam\\steamapps\\common\\Mafia\\Mafia\\",
}
)foo";

/* platform specific launcher api */
const char *launcher_localpath();
int         launcher_abort(const char *msg);
int         launcher_gameinit(std::string localpath, std::string gamepath);

/* entry function */
#if defined(ZPL_SYSTEM_WINDOWS)
int wmain()
#else
int main()
#endif
{
    #if defined(ZPL_SYSTEM_WINDOWS) && defined(OAKWOOD_CONSOLE)
    { /* console handler */
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        SetConsoleTitleW(L"Oakwood: dev-console");

        freopen("CON", "w", stdout);
        freopen("CONIN$", "r", stdin);
    }
    #endif

    /* small local helper */
    #define concat(local, path) (local + "/" + path).c_str()

    std::string localpath = launcher_localpath();
    std::string gamepath;

    zpl_printf("[info] oakwood: %s\n", localpath.c_str());

    /* create default folder structure */
    zpl_path_mkdir(concat(localpath, "bin"), 0755);
    zpl_path_mkdir(concat(localpath, "bin"), 0755);
    zpl_path_mkdir(concat(localpath, "bin"), 0755);

    { /* gamepath */
        zpl_file file = {0};
        zplFileError error = zpl_file_open(&file, concat(localpath, OAKWOOD_CONFIG_NAME)); if (error != 0) {
            zpl_file_create(&file, concat(localpath, OAKWOOD_CONFIG_NAME));
            zpl_file_write(&file, OAKWOOD_CONFIG_DATA, zpl_strlen(OAKWOOD_CONFIG_DATA));
            zpl_file_seek(&file, 0);
        }

        /* read and copy the data */
        isize file_size = (isize)zpl_file_size(&file);
        char *content = (char *)zpl_malloc(file_size + 1); {
            zpl_file_read(&file, content, file_size);
            zpl_file_close(&file);
            content[file_size] = 0;
        };

        /* parse json */
        zpl_json_object root = {0}; u8 err;
        zpl_json_parse(&root, file_size, content, zpl_heap(), true, &err); {
            zpl_json_object *value;
            zpl_json_find(&root, "gamepath", false, &value); if (value && value->string) {
                gamepath = std::string(value->string);
            }
        }

        zpl_json_free(&root);
        zpl_mfree(content);
    }

    zpl_printf("[info] gamepath: %s\n", gamepath.c_str());
    if (!zpl_file_exists(concat(gamepath, "Game.exe"))) {
        return launcher_abort(("Cannot find a game executable by given path: " + gamepath).c_str());
    }

    #if defined(ZPL_SYSTEM_WINDOWS)
    { /* working directory, and library paths */
        auto addDllDirectory = (decltype(&AddDllDirectory))GetProcAddress(GetModuleHandle(L"kernel32.dll"), "AddDllDirectory");
        auto setDefaultDllDirectories = (decltype(&SetDefaultDllDirectories))GetProcAddress(GetModuleHandle(L"kernel32.dll"), "SetDefaultDllDirectories");

        if (addDllDirectory && setDefaultDllDirectories) {
            setDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);

            /* dll library search locations */
            AddDllDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)gamepath.c_str()));
            AddDllDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)localpath.c_str()));
            AddDllDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)concat(localpath, "bin")));

            /* game working directory */
            SetCurrentDirectoryW((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)gamepath.c_str()));
        }
    }
    #endif

    /* start game loading and initialization */
    return launcher_gameinit(localpath, gamepath + "\\Game.exe");
}
