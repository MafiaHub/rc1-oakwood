#pragma once

#define OAK_INTERNAL

#include "mod_api.hpp"

oak_api gm = {};
zpl_dll_handle dll_handle = 0;

auto set_up_natives() -> void;

auto init_api() {
    mod_log("Loading gamemode...");
    set_up_natives();
}

#define xstr(x) str(x)
#define str(x) #x

auto load_dll(const char *name) {
    if (!zpl_file_exists(name)) {
        zpl_printf("Gamemode '%s' not found! Exiting...\n", name);
        getchar();
        zpl_exit(1);
    }

    dll_handle = zpl_dll_load(name);

    if (!dll_handle) {
        zpl_printf("Gamemode '%s' has corrupted DLL file! Exiting...\n", name);
        getchar();
        zpl_exit(1);
    }

    auto proc = (oak_mod_entrypoint_ptr *)zpl_dll_proc_address(dll_handle, xstr(OAK_MOD_ENTRYPOINT_NAME));

    if (!proc) {
        zpl_printf("Gamemode '%s' is invalid! Exiting...\n", name);
        getchar();
        zpl_exit(1);
    }

    proc(&gm);

    zpl_printf("================================\n");
    zpl_printf("Gamemode '%s' has been loaded!\nAuthor: %s\nVersion: %s\n", gm.name.c_str(), gm.author.c_str(), gm.version.c_str());
    zpl_printf("================================\n");
}

auto free_dll() {
    auto proc = (oak_mod_shutdown_ptr *)zpl_dll_proc_address(dll_handle, xstr(OAK_MOD_SHUTDOWN_NAME));

    if (!proc) {
        zpl_printf("Gamemode '%s' has no shutdown procedure ...\n", gm.name.c_str());
    }

    proc(&gm);
    
    zpl_dll_unload(dll_handle);
}

#undef xstr
#undef str
