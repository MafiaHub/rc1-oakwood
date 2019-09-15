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
    ImGui::Begin("Password", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    {
        ImGui::Text("Enter the password to access this server:");

        ImGui::InputText("password", GlobalConfig.password, 32, ImGuiInputTextFlags_Password);

        if (ImGui::Button("Connect"))
        {
            ServerInfo::join_server(server);
        }
    }
    ImGui::End();
}

} // namespace passwordPrompt
