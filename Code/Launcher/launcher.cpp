/* core library */
#define ZPL_IMPL
#include "zpl.h"

/* system libraries */
#include "shellapi.h" // CommandLineToArgvW
#include <string>
#include <algorithm>

#include "settings.h"

/* platform specific launcher api */
const char *launcher_localpath();
int launcher_abort(const char *msg);
int launcher_gameinit(std::string localpath, std::string gamepath);

/* small local helper */
#define concat(local, path) (local + "/" + path).c_str()
#define json_apply(OBJ, VAR, NAME, STR, DEF, CAST)           \
    do {                                                     \
        zpl_json_object *NAME = zpl_json_find(OBJ, #NAME, false);     \
        VAR = (CAST)((NAME) ? NAME->STR : DEF); \
    } while (0)

/* entry function */
#if defined(ZPL_SYSTEM_WINDOWS)
int wmain()
#else
int main()
#endif
{
#if defined(ZPL_SYSTEM_WINDOWS) && OAKWOOD_CONSOLE == 1
    { /* console handler */
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        SetConsoleTitleW(L"Oakwood: dev-console");

        freopen("CON", "w", stdout);
        freopen("CONIN$", "r", stdin);
    }
#endif

    std::string localpath = launcher_localpath();
    std::string gamepath;

    /* special code used primarily on vs while debugging */
    /* simplifies ./Build/../Bin/ -> ./Bin/ */
    auto prevdir = localpath.find("\\..\\");
    if (prevdir != std::string::npos) {
        auto first = localpath.substr(0, localpath.find_last_of("\\", prevdir - 1));
        auto last = localpath.substr(prevdir + 4);
        localpath = first + "\\" + last;
    }

    zpl_printf("[info] Oakwood path: %s\n", localpath.c_str());

    /* create default folder structure */
    zpl_path_mkdir(concat(localpath, "bin"), 0755);
    zpl_path_mkdir(concat(localpath, "cache"), 0755);
    zpl_path_mkdir(concat(localpath, "config"), 0755);

    { /* gamepath */
        char *config_name = (char *)OAKWOOD_CONFIG_NAME;

        int argc;
        auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        if (argc > 1) {
            config_name = (char *)zpl_ucs2_to_utf8_buf((u16 *)argv[1]);
            zpl_printf("[info] Custom config used: %s\n", config_name);
        }

        zpl_file file = {0};
        zplFileError error = zpl_file_open(&file, concat(localpath, config_name));
        if (error != 0) {
            zpl_file_create(&file, concat(localpath, OAKWOOD_CONFIG_NAME));
            zpl_file_write(&file, OAKWOOD_CONFIG_DATA, zpl_strlen(OAKWOOD_CONFIG_DATA));
            zpl_file_seek(&file, 0);
        }

        LocalFree(argv);

        /* read and copy the data */
        isize file_size = (isize)zpl_file_size(&file);
        char *content = (char *)zpl_malloc(file_size + 1);
        {
            zpl_file_read(&file, content, file_size);
            zpl_file_close(&file);
            content[file_size] = 0;
        };

        /* parse json */
        zpl_json_object root = {0};
        u8 err;
        zpl_json_parse(&root, file_size, content, zpl_heap(), true, &err);
        {
            std::string temp_gamepath;
            int isfullscreen;

            json_apply(&root, temp_gamepath, gamepath, string, 0, std::string);
            json_apply(&root, settings.width, width, integer, 1600, u16);
            json_apply(&root, settings.height, height, integer, 900, u16);
            json_apply(&root, isfullscreen, fullscreen, constant, 1, u8);
            json_apply(&root, settings.antialiasing, antialiasing, integer, 4, u8);

            gamepath = temp_gamepath;
            settings.fullscreen = isfullscreen == ZPL_JSON_CONST_TRUE;
        }

        zpl_json_free(&root);
        zpl_mfree(content);
    }

    std::replace(gamepath.begin(), gamepath.end(), '\\', '/');

    zpl_printf("[info] Game path: %s\n", gamepath.c_str());
    if (!zpl_fs_exists(concat(gamepath, "Game.exe"))) {
        return launcher_abort(("Cannot find a game executable by given path:\n" + gamepath + "\n\n Please check your path and try again!").c_str());
    }

#if defined(ZPL_SYSTEM_WINDOWS)
    { /* working directory, and library paths */
        auto addDllDirectory = (decltype(&AddDllDirectory))GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "AddDllDirectory");
        auto setDefaultDllDirectories = (decltype(&SetDefaultDllDirectories))GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetDefaultDllDirectories");

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
    return launcher_gameinit(localpath, gamepath + "/Game.exe");
}
