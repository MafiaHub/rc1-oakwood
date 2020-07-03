namespace conndlg
{
    std::string text;

    void render()
    {
        ImGui::SetNextWindowPosCenter();
        ImGui::Begin("Connecting", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        {
            ImGui::SameLine((ImGui::GetWindowWidth() / 2) - ImGui::CalcTextSize(text.c_str()).x / 2);

            ImGui::Text(text.c_str());

            ImGui::Text("                                                                                            ");

            ImGui::SameLine((ImGui::GetWindowWidth() / 2) - 60);

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                switchClientState(ClientState_Browser);
            }
        }
        ImGui::End();
    }
} // namespace infobox
