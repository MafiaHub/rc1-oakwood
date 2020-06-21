namespace mainmenu {

    S_vector camera_pos = {36.53f, 4.62f, 264.965f};
    S_vector camera_dir = {0.896f, -0.0488f, 0.443f};

    #define GET_TEXT(id) \
        MafiaSDK::GetTextDatabase()->GetText((unsigned int)id)

    enum TextDBEnum {
        //Controlls menu texts
        TEXT_PLAYER = 206,
        TEXT_CAR = 207,
        TEXT_OTHERS = 208,
        TEXT_CONTROLS = 215,
        TEXT_PRIMARY = 354,
        TEXT_SECONDARY = 355,
        TEXT_AIMING = 225,
        TEXT_SENSITIVITY_X = 222,
        TEXT_SENSITIVITY_Y = 223,
        TEXT_AIM_SPEED = 224,
        TEXT_STEERING = 219,
        TEXT_STEERING_LINEARITY = 220,
        TEXT_NEXT_OTHER = 226,
        TEXT_CROSSHAIR = 234,
        TEXT_CENTER_POINT = 235,
        TEXT_CROSSHAIR_CENTER_POINT = 236,
        TEXT_SPEEDOMETER = 347,
        TEXT_SIDE_ROLL = 247,
        TEXT_MOUSE_CONTROL = 256,
        TEXT_SUBTITLES = 227,
        TEXT_RESET_TO_DEFAULT = 211,

        //Video settings texts
        TEXT_VIDEO = 230,
        TEXT_LEVEL_OF_DETAIL = 231,
        TEXT_DRAW_DISTANCE = 232,
        TEXT_LEVEL_OF_BLOOD = 238,
        TEXT_IN_GAME_EFFECTS = 260,
        TEXT_IN_GAME_SOUNDS = 270,
        TEXT_SHADOWS = 280,
        TEXT_PARTICLES = 290,
        TEXT_GAMMA = 239,

        //Audio settings texts
        TEXT_AUDIO = 250,
        TEXT_SOUNDS = 251,
        TEXT_MUSIC = 252,
        TEXT_CARS = 253,
        TEXT_SPEECH = 254
    };

    std::vector<ServerInfo::ServerData> servers;
    std::vector<const char*> car_binds = { "Accelerate", "Brake/Back", "Turn Left", "Turn Right", "Handbrake", "Speed Limiter", "Gear change up", "Gear change down", "Horn", "Look Left", "Look Right", "Clutch", "Manual/Automatic transmission", "Change Camera", "Put back on the Track" };
    std::vector<const char*> player_binds = { "Move Forward", "Move Backwards", "Strafe Left", "Strafe Right", "Walk", "Action Button (Use)", "Fire", "Crouch", "Jump", "Aim", "Next Weapon", "Prev Weapon", "Inventory", "Hide Weapon", "Throw Weapon", "Switch Run/Walk", "Reload", "Sniper Mode", "Objectives", "Map" };
    int is_picking_key              = -1;
    BYTE old_dik_buffer[256];
    char qc_address[32] = "127.0.0.1";
    int qc_port;

    /*
    * Parse json response from master server
    */
    inline auto generate_browser_list() -> void {

        servers.clear();
        auto fetched_list = ServerInfo::fetch_master_server();
        if (!fetched_list.empty()) {
            u8 failed = 0;
            zpl_string json_config_data = zpl_string_make(zpl_heap(), fetched_list.c_str());
            zpl_json_object json_master_data = { 0 };
            zpl_json_parse(&json_master_data, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &failed);

            if (!failed) {
                zpl_json_object* server_property;
                server_property = zpl_json_find(&json_master_data, "servers", false);

                if (!server_property)
                    return;

                for (i32 i = 0; i < zpl_array_count(server_property->nodes); ++i) {
                    zpl_json_object* server_node = (server_property->nodes + i);
                    ServerInfo::ServerData new_server_data = ServerInfo::populate_server_data(server_node);
                    if (!new_server_data.valid) continue;

                    servers.push_back(new_server_data);
                }
            }
        }
    }

    inline void init() {
        Profile::load_profile();
        strcpy(qc_address, (char *)GlobalConfig.server_address);
        qc_port = GlobalConfig.port;

        generate_browser_list();
        input::block_input(true);
    }

    /*
    * Starts executing picking menu, we re-enabling ingame input
    * And comparing engines-dik buffer for changes
    * TODO(DavoSK): Add check for other type of buttons ten keyboard
    */
    inline void execute_picking(int key_idx) {
        memcpy(old_dik_buffer, MafiaSDK::GetEngineDikBuffer(), 256);
        is_picking_key = (key_idx - 1);
        input::block_input(false);
    }

    inline void render_input_settings() {
        if (ImGui::BeginTabBar("input")) {
            if (ImGui::BeginTabItem("Player")) {
                ImGui::Columns(3, "mycolumns");
                ImGui::Text("Action"); ImGui::NextColumn();
                ImGui::Text("Primary"); ImGui::NextColumn();
                ImGui::Text("Secondary"); ImGui::NextColumn();
                ImGui::Separator();

                MafiaSDK::GameKey* game_key_buffer = MafiaSDK::GetKeysBuffer();
                int key_idx = 0;
                for (int i = 0; i < 20; i++) {
                    ImGui::Text(player_binds.at(i)); ImGui::NextColumn();
                    const char* key_name = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);

                    ImGui::PushID(key_idx);
                    if (key_name && ImGui::Button(key_name)) {
                        execute_picking(key_idx);
                    }
                    if (!key_name && ImGui::Button("NONE")) {
                        execute_picking(key_idx);
                    }
                    ImGui::PopID();

                    ImGui::NextColumn();

                    if (key_idx < 24) {
                        const char* key_name_next = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);
                        ImGui::PushID(key_idx);
                        if (key_name_next && ImGui::Button(key_name_next)) {
                            execute_picking(key_idx);
                        }
                        if (!key_name_next && ImGui::Button("NONE")) {
                            execute_picking(key_idx);
                        }
                        ImGui::PopID();
                        ImGui::NextColumn();
                    }
                    else {
                        ImGui::Text(" "); ImGui::NextColumn();
                    }
                }

                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::EndTabItem();

                if (ImGui::Button("Reset to defaults")) {
                    MafiaSDK::GetInput()->ResetDefault(1);
                }
            }

            if (ImGui::BeginTabItem("Car")) {
                ImGui::Columns(3, "mycolumns");
                ImGui::Text("Action"); ImGui::NextColumn();
                ImGui::Text("Primary"); ImGui::NextColumn();
                ImGui::Text("Secondary"); ImGui::NextColumn();
                ImGui::Separator();

                MafiaSDK::GameKey* game_key_buffer = MafiaSDK::GetKeysBuffer();

                int key_idx = 33;
                for (int i = 0; i < 15; i++) {
                    ImGui::Text(car_binds.at(i)); ImGui::NextColumn();
                    const char* key_name = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);

                    ImGui::PushID(key_idx);
                    if (key_name && ImGui::Button(key_name)) {
                        execute_picking(key_idx);
                    }

                    if (!key_name && ImGui::Button("NONE")) {
                        execute_picking(key_idx);
                    }
                    ImGui::PopID();

                    ImGui::NextColumn();

                    if (key_idx < 57) {
                        const char* key_name_ex = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);
                        ImGui::PushID(key_idx);
                        if (key_name_ex && ImGui::Button(key_name_ex)) {
                            execute_picking(key_idx);
                        }

                        if (!key_name_ex && ImGui::Button("NONE")) {
                            execute_picking(key_idx);
                        }
                        ImGui::PopID();
                        ImGui::NextColumn();
                    }
                    else {
                        ImGui::Text(" "); ImGui::NextColumn();
                    }
                }

                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::EndTabItem();

                if (ImGui::Button("Reset to defaults")) {
                    MafiaSDK::GetInput()->ResetDefault(2);
                }
            }

            if (ImGui::BeginTabItem("Misc")) {
                if (ImGui::TreeNode("Aim")) {
                    ImGui::SliderFloat("Sensitivity X", (float*)ADDR_AIM_SENSITIVITY_X, 0.0f, 1.0f);
                    ImGui::SliderFloat("Sensitivity Y", (float*)ADDR_AIM_SENSITIVITY_Y, 0.0f, 1.0f);
                    ImGui::SliderFloat("Aim Speed", (float*)ADDR_AIM_SPEED, 0.0f, 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Steering")) {
                    ImGui::SliderFloat("Unlinear/Linear", (float*)ADDR_STEERING_LINEARITY, 0.0f, 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Other")) {
                    static const char* current_item = NULL;
                    const char* items[] = { "Cross",  "Point", "Cross & Point" };
                    if (ImGui::BeginCombo("Crosshair", current_item)) {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                            bool is_selected = (n == *(BYTE*)(ADDR_CROSSHAIR_TYPE));
                            if (ImGui::Selectable(items[n], is_selected)) {
                                *(BYTE*)(ADDR_CROSSHAIR_TYPE) = (BYTE)n;
                                current_item = items[n];
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Checkbox("Speedometer MPH",     (bool*)ADDR_SPEEDOMETER_TYPE);
                    ImGui::Checkbox("Side Roll",       (bool*)ADDR_SIDE_ROLL);
                    ImGui::Checkbox("Inverse mouse movement",   (bool*)ADDR_MOUSE_CONTROL);
                    ImGui::Checkbox("Enable subtitles",       (bool*)ADDR_ENABLE_SUBTITLES);

                    ImGui::TreePop();
                }

                ImGui::EndTabItem();

                if (ImGui::Button("Reset to defaults")) {
                    *(float*)(ADDR_AIM_SENSITIVITY_X)        = 0.5f;
                    *(float*)(ADDR_AIM_SENSITIVITY_Y)        = 0.5f;
                    *(float*)(ADDR_AIM_SPEED)                = 0.5f;
                    *(float*)(ADDR_STEERING_LINEARITY)       = 0.2f;
                    *(BYTE*)(ADDR_CROSSHAIR_TYPE)            = 0;
                    *(bool*)(ADDR_SPEEDOMETER_TYPE)          = true;
                    *(bool*)(ADDR_SIDE_ROLL)                 = true;
                    *(bool*)(ADDR_MOUSE_CONTROL)             = false;
                    *(bool*)(ADDR_ENABLE_SUBTITLES)          = false;
                }
            }

            ImGui::EndTabBar();
        }
    }

    inline void render_audio_settings() {

        ImGui::Text("Audio");
        if (ImGui::SliderFloat("Sound Effects", (float*)ADDR_SOUNDS_SLIDER, 0.0f, 1.0f)) {
            float new_sound_volume      = *(float*)ADDR_SOUNDS_SLIDER;
            MafiaSDK::GetMission()->GetGame()->SetSoundsVolume(new_sound_volume);
        }

        if (ImGui::SliderFloat("Music", (float*)ADDR_MUSIC_SLIDER, 0.0f, 1.0f)) {
            MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();
        }

        ImGui::SliderFloat("Vehicles", (float*)ADDR_CAR_SLIDER, 0.0f, 1.0f);

        ImGui::SliderFloat("Dialogs", (float*)ADDR_SPEECH_SLIDER, 0.0f, 1.0f);

        if (ImGui::Button("Reset to defaults")) {
            //Sounds, car, music, speech = 1.0f;
            float new_sound_val = 1.0f;
            *(float*)(ADDR_SOUNDS_SLIDER)    = new_sound_val;
            *(float*)(ADDR_CAR_SLIDER)       = new_sound_val;
            *(float*)(ADDR_SPEECH_SLIDER)    = new_sound_val;
            *(float*)(ADDR_MUSIC_SLIDER)     = new_sound_val;
            MafiaSDK::GetMission()->GetGame()->SetSoundsVolume(new_sound_val);
            MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();
        }
    }

    inline void render_game_settings() {
        if (ImGui::BeginTabItem("Input Settings")) {
            render_input_settings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Audio Settings")) {
            render_audio_settings();
            ImGui::EndTabItem();
        }

        if (ImGui::Button("Save")) {
            Profile::generate_profile(Profile::ExtraFields{ qc_address, qc_port });
        } ImGui::SameLine();
    }

    inline void draw_picking_state() {
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        ImGui::Begin("Press new Key",
            nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar);

        ImGui::Text("Use ESC to cancel the operation.\nUse BACKSPACE to clear the key.");

        ImGui::SetWindowSize(ImVec2(433, 70));
        BYTE* engine_dik_buffer = MafiaSDK::GetEngineDikBuffer();
        if (engine_dik_buffer) {
            for (int i = 0; i < 256; i++) {
                if (engine_dik_buffer[i] != old_dik_buffer[i]) {
                    //NOTE(DavoSK): Check if key is not binded somewhere else
                    MafiaSDK::GameKey newToBind(i, MafiaSDK::GameKey_Type::KEYBOARD);
                    auto keys = MafiaSDK::GetKeysBuffer();

                    if (i == DIK_BACKSPACE) {
                        MafiaSDK::GameKey keyNothing(-1, MafiaSDK::GameKey_Type::KEYBOARD);
                        MafiaSDK::GetInput()->BindKey(keyNothing, is_picking_key);
                        keys[is_picking_key] = keyNothing;
                        is_picking_key = -1;
                        input::block_input(true);
                        break;
                    }

                    if (i == DIK_ESCAPE) {
                        is_picking_key = -1;
                        input::block_input(true);
                        break;
                    }

                    int index_to_replace = -1;
                    if (is_picking_key < 33) {
                        for (int j = 0; j < 33; j++) {
                            if (keys[j].dik_key == newToBind.dik_key) {
                                index_to_replace = j;
                                break;
                            }
                        }
                    } else {
                        for (int j = 33; j < 60; j++) {
                            if (keys[j].dik_key == newToBind.dik_key) {
                                index_to_replace = j;
                                break;
                            }
                        }
                    }

                    input::block_input(true);
                    if (index_to_replace != -1) {
                        MafiaSDK::GameKey keyNothing(-1, MafiaSDK::GameKey_Type::KEYBOARD);
                        MafiaSDK::GetInput()->BindKey(keyNothing, index_to_replace);
                        keys[index_to_replace] = keyNothing;
                    }

                    MafiaSDK::GetInput()->BindKey(newToBind, is_picking_key);
                    keys[is_picking_key] = newToBind;
                    is_picking_key = -1;
                    break;
                }
            }
        }
        ImGui::End();
    }

    inline void render() {
        if (is_picking_key == -1) {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Main Menu",
                nullptr,
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse);

            constexpr int padding = 100;
            auto width = MafiaSDK::GetIGraph()->Scrn_sx();
            auto height = MafiaSDK::GetIGraph()->Scrn_sy();
            ImGui::SetWindowSize(ImVec2(width - padding, height - padding));
            ImGui::SetWindowPos(ImVec2(padding / 2, padding / 2));

            if (ImGui::BeginTabBar("blah")) {
                if (ImGui::BeginTabItem("Server Browser")) {
                    ImGui::Columns(4, "mycolumns");
                    ImGui::Text("Name"); ImGui::NextColumn();
                    ImGui::Text("Host"); ImGui::NextColumn();
                    ImGui::Text("Players"); ImGui::NextColumn();
                    ImGui::Text("Port"); ImGui::NextColumn();
                    ImGui::Separator();

                    for (auto server : servers) {
                        if (ImGui::Button(server.server_name.c_str())) {
                            if (::strnicmp(GlobalConfig.username, "ChangeName", 10) == 0 ||
                                ::strlen(GlobalConfig.username) == 0)
                            {
                                modules::infobox::displayError("You need to set your nickname first! See Player tab.");
                            }
                            else
                            {
                                Profile::generate_profile(Profile::ExtraFields{ qc_address, qc_port });
                                ServerInfo::join_server(server);
                            }
                        } ImGui::NextColumn();

                        ImGui::Text("%s", server.server_ip.c_str()); ImGui::NextColumn();
                        ImGui::Text("%s/%s", server.current_players.c_str(), server.max_players.c_str()); ImGui::NextColumn();
                        ImGui::Text("%d", server.port); ImGui::NextColumn();
                    }

                    ImGui::Columns(1);
                    ImGui::Separator();

                    if (ImGui::Button("Refresh")) {
                        generate_browser_list();
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Quick Connect")) {
                    ImGui::InputText("IP", (char*)qc_address, 32);
                    ImGui::InputInt("Port", &qc_port);

                    if (ImGui::Button("Connect")) {
                        if (::strnicmp(GlobalConfig.username, "ChangeName", 10) == 0 ||
                            ::strlen(GlobalConfig.username) == 0)
                        {
                            modules::infobox::displayError("You need to set your nickname first! See Player tab.");
                        }
                        else
                        {
                            ServerInfo::ServerData server = ServerInfo::fetch_server_data(qc_address, qc_port);

                            if (!server.valid) {
                                modules::infobox::displayError("Could not connect to the server!");
                            }
                            else
                            {
                                server.server_ip = std::string(qc_address);
                                server.port = qc_port;
                                Profile::generate_profile(Profile::ExtraFields{ qc_address, qc_port });
                                ServerInfo::join_server(server);
                            }
                        }
                    }  ImGui::SameLine();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Player Settings")) {
                    ImGui::InputText("Nickname", (char*)GlobalConfig.username, 32);
                    ImGui::EndTabItem();
                }

                render_game_settings();
                ImGui::EndTabBar();

                if (ImGui::Button("Quit")) {
                    exit(0);
                }
            }

            ImGui::End();
        } else {
            draw_picking_state();
        }
    }
}
