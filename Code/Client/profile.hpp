#pragma once

enum GameMenuStaticAddresses {
    //float
    AIM_SENSITIVITY_X = 0x006D4B00,
    AIM_SENSITIVITY_Y = 0x006D4B04,
    AIM_SPEED = 0x006D4B08,
    STEERING_LINEARITY = 0x006D4AEC,

    SOUNDS_SLIDER = 0x006D4B0C,
    SOUND_GAME_ADDR = 0x00634B00,
    MUSIC_SLIDER = 0x006D4B10,
    SPEECH_SLIDER = 0x006D4B18,

    //byte
    CROSSHAIR_TYPE = 0x006D4B40,
    SPEEDOMETER_TYPE = 0x006D4B44,
    SIDE_ROLL = 0x006D4B45,
    MOUSE_CONTROL = 0x006D4B46,
    ENABLE_SUBTITLES = 0x006D4B47,
};


namespace Profile {
    struct ExtraFields {
        std::string address;
        int port;
    };

    struct OakwoodProfile {
        u8 username[32];
        u8 address[32];
        u32 port;
        MafiaSDK::GameKey key_bindings[60];

        f32 aim_speed;
        f32 aim_sensitivity_x;
        f32 aim_sensitivity_y;
        f32 steering_linearity;

        u8 crosshair_type;
        u8 speedometer_type;
        u8 sideroll;
        u8 mouse_control;
        u8 enable_subtitles;

        f32 sounds_slider;
        f32 cars_slider;
        f32 music_slider;
        f32 speech_slider;
    };

    /*
        * Load all settings from file
        */
    inline auto load_profile() -> OakwoodProfile {
        std::ifstream load_file("OakwoodProfile.data", std::ios::binary);
        if (load_file.good()) {
            OakwoodProfile save_struct;
            load_file.read((char*)& save_struct, sizeof(OakwoodProfile));

            strcpy(GlobalConfig.username, (char*)save_struct.username);
            strcpy(GlobalConfig.server_address, (char*)save_struct.address);

            if (!strlen(GlobalConfig.username)) {
                strcpy(GlobalConfig.username, "ChangeName");
            }

            GlobalConfig.port = save_struct.port;

            auto game_key_buffer = MafiaSDK::GetKeysBuffer();
            memcpy(game_key_buffer, save_struct.key_bindings, sizeof(MafiaSDK::GameKey) * 60);
            for (int i = 0; i < 60; i++) {
                MafiaSDK::GetInput()->BindKey(*game_key_buffer, i);
                game_key_buffer++;
            }

            *(float*)(AIM_SENSITIVITY_X) = save_struct.aim_sensitivity_x;
            *(float*)(AIM_SENSITIVITY_Y) = save_struct.aim_sensitivity_y;
            *(float*)(AIM_SPEED) = save_struct.aim_speed;

            *(float*)(STEERING_LINEARITY) = save_struct.steering_linearity;
            *(BYTE*)(CROSSHAIR_TYPE) = save_struct.crosshair_type;
            *(bool*)(SPEEDOMETER_TYPE) = save_struct.speedometer_type;
            *(bool*)(SIDE_ROLL) = save_struct.sideroll;
            *(bool*)(MOUSE_CONTROL) = save_struct.mouse_control;
            *(bool*)(ENABLE_SUBTITLES) = save_struct.enable_subtitles;


            *(float*)(SOUNDS_SLIDER) = save_struct.sounds_slider;
            *(float*)(SOUND_GAME_ADDR) = save_struct.cars_slider;
            *(float*)(MUSIC_SLIDER) = save_struct.music_slider;
            *(float*)(SPEECH_SLIDER) = save_struct.speech_slider;

            return save_struct;
        }
    }

    /*
* Generates oakwood profile containing all game & multiplayer settings
*/
    inline auto generate_profile(ExtraFields extra) {
        OakwoodProfile save_struct;
        strcpy((char*)save_struct.username, GlobalConfig.username);

        strcpy((char*)save_struct.address, extra.address.c_str());
        save_struct.port = extra.port;

        memcpy(save_struct.key_bindings, MafiaSDK::GetKeysBuffer(), sizeof(MafiaSDK::GameKey) * 60);

        save_struct.aim_sensitivity_x = *(float*)(AIM_SENSITIVITY_X);
        save_struct.aim_sensitivity_y = *(float*)(AIM_SENSITIVITY_Y);
        save_struct.aim_speed = *(float*)(AIM_SPEED);

        save_struct.steering_linearity = *(float*)(STEERING_LINEARITY);
        save_struct.crosshair_type = *(BYTE*)(CROSSHAIR_TYPE);
        save_struct.speedometer_type = *(bool*)(SPEEDOMETER_TYPE);
        save_struct.sideroll = *(bool*)(SIDE_ROLL);
        save_struct.mouse_control = *(bool*)(MOUSE_CONTROL);
        save_struct.enable_subtitles = *(bool*)(ENABLE_SUBTITLES);


        save_struct.sounds_slider = *(float*)(SOUNDS_SLIDER);
        save_struct.cars_slider = *(float*)(SOUND_GAME_ADDR);
        save_struct.music_slider = *(float*)(MUSIC_SLIDER);
        save_struct.speech_slider = *(float*)(SPEECH_SLIDER);

        std::ofstream save_file("OakwoodProfile.data", std::ios::binary);
        save_file.write((const char*)& save_struct, sizeof(OakwoodProfile));
    }

}
