namespace pausemenu {
    input::KeyToggle esc_key(VK_ESCAPE);
    bool is_enabled = false;

    inline void init() {

    }

    inline void render() {
        if (esc_key) {
            is_enabled = !is_enabled;
            input::block_input(is_enabled);
        }

        if (is_enabled) {
         
            ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
            ImGui::Begin("Pause Menu",
                nullptr,
                ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_NoScrollbar);

            ImGui::SetWindowSize(ImVec2(400, 300));
          
            ImGui::Text(R"(Welcome to Mafia: Oakwood 
                           your are playing on %d version of modification.  
                           Please report all your issues on our discord channel)", OAK_BUILD_CHANNEL);

            ImGui::Text("Current build: %d", OAK_BUILD_VERSION);
            ImGui::Text("Current build date: %s", OAK_BUILD_TIME);
            ImGui::Text("Current server IP: %s", GlobalConfig.server_address);
          
            //ImGui::SameLine();

            if (ImGui::Button("Quit")) {
                librg_network_stop(&network_context);
                exit(0);
            }
            
            ImGui::End();
        }
    }
};
