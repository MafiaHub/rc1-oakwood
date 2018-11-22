#pragma once

#include "factory.hpp"
#include "commands.hpp"
#include "patches.hpp"
#include "game_events.hpp"

inline auto mod_pre_init_game() {
    graphics::hook();
	input::hook();
	Sleep(300);
	input::hook_window();
	//voip::init();
	//alloc_console();
}

inline auto mod_init_game() {
    mod_init_patches();
    mod_bind_events();
}