#pragma once
struct mafia_door {
    mafia_door() {
        zpl_zero_item(this);
    }
    char name[32];
    f32 angle;
    u8 open_side;
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Door* door;
#endif
};
