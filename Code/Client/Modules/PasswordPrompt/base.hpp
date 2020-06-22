namespace passwordPrompt
{

ServerInfo::ServerData server;

void init(ServerInfo::ServerData data) {
    server = data;

    switchClientState(ClientState_PasswordPrompt);
    strcpy(GlobalConfig.password, "");
}

void render()
{
    ImGui::SetNextWindowPosCenter();
    ImGui::Begin("Protected Access", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    {
        ImGui::Text("Enter the password to access this server:");

        ImGui::SetKeyboardFocusHere(0);
        ImGui::InputText(" ", GlobalConfig.password, 128, ImGuiInputTextFlags_Password);
        ImGui::Text("   Press ENTER to confirm, press ESC to cancel.   ");
        if (input::is_key_down(VK_RETURN))
        {
            ServerInfo::join_server(server);
        }
        else if (input::is_key_down(VK_ESCAPE))
        {
            memset(GlobalConfig.password, 0, sizeof(GlobalConfig.password));
            switchClientState(ClientState_Browser);
        }
    }
    ImGui::End();
}

} // namespace passwordPrompt
