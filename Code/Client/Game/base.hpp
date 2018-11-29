#pragma once

#include "factory.hpp"
#include "commands.hpp"
#include "patches.hpp"
#include "game_events.hpp"
#include "Hooks/engine.hpp"

inline auto mod_pre_init_game() {
    graphics::hook();
    input::hook();
    hooks::engine::init();
    //voip::init();

    init_config();
    mod_bind_events();
    mod_init_networking();
    alloc_console();
}

