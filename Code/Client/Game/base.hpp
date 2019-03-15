#pragma once

#include "patches.hpp"
#include "game_events.hpp"
#include "CrashHandler/crashhandler.hpp"

static LONG WINAPI TerminateInstantly(LPEXCEPTION_POINTERS pointers) {
    TerminateProcess(GetCurrentProcess(), 0xDEADCAFE);
    return EXCEPTION_CONTINUE_SEARCH;
}

void mod_shutdown() {
    crashhandler::exceptions_capturing = false;
    if (librg_is_connected(&network_context)) {
        librg_network_stop(&network_context);
    }

    AddVectoredExceptionHandler(FALSE, TerminateInstantly);
    SetUnhandledExceptionFilter(TerminateInstantly);
    TerminateProcess(GetCurrentProcess(), 0);
}

inline auto mod_pre_init_game() {
    graphics::hook();
    input::hook();
    crashhandler::init();
    //voip::init();

    init_config();
    mod_bind_events();
    mod_init_networking();
    //alloc_console();
}

