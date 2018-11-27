#pragma once

namespace playerlist {
    
    KeyHeld playerListKey(VK_TAB);

    void render() {

        if (!playerListKey) return;

        ImGui::SetNextWindowPosCenter();
        ImGui::Begin("Mafia Multiplayer - Player list",
            nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar);

        //ImGui::SetWindowPos(ImVec2(20, 20));
        ImGui::Columns(3, "mycolumns");
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Ping"); ImGui::NextColumn();
        ImGui::Separator();

        for (u32 i = 0; i < network_context.max_entities; i++) {

            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (!entity) continue;

            if (entity->type == TYPE_PLAYER && entity->user_data) {
                auto player = reinterpret_cast<mafia_player*>(entity->user_data);
                
                ImGui::Text("%d", entity->id); ImGui::NextColumn();
                ImGui::Text("%s", player->name); ImGui::NextColumn();
                ImGui::Text("%d", player->ping); ImGui::NextColumn();
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::End();
    }
};
