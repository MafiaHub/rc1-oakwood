namespace playerlist {
    constexpr int VKEY_P = 0x50;

    void render() {
        if (!input::is_key_down(VKEY_P) || modules::chat::is_focused) 
            return;

        ImGui::SetNextWindowPosCenter();
        ImGui::SetNextWindowSizeConstraints({ 230, -1 }, { 230, -1 });
        ImGui::Begin("Mafia Oakwood - Player list",
            nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Columns(3, "mycolumns");
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Ping"); ImGui::NextColumn();
        ImGui::Separator();

        for (u32 i = 0; i < network_context.max_entities; i++) {
            librg_entity* entity = librg_entity_fetch(&network_context, i);
            if (!entity) continue;

            if (entity->type == TYPE_PLAYER && entity->user_data) {
                auto player = reinterpret_cast<mafia_player*>(entity->user_data);
                ImGui::Text("%d", i); ImGui::NextColumn();
                ImGui::Text("%s", player->name); ImGui::NextColumn();
                ImGui::Text("%d", player->ping); ImGui::NextColumn();
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::End();
        
    }
};
