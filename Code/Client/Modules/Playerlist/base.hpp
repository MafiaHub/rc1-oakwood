namespace playerlist {

    constexpr int VKEY_P = 0x50;

    struct {
        u32 player_count;
        player_scoreboard_info* players_info;
    } current_scoreboard;

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

        for (u32 i = 0; i < current_scoreboard.player_count; i++) {
            auto scoreboard_info = current_scoreboard.players_info[i];
            ImGui::Text("%d", scoreboard_info.server_id); ImGui::NextColumn();
            ImGui::Text("%s", scoreboard_info.nickname); ImGui::NextColumn();
            ImGui::Text("%d", scoreboard_info.ping); ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::End();
    }

    void add_messages() {
        librg_network_add(&network_context, NETWORK_TASK_UPDATE_SCOREBOARD, [](librg_message * msg) {
            current_scoreboard.player_count = librg_data_ru32(msg->data);

            if (current_scoreboard.players_info != nullptr)
                free(current_scoreboard.players_info);

            auto read_size = sizeof(player_scoreboard_info) * current_scoreboard.player_count;
            current_scoreboard.players_info = (player_scoreboard_info*)malloc(read_size);
            librg_data_rptr(msg->data, current_scoreboard.players_info, read_size);
        });
    }
};
