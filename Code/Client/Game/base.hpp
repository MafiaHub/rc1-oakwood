#pragma once

#include "factory.hpp"
#include "commands.hpp"
#include "patches.hpp"
#include "game_events.hpp"

inline void mod_init_game();
#include "Hooks/engine.hpp"

inline auto mod_pre_init_game() {
    graphics::hook();
    input::hook();
    hooks::engine::init();
    //voip::init();
    alloc_console();
}

inline void mod_init_game() {
    input::hook_window();
    mod_bind_events();
}

