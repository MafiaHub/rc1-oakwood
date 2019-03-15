namespace gamemap {
    inline void draw_player_cursor(void* vertex_buffer);
}

namespace modules { 
    namespace player {
        inline auto get_local_ped()->MafiaSDK::C_Player*;
        inline void use_door(MafiaSDK::C_Door* door, MafiaSDK::C_Door_Enum::States state);
    }

    #include "Modules/Chat/base.hpp"
    #include "Modules/Door/base.hpp"
    #include "Modules/Player/base.hpp"
    #include "Modules/Vehicle/base.hpp"
    #include "Modules/Misc/base.hpp"
    #include "Modules/WeaponDrop/base.hpp"
}
