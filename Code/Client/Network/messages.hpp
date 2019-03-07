#pragma once

auto mod_add_network_messages() {
    
    modules::player::add_messages();
    modules::misc::add_messages();
    modules::vehicle::add_messages();
    //#include "Messages/misc.hpp"
    //#include "Messages/vehicle.hpp"
}
