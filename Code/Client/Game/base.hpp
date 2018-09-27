#pragma once

#include "factory.hpp"
#include "patches.hpp"
#include "game_events.hpp"

inline auto mod_pre_init_game() -> void {
    graphics::hook();
	input::hook();
	Sleep(300);
	input::hook_window();
}

inline auto mod_init_game() -> void {
    mod_init_patches();
    mod_bind_events();
}