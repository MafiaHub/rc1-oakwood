#pragma once

#include "methods.hpp"
#include "patches.hpp"
#include "game_events.hpp"
#include "CrashHandler/crashhandler.hpp"

static LONG WINAPI TerminateInstantly(LPEXCEPTION_POINTERS pointers) {
    TerminateProcess(GetCurrentProcess(), 0xDEADCAFE);
    return EXCEPTION_CONTINUE_SEARCH;
}

inline auto mod_init_game() {
    graphics::hook();
    input::hook();
    crashhandler::init();
    config::init();
    mod_bind_events();
    mod_init_networking();
}

