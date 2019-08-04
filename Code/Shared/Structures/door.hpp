#define OAK_MAX_DOOR_NAME_SIZE 32

#pragma once
struct mafia_door : public oak_object {
    char name[OAK_MAX_DOOR_NAME_SIZE];
    f32 angle;
    u8 open_side;
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Door* door;
#endif
};
