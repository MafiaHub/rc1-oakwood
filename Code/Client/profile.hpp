#pragma once

enum GameMenuStaticAddresses {
    //float
    ADDR_AIM_SENSITIVITY_X = 0x006D4B00,
    ADDR_AIM_SENSITIVITY_Y = 0x006D4B04,
    ADDR_AIM_SPEED = 0x006D4B08,
    ADDR_STEERING_LINEARITY = 0x006D4AEC,

    ADDR_SOUNDS_SLIDER = 0x006D4B0C,
    ADDR_CAR_SLIDER    = 0x00634B00,
    ADDR_MUSIC_SLIDER  = 0x006D4B10,
    ADDR_SPEECH_SLIDER = 0x006D4B18,

    //byte
    ADDR_CROSSHAIR_TYPE = 0x006D4B40,
    ADDR_SPEEDOMETER_TYPE = 0x006D4B44,
    ADDR_SIDE_ROLL = 0x006D4B45,
    ADDR_MOUSE_CONTROL = 0x006D4B46,
    ADDR_ENABLE_SUBTITLES = 0x006D4B47,
};


namespace Profile {
    struct ExtraFields {
        std::string address;
        int port;
    };

    /*
     * Load all settings from file
    */
    inline auto load_profile() -> void {

        std::string profile_path = GlobalConfig.localpath + "config/profile.json";
        const char* filename = profile_path.c_str();
        if (!zpl_fs_exists(filename)) {
            strcpy(GlobalConfig.username, "ChangeName");
            strcpy(GlobalConfig.server_address, "127.0.0.1");
            return;
        }

        /* read and copy the data */
        zpl_file_contents json_file = zpl_file_read_contents(zpl_heap(), true, filename);

        u8 failed = 0;
        zpl_string json_config_data = zpl_string_make(zpl_heap(), (const char*)json_file.data);

        zpl_json_object json_master_data = { 0 };
        zpl_json_parse(&json_master_data, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &failed);

        if (!failed) {
            zpl_json_object* node_property;

            //gobal settings & network
            node_property = zpl_json_find(&json_master_data, "username", false);
            if (node_property) {
                strcpy(GlobalConfig.username, (char*)node_property->string);

                if (!strlen(GlobalConfig.username))
                    strcpy(GlobalConfig.username, "ChangeName");
            }

            node_property = zpl_json_find(&json_master_data, "address", false);
            if (node_property) {
                strcpy(GlobalConfig.server_address, (char*)node_property->string);

                if(!strlen(GlobalConfig.server_address))
                    strcpy(GlobalConfig.server_address, "127.0.0.1");
            }

            json_apply(&json_master_data, GlobalConfig.port, port, integer, 27010);

            //game settings
            json_apply(&json_master_data, *(f32*)(ADDR_AIM_SENSITIVITY_X), aim_sensitivity_x, real, 0.0f);
            json_apply(&json_master_data, *(f32*)(ADDR_AIM_SENSITIVITY_Y), aim_sensitivity_y, real, 0.0f);
            json_apply(&json_master_data, *(f32*)(ADDR_AIM_SPEED), aim_speed, real, 0.0f);

            json_apply(&json_master_data, *(f32*)(ADDR_STEERING_LINEARITY), steering_linearity, real, 0.0f);
            json_apply(&json_master_data, *(u8*)(ADDR_CROSSHAIR_TYPE), crosshair_type, integer, 0);
            json_apply(&json_master_data, *(u8*)(ADDR_SPEEDOMETER_TYPE), speedometer_type, constant, 0);

            json_apply(&json_master_data, *(u8*)(ADDR_SIDE_ROLL), sideroll, constant, 0);
            json_apply(&json_master_data, *(u8*)(ADDR_MOUSE_CONTROL), mouse_control, constant, 0);
            json_apply(&json_master_data, *(u8*)(ADDR_ENABLE_SUBTITLES), enable_subtitles, constant, 0);

            json_apply(&json_master_data, *(f32*)(ADDR_SOUNDS_SLIDER), sounds_slider, real, 0.0f);
            json_apply(&json_master_data, *(f32*)(ADDR_CAR_SLIDER), cars_slider, real, 0.0f);
            json_apply(&json_master_data, *(f32*)(ADDR_MUSIC_SLIDER), music_slider, real, 0.0f);
            json_apply(&json_master_data, *(f32*)(ADDR_SPEECH_SLIDER), speech_slider, real, 0.0f);

            GlobalConfig.mus_volume = *(f32*)(ADDR_MUSIC_SLIDER);

            node_property = zpl_json_find(&json_master_data, "keys", false);
            if (!node_property)
                return;

            auto game_key_buffer = MafiaSDK::GetKeysBuffer();
            for (i32 i = 0; i < zpl_array_count(node_property->nodes); ++i) {
                zpl_json_object* server_node = (node_property->nodes + i);
                if (server_node) {
                    zpl_json_object* key_property;

                    WORD dik_key;
                    key_property = zpl_json_find(server_node, "dik", false);
                    dik_key = key_property->integer;

                    WORD type;
                    key_property = zpl_json_find(server_node, "type", false);
                    type = key_property->integer;

                    game_key_buffer->SetKey(dik_key, (MafiaSDK::GameKey_Type)type);
                    MafiaSDK::GetInput()->BindKey(*game_key_buffer, i);
                    game_key_buffer++;
                }
            }
        }
    }

    /*
    * Generates oakwood profile containing all game & multiplayer settings
    */
    inline auto generate_profile(ExtraFields extra) {

        zpl_json_object new_json_file = { 0 };
        zpl_json_init_node(&new_json_file, zpl_heap(), NULL, ZPL_JSON_TYPE_OBJECT);

        //global & keys
        zpl_json_object* new_node = zpl_json_add(&new_json_file, "username", ZPL_JSON_TYPE_STRING);
        new_node->string = GlobalConfig.username;

        new_node = zpl_json_add(&new_json_file, "address", ZPL_JSON_TYPE_STRING);
        new_node->string = (char*)extra.address.c_str();

        new_node = zpl_json_add(&new_json_file, "port", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = extra.port;

        //game settings
        new_node = zpl_json_add(&new_json_file, "aim_sensitivity_x", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_AIM_SENSITIVITY_X);

        new_node = zpl_json_add(&new_json_file, "aim_sensitivity_y", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_AIM_SENSITIVITY_Y);

        new_node = zpl_json_add(&new_json_file, "aim_speed", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_AIM_SPEED);

        new_node = zpl_json_add(&new_json_file, "steering_linearity", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_STEERING_LINEARITY);

        new_node = zpl_json_add(&new_json_file, "crosshair_type", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = *(u8*)(ADDR_CROSSHAIR_TYPE);

        new_node = zpl_json_add(&new_json_file, "speedometer_type", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = *(u8*)(ADDR_SPEEDOMETER_TYPE);

        new_node = zpl_json_add(&new_json_file, "sideroll", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = *(u8*)(ADDR_SIDE_ROLL);

        new_node = zpl_json_add(&new_json_file, "mouse_control", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = *(u8*)(ADDR_MOUSE_CONTROL);

        new_node = zpl_json_add(&new_json_file, "enable_subtitles", ZPL_JSON_TYPE_INTEGER);
        new_node->integer = *(u8*)(ADDR_ENABLE_SUBTITLES);

        //audio
        new_node = zpl_json_add(&new_json_file, "sounds_slider", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_SOUNDS_SLIDER);

        new_node = zpl_json_add(&new_json_file, "cars_slider", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_CAR_SLIDER);

        new_node = zpl_json_add(&new_json_file, "music_slider", ZPL_JSON_TYPE_REAL);
        new_node->real = GlobalConfig.mus_volume;

        new_node = zpl_json_add(&new_json_file, "speech_slider", ZPL_JSON_TYPE_REAL);
        new_node->real = *(f32*)(ADDR_SPEECH_SLIDER);

        //keys
        MafiaSDK::GameKey* game_key_buffer = MafiaSDK::GetKeysBuffer();
        zpl_json_object* keys_array = zpl_json_add(&new_json_file, "keys", ZPL_JSON_TYPE_ARRAY);

        std::vector<char*> allocated_key_names;

        for (u32 i = 0; i < 60; i++) {
            zpl_json_object* new_key_object = zpl_json_add(keys_array, NULL, ZPL_JSON_TYPE_OBJECT);

            new_node = zpl_json_add(new_key_object, "name", ZPL_JSON_TYPE_STRING);

            const char* geted_name = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[i]);
            if (geted_name) {
                new_node->string = zpl_alloc_str(zpl_heap(), geted_name);
                allocated_key_names.push_back(new_node->string);
            } else new_node->string = "Undefined";

            new_node = zpl_json_add(new_key_object, "dik", ZPL_JSON_TYPE_INTEGER);
            new_node->integer = game_key_buffer[i].GetDIK();

            new_node = zpl_json_add(new_key_object, "type", ZPL_JSON_TYPE_INTEGER);
            new_node->integer = game_key_buffer[i].GetType();
        }

        std::string profile_path = GlobalConfig.localpath + "config/profile.json";

        zpl_file file = { 0 };
        zpl_file_error error = zpl_file_create(&file, profile_path.c_str());
        if(error == ZPL_FILE_ERROR_NONE) {
            zpl_json_write(&file, &new_json_file, 0);
            zpl_file_close(&file);

            for (char* allocated_key : allocated_key_names)
                zpl_mfree(allocated_key);
        }

        zpl_json_free(&new_json_file);
    }

}
