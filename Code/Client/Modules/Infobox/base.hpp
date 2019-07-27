namespace infobox
{

std::string error = "Undefined";
std::function<void()> cb;

void render()
{
    ImGui::SetNextWindowPosCenter();
    ImGui::Begin("Infobox", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    {
        ImGui::Text(error.c_str());

        if (ImGui::Button("OK"))
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
