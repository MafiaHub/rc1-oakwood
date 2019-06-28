namespace mainmenu {

    S_vector camera_pos = {36.53f, 4.62f, 264.965f};
    S_vector camera_dir = {0.896f, -0.0488f, 0.443f};

    #define MASTER_PULL_TIMEOUT 3.0f
    #define MASTER_RETRIES_MAX 3
    #define GET_TEXT(id) \
        MafiaSDK::GetTextDatabase()->GetText((unsigned int)id)

    struct ServerData {
        std::string server_name;
        std::string server_ip;
        std::string max_players;
        std::string current_players;
        std::string mapname;
        int port;
    };

    struct OakwoodProfileStruct {
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

    enum GameStaticAddresses {
        //float
        AIM_SENSITIVITY_X       = 0x006D4B00,
        AIM_SENSITIVITY_Y       = 0x006D4B04,
        AIM_SPEED               = 0x006D4B08,
        STEERING_LINEARITY      = 0x006D4AEC,

        SOUNDS_SLIDER           = 0x006D4B0C,
        SOUND_GAME_ADDR         = 0x00634B00,
        MUSIC_SLIDER            = 0x006D4B10,
        SPEECH_SLIDER           = 0x006D4B18,

        //byte
        CROSSHAIR_TYPE          = 0x006D4B40,
        SPEEDOMETER_TYPE        = 0x006D4B44,
        SIDE_ROLL               = 0x006D4B45,
        MOUSE_CONTROL           = 0x006D4B46,
        ENABLE_SUBTITLES        = 0x006D4B47,
    };

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

    std::vector<ServerData> servers;
    std::vector<u32> car_names      = { 330, 331, 332, 333, 334, 335, 337, 338, 339, 340, 341, 342, 336, 310, 344 };
    std::vector<u32> player_names   = { 301, 302, 303, 304, 305, 307, 308, 309, 311, 312, 317, 318, 319, 320, 321, 322, 323, 324, 325, 343 };
    bool is_active                  = false;
    int is_picking_key              = -1;
    BYTE old_dik_buffer[256];
    std::string qc_address;
    int qc_port;

    inline void tick() {
        auto game = MafiaSDK::GetMission()->GetGame();
        if (game && is_active) {
            auto cam = game->GetCamera();
            if (cam) {
                cam->LockAt(camera_pos, camera_dir);
            }
        }
    }

    /*
    * HTTP fetch of master server data 
    */
    inline auto fetch_master_server() -> std::string {
        zpl_local_persist f32 fetch_time = 0.0f;
        zpl_local_persist u8 retries_count = 0;
        http_t* request = http_get("http://oakmaster.madaraszd.net/fetch", NULL);
        if (!request) {
            mod_log("[ServerBrowser] Invalid request.\n");
            MessageBoxW(0, L"Please, contact the developers! It could also be a connectivity issue between the server and you, make sure your connection is stable.", L"Invalid request!", MB_OK);
            return "{}";
        }

        fetch_time = zpl_time_now();
        http_status_t status = HTTP_STATUS_PENDING;
        int prev_size = -1;
        while (status == HTTP_STATUS_PENDING) {
            status = http_process(request);
            if (prev_size != (int)request->response_size) {
                prev_size = (int)request->response_size;
            }
            if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT) {
                if (retries_count >= MASTER_RETRIES_MAX) {
                    printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
                    MessageBoxW(0, L"Please, contact the developers! It could also be a connectivity issue between the server and you, make sure your connection is stable.", L"Master server is down!", MB_OK);
                    http_release(request);
                    return "{}";
                }
                retries_count++;
                return fetch_master_server();
            }
        }

        if (status == HTTP_STATUS_FAILED) {
            printf("[ServerBrowser] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
            MessageBoxW(0, L"Master server is down!", L"Please, contact the developers!", MB_OK);
            http_release(request);
            return "{}";
        }

        fetch_time      = 0.0f;
        retries_count   = 0;

        auto to_return = std::string(static_cast<const char*>(request->response_data));
        http_release(request);
        return to_return;
    }

    /*
    * Generates oakwood profile containing all game & multiplayer settings
    */
    inline auto generate_profile() {        
        OakwoodProfileStruct save_struct;
        strcpy((char*)save_struct.username, GlobalConfig.username);

        strcpy((char*)save_struct.address, qc_address.c_str());
        save_struct.port = qc_port;

        memcpy(save_struct.key_bindings, MafiaSDK::GetKeysBuffer(), sizeof(MafiaSDK::GameKey) * 60);    

        save_struct.aim_sensitivity_x = *(float*)(AIM_SENSITIVITY_X);
        save_struct.aim_sensitivity_y = *(float*)(AIM_SENSITIVITY_Y);
        save_struct.aim_speed         = *(float*)(AIM_SPEED);

        save_struct.steering_linearity  = *(float*)(STEERING_LINEARITY);
        save_struct.crosshair_type      = *(BYTE*)(CROSSHAIR_TYPE);
        save_struct.speedometer_type    = *(bool*)(SPEEDOMETER_TYPE);
        save_struct.sideroll            = *(bool*)(SIDE_ROLL);
        save_struct.mouse_control       =  *(bool*)(MOUSE_CONTROL);
        save_struct.enable_subtitles    = *(bool*)(ENABLE_SUBTITLES);


        save_struct.sounds_slider       = *(float*)(SOUNDS_SLIDER);
        save_struct.cars_slider         = *(float*)(SOUND_GAME_ADDR);
        save_struct.music_slider        = *(float*)(MUSIC_SLIDER);
        save_struct.speech_slider       = *(float*)(SPEECH_SLIDER);

        std::ofstream save_file("OakwoodProfile.data", std::ios::binary);
        save_file.write((const char*)& save_struct, sizeof(OakwoodProfileStruct));
    }

    /*
    * Load all settings from file 
    */
    inline auto load_profile() -> void {
        std::ifstream load_file("OakwoodProfile.data", std::ios::binary);
        if (load_file.good()) {
            OakwoodProfileStruct save_struct;
            load_file.read((char*)&save_struct, sizeof(OakwoodProfileStruct));
            
            strcpy(GlobalConfig.username, (char*)save_struct.username);
            strcpy(GlobalConfig.server_address, (char*)save_struct.address);
            qc_address = std::string(GlobalConfig.server_address);

            if (!strlen(GlobalConfig.username)) {
                strcpy(GlobalConfig.username, "ChangeName");
            }

            GlobalConfig.port = save_struct.port;
            qc_port = save_struct.port;

            auto game_key_buffer = MafiaSDK::GetKeysBuffer();
            memcpy(game_key_buffer, save_struct.key_bindings, sizeof(MafiaSDK::GameKey) * 60);
            for (int i = 0; i < 60; i++) {
                MafiaSDK::GetInput()->BindKey(*game_key_buffer, i);
                game_key_buffer++;
            }

            *(float*)(AIM_SENSITIVITY_X) = save_struct.aim_sensitivity_x;
            *(float*)(AIM_SENSITIVITY_Y) = save_struct.aim_sensitivity_y;
            *(float*)(AIM_SPEED) = save_struct.aim_speed;

            *(float*)(STEERING_LINEARITY)   = save_struct.steering_linearity;
            *(BYTE*)(CROSSHAIR_TYPE)        = save_struct.crosshair_type;
            *(bool*)(SPEEDOMETER_TYPE)  = save_struct.speedometer_type;
            *(bool*)(SIDE_ROLL)         = save_struct.sideroll;
            *(bool*)(MOUSE_CONTROL)     = save_struct.mouse_control;
            *(bool*)(ENABLE_SUBTITLES)  = save_struct.enable_subtitles;


            *(float*)(SOUNDS_SLIDER)    = save_struct.sounds_slider;
            *(float*)(SOUND_GAME_ADDR)  = save_struct.cars_slider;
            *(float*)(MUSIC_SLIDER)     = save_struct.music_slider;
            *(float*)(SPEECH_SLIDER)    = save_struct.speech_slider;
        }
    }

    /*
    * Parse json response from master server 
    */
    inline auto generate_browser_list() -> void {

        servers.clear();
        auto fetched_list = fetch_master_server();
        if (!fetched_list.empty()) {
            u8 failed = 0;
            zpl_string json_config_data = zpl_string_make(zpl_heap(), fetched_list.c_str());
            zpl_json_object json_master_data = { 0 };
            zpl_json_parse(&json_master_data, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &failed);

            if (!failed) {
                zpl_json_object* server_property;
                zpl_json_find(&json_master_data, "server", false, &server_property);

                if (!server_property)
                    return;

                for (i32 i = 0; i < zpl_array_count(server_property->nodes); ++i) {
                    zpl_json_object* server_node = (server_property->nodes + i);
                    zpl_json_object * server_property;
                    ServerData new_server_data;

                    zpl_json_find(server_node, "name", false, &server_property);
                    new_server_data.server_name = std::string(server_property->string);

                    zpl_json_find(server_node, "host", false, &server_property);
                    new_server_data.server_ip = std::string(server_property->string);

                    zpl_json_find(server_node, "maxPlayers", false, &server_property);
                    new_server_data.max_players = std::to_string(server_property->integer);

                    zpl_json_find(server_node, "players", false, &server_property);
                    new_server_data.current_players = std::to_string(server_property->integer);

                    zpl_json_find(server_node, "port", false, &server_property);
                    new_server_data.port = (int)std::atoi(server_property->string);

                    zpl_json_find(server_node, "mapname", false, &server_property);
                    new_server_data.mapname = std::string(server_property->string);

                    servers.push_back(new_server_data);
                }
            }
        }
    }

    inline void init() {
        load_profile();
        fetch_master_server();
        generate_browser_list();
        input::block_input(true);
        is_active = true;
    }

    /*
    * Exit main menu by joining an specific server
    */
    inline void join_server(ServerData server) {

        if (::strcmp(GlobalConfig.username, "ChangeMe") == 0 ||
            ::strlen(GlobalConfig.username) == 0)
        {
            MessageBox(NULL, "You need to set your nickname first! See Player tab.", "Change your nickname", MB_OK);
            return;
        }

        //NOTE(DavoSK): Set global config server address & port
        strcpy(GlobalConfig.server_address, server.server_ip.c_str());
        GlobalConfig.port = server.port;

        input::block_input(false);
        is_active = false;
        generate_profile(); // save current settings

        MafiaSDK::GetMission()->MapLoad(server.mapname.c_str());
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
            if (ImGui::BeginTabItem(GET_TEXT(TEXT_PLAYER))) {
                ImGui::Columns(3, "mycolumns");
                ImGui::Text(GET_TEXT(TEXT_CONTROLS)); ImGui::NextColumn();
                ImGui::Text(GET_TEXT(TEXT_PRIMARY)); ImGui::NextColumn();
                ImGui::Text(GET_TEXT(TEXT_SECONDARY)); ImGui::NextColumn();
                ImGui::Separator();

                MafiaSDK::GameKey* game_key_buffer = MafiaSDK::GetKeysBuffer();
                int key_idx = 0;
                for (int i = 0; i < 20; i++) {
                    ImGui::Text(GET_TEXT(player_names.at(i))); ImGui::NextColumn();
                    const char* key_name = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);

                    ImGui::PushID(key_idx);
                    if (key_name && ImGui::Button(key_name)) {
                        execute_picking(key_idx);
                    }
                    if (!key_name && ImGui::Button("EMPTY")) {
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
                        if (!key_name_next && ImGui::Button("EMPTY")) {
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

                if (ImGui::Button(GET_TEXT(TEXT_RESET_TO_DEFAULT))) {
                    MafiaSDK::GetInput()->ResetDefault(1);
                }
            }

            if (ImGui::BeginTabItem(GET_TEXT(TEXT_CAR))) {
                ImGui::Columns(3, "mycolumns");
                ImGui::Text(GET_TEXT(TEXT_CONTROLS)); ImGui::NextColumn();
                ImGui::Text(GET_TEXT(TEXT_PRIMARY)); ImGui::NextColumn();
                ImGui::Text(GET_TEXT(TEXT_SECONDARY)); ImGui::NextColumn();
                ImGui::Separator();

                MafiaSDK::GameKey* game_key_buffer = MafiaSDK::GetKeysBuffer();
                int key_idx = 33;
                for (int i = 0; i < 15; i++) {
                    ImGui::Text(GET_TEXT(car_names.at(i))); ImGui::NextColumn();
                    const char* key_name = MafiaSDK::GetInput()->GetKeyName(&game_key_buffer[key_idx++]);

                    ImGui::PushID(key_idx);
                    if (key_name && ImGui::Button(key_name)) {
                        execute_picking(key_idx);
                    }

                    if (!key_name && ImGui::Button("EMPTY")) {
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

                        if (!key_name_ex && ImGui::Button("EMPTY")) {
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

                if (ImGui::Button(GET_TEXT(TEXT_RESET_TO_DEFAULT))) {
                    MafiaSDK::GetInput()->ResetDefault(2);
                }
            }

            if (ImGui::BeginTabItem(GET_TEXT(TEXT_OTHERS))) {
                if (ImGui::TreeNode(GET_TEXT(TEXT_AIMING))) {
                    ImGui::SliderFloat(GET_TEXT(TEXT_SENSITIVITY_X), (float*)AIM_SENSITIVITY_X, 0.0f, 1.0f);
                    ImGui::SliderFloat(GET_TEXT(TEXT_SENSITIVITY_Y), (float*)AIM_SENSITIVITY_Y, 0.0f, 1.0f);
                    ImGui::SliderFloat(GET_TEXT(TEXT_AIM_SPEED), (float*)AIM_SPEED, 0.0f, 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(GET_TEXT(TEXT_STEERING))) {
                    ImGui::SliderFloat(GET_TEXT(TEXT_STEERING_LINEARITY), (float*)STEERING_LINEARITY, 0.0f, 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(GET_TEXT(TEXT_NEXT_OTHER))) {
                    static const char* current_item = NULL;
                    const char* items[] = { GET_TEXT(TEXT_CROSSHAIR),  GET_TEXT(TEXT_CENTER_POINT), GET_TEXT(TEXT_CROSSHAIR_CENTER_POINT) };
                    if (ImGui::BeginCombo(GET_TEXT(233), current_item)) {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                            bool is_selected = (n == *(BYTE*)(CROSSHAIR_TYPE));
                            if (ImGui::Selectable(items[n], is_selected)) {
                                *(BYTE*)(CROSSHAIR_TYPE) = (BYTE)n;
                                current_item = items[n];
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Checkbox(GET_TEXT(TEXT_SPEEDOMETER),     (bool*)SPEEDOMETER_TYPE);
                    ImGui::Checkbox(GET_TEXT(TEXT_SIDE_ROLL),       (bool*)SIDE_ROLL);
                    ImGui::Checkbox(GET_TEXT(TEXT_MOUSE_CONTROL),   (bool*)MOUSE_CONTROL);
                    ImGui::Checkbox(GET_TEXT(TEXT_SUBTITLES),       (bool*)ENABLE_SUBTITLES);

                    ImGui::TreePop();
                }

                ImGui::EndTabItem();

                if (ImGui::Button(GET_TEXT(TEXT_RESET_TO_DEFAULT))) {
                    *(float*)(AIM_SENSITIVITY_X)        = 0.5f;
                    *(float*)(AIM_SENSITIVITY_Y)        = 0.5f;
                    *(float*)(AIM_SPEED)                = 0.5f;
                    *(float*)(STEERING_LINEARITY)       = 0.2f;
                    *(BYTE*)(CROSSHAIR_TYPE)            = 0;
                    *(bool*)(SPEEDOMETER_TYPE)          = true;
                    *(bool*)(SIDE_ROLL)                 = true;
                    *(bool*)(MOUSE_CONTROL)             = false;
                    *(bool*)(ENABLE_SUBTITLES)          = false;
                }
            }
 
            ImGui::EndTabBar();
        }
    }

    inline void render_audio_settings() {
        
        ImGui::Text(GET_TEXT(TEXT_AUDIO));
        if (ImGui::SliderFloat(GET_TEXT(TEXT_SOUNDS), (float*)SOUNDS_SLIDER, 0.0f, 1.0f)) {
            float new_sound_volume      = *(float*)SOUNDS_SLIDER;
            *(float*)(SOUND_GAME_ADDR)  = new_sound_volume;
            MafiaSDK::GetMission()->GetGame()->SetSoundsVolume(new_sound_volume);
        }

        if (ImGui::SliderFloat(GET_TEXT(TEXT_MUSIC), (float*)MUSIC_SLIDER, 0.0f, 1.0f)) {
            MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();
        }
        
        ImGui::SliderFloat(GET_TEXT(TEXT_CARS), (float*)SOUND_GAME_ADDR, 0.0f, 1.0f);
        ImGui::SliderFloat(GET_TEXT(TEXT_SPEECH), (float*)SPEECH_SLIDER, 0.0f, 1.0f);

        if (ImGui::Button(GET_TEXT(TEXT_RESET_TO_DEFAULT))) {
            //Sounds, car, music, speech = 1.0f;
            float new_sound_val = 1.0f;
            *(float*)(SOUNDS_SLIDER)    = new_sound_val;
            *(float*)(SOUND_GAME_ADDR)  = new_sound_val;
            *(float*)(SPEECH_SLIDER)    = new_sound_val;
            *(float*)(MUSIC_SLIDER)     = new_sound_val;
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
            generate_profile();
        } ImGui::SameLine();
    }

    inline void draw_picking_state() {
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        ImGui::Begin(GET_TEXT(181),
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
        if (is_active) {
            if (is_picking_key == -1) {
                ImGui::SetNextWindowPosCenter();
                ImGui::Begin("Mafia Oakwood",
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
                                join_server(server);
                            } ImGui::NextColumn();

                            ImGui::Text("%s", server.server_ip.c_str()); ImGui::NextColumn();
                            ImGui::Text("%s/%s", server.current_players.c_str(), server.max_players.c_str()); ImGui::NextColumn();
                            ImGui::Text("%d", server.port); ImGui::NextColumn();
                        }

                        ImGui::Columns(1);
                        ImGui::Separator();

                        if (ImGui::Button("Refresh")) {
                            fetch_master_server();
                            generate_browser_list();
                        }

                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Quick Connect")) {
                        ImGui::InputText("IP", (char*)qc_address.c_str(), 32);
                        ImGui::InputInt("Port", &qc_port);
                       
                        if (ImGui::Button("Connect")) {
                            ServerData server = { "Dummy", qc_address.c_str(), "", "", "freeride", qc_port };
                            join_server(server);
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
}
