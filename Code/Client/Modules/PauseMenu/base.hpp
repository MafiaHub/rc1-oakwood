namespace pausemenu {
    input::KeyToggle esc_key(VK_ESCAPE);

    inline bool check_input() {
        bool state = !!esc_key;
        if (state) {
            if (menuActiveState == Menu_Pause) {
                menuActiveState = Menu_Chat;
                input::block_input(false);
                modules::chat::is_focused = false;
                return false;
            } else input::block_input(true);
            return true;
        }
        return false;
    }

    inline void render() {
        
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        if (mainmenu::is_picking_key == -1) {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Mafia: Oakwood - Pause Menu",
                nullptr,
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoCollapse);

            constexpr int padding = 100;
            auto width = MafiaSDK::GetIGraph()->Scrn_sx();
            auto height = MafiaSDK::GetIGraph()->Scrn_sy();
            ImGui::SetWindowSize(ImVec2(width - padding, height - padding));
            ImGui::SetWindowPos(ImVec2(padding / 2, padding / 2));

            if (ImGui::BeginTabBar("escmenu")) {

                if (ImGui::BeginTabItem("Info")) {
                    if (clientActiveState == ClientState_Connected) {
                        ImGui::Text(R"(Welcome to Mafia: Oakwood, you are playing a '%s' version of this modification. 
Please report all your issues on our discord server.)", OAK_BUILD_TYPE);
                        ImGui::Text("Current build: v%s", OAK_VERSION);
                        ImGui::Text("Current build date: %s %s", OAK_BUILD_DATE, OAK_BUILD_TIME);
                        ImGui::Text("Current server IP: %s", GlobalConfig.server_address);
                    } else {
                        ImGui::Text("You aren't connected to any server right now! :(");
                    }

                    if (ImGui::Button("Exit to menu")) {
                        if (clientActiveState == ClientState_Connected) {
                            librg_network_stop(&network_context);
                        }
                    }  ImGui::SameLine();

                    if (ImGui::Button("Quit")) {
                        librg_network_stop(&network_context);
                        exit(0);
                    }

                    ImGui::EndTabItem();
                }

                mainmenu::render_game_settings();

                ImGui::EndTabBar();
            }
            
            ImGui::End();
        }
        else {
            mainmenu::draw_picking_state();
        }
    }
};
