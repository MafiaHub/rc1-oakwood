#pragma once

auto mod_add_network_messages() {
    modules::player::add_messages();
    modules::misc::add_messages();
    modules::vehicle::add_messages();
    modules::playerlist::add_messages();
    modules::gamemap::add_messages();
}
