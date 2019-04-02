namespace pausemenu {
    input::KeyToggle esc_key(VK_ESCAPE);

    inline bool check_input() {
        bool state = !!esc_key;
        if (state && librg_is_connected(&network_context)) {
            if (menuActiveState == Menu_Pause) {
                menuActiveState = Menu_Chat;
                input::block_input(false);
                return false;
            }
            else {
                input::block_input(true);
            }

            return true;
        }
        return false;
    }

    inline void render() {
        if (librg_is_connected(&network_context)) { 
            ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
            ImGui::Begin("Pause Menu",
                nullptr,
                ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_NoScrollbar);

            ImGui::SetWindowSize(ImVec2(433, 167));
          
            ImGui::Text(R"(Welcome to Mafia: Oakwood,
    you are playing a '%s' version of this modification.  
    Please report all your issues on our discord server.)", oak_build_channel[OAK_BUILD_CHANNEL]);

            ImGui::Text("Current build: %s (%x)", OAK_BUILD_VERSION_STR, OAK_BUILD_VERSION);
            ImGui::Text("Current build date: %s %s", OAK_BUILD_DATE, OAK_BUILD_TIME);
            ImGui::Text("Current server IP: %s", GlobalConfig.server_address);
        }
        else {
            ImGui::Text("You aren't connected to any server right now! :(");
        }
        
        if (ImGui::Button("Quit")) {
            librg_network_stop(&network_context);
            exit(0);
        }
        
        ImGui::End();
    }
};
