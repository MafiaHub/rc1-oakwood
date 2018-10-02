#pragma once

#define OAK_INTERNAL

#include "mod_api.hpp"

oak_api gm = {};
zpl_dll_handle dll_handle = 0;

auto set_up_natives() -> void;

auto init_api() {
    set_up_natives();
}

auto load_dll(const char *name) {
    if (!zpl_file_exists(name)) {
        zpl_printf("Gamemode '%s' not found! Exiting...", name);
        zpl_exit(1);
    }

    dll_handle = zpl_dll_load(name);

    if (!dll_handle) {
        zpl_printf("Gamemode '%s' has corrupted DLL file! Exiting...", name);
        zpl_exit(1);
    }

    #define xstr(x) str(x)
    #define str(x) #x

    auto proc = (oak_mod_entrypoint_ptr *)zpl_dll_proc_address(dll_handle, xstr(OAK_MOD_ENTRYPOINT_NAME));

    #undef xstr
    #undef str

    if (!proc) {
        zpl_printf("Gamemode '%s' is invalid! Exiting...", name);
        zpl_exit(1);
    }

    proc(&gm);

    zpl_printf("Gamemode '%s' has been loaded!\nAuthor: %s\nVersion: %s\n\n", gm.name.c_str(), gm.author.c_str(), gm.version.c_str());
}

auto free_dll() {
    zpl_dll_unload(dll_handle);
}