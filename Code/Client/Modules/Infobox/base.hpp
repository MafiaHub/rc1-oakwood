namespace infobox
{

std::string error = "Undefined";
std::function<void()> cb;

void render()
{
    ImGui::SetNextWindowPosCenter();
    ImGui::Begin("Infobox", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    {
        ImGui::Text(error.c_str());

        ImGui::Text("                                                                                            ");

        ImGui::SameLine((ImGui::GetWindowWidth() / 2) - 60);

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            switchClientState(ClientState_Browser);
        }

        if (cb)
            cb();
    }
    ImGui::End();
}

void displayError(const std::string &message, std::function<void()> _cb)
{
    error = message;

    auto missionName = std::string(MafiaSDK::GetCurrentMissionName());

    if (missionName != "tutorial")
        MafiaSDK::GetMission()->MapLoad("tutorial");

    switchClientState(ClientState_Infobox);
    input::block_input(true);
    cb = _cb;
}

} // namespace infobox
