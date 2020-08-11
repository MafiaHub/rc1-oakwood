namespace pausemenu {
    input::KeyToggle esc_key(VK_ESCAPE);

    inline bool check_input() {
        bool state = !!esc_key;
        if (state && menuActiveState != Menu_Dialog) {
            if (menuActiveState == Menu_Pause) {
                menuActiveState = Menu_Chat;
                input::block_input(false);
                modules::chat::is_focused = false;
                return false;
            } else input::block_input(true);
            return true;
        }
        return false;
    }

    inline const char color_marker_start = '{';
    inline const char color_marker_end = '}';

    inline bool process_inline_hex_color(const char* start, const char* end, ImVec4& color)
    {
        const int hexCount = (int)(end - start);
        if (hexCount == 6 || hexCount == 8)
        {
            char hex[9];
            strncpy(hex, start, hexCount);
            hex[hexCount] = 0;

            unsigned int hexColor = 0;
            if (sscanf(hex, "%x", &hexColor) > 0)
            {
                color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
                color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
                color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
                color.w = 1.0f;

                if (hexCount == 8)
                {
                    color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
                }

                return true;
            }
        }

        return false;
    }

    inline void colored_text(const char* fmt, ...)
    {
        char tempStr[4096];

        va_list argPtr;
        va_start(argPtr, fmt);
        _vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
        va_end(argPtr);
        tempStr[sizeof(tempStr) - 1] = '\0';

        bool pushedColorStyle = false;
        const char* textStart = tempStr;
        const char* textCur = tempStr;
        while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
        {
            if (*textCur == color_marker_start)
            {
                // Print accumulated text
                if (textCur != textStart)
                {
                    ImGui::TextUnformatted(textStart, textCur);
                    ImGui::SameLine(0.0f, 0.0f);
                }

                // Process color code
                const char* colorStart = textCur + 1;
                do
                {
                    ++textCur;
                } while (*textCur != '\0' && *textCur != color_marker_end);

                // Change color
                if (pushedColorStyle)
                {
                    ImGui::PopStyleColor();
                    pushedColorStyle = false;
                }

                ImVec4 textColor;
                if (process_inline_hex_color(colorStart, textCur, textColor))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                    pushedColorStyle = true;
                }

                textStart = textCur + 1;
            }
            else if (*textCur == '\n')
            {
                // Print accumulated text an go to next line
                ImGui::TextUnformatted(textStart, textCur);
                textStart = textCur + 1;
            }

            ++textCur;
        }

        if (textCur != textStart)
        {
            ImGui::TextUnformatted(textStart, textCur);
        }
        else
        {
            ImGui::NewLine();
        }

        if (pushedColorStyle)
        {
            ImGui::PopStyleColor();
        }
    }

    char title[32] = "";
    char message[512] = "";

    std::string errMsg = "";

    HANDLE myProcess = NULL;
    DWORD exCode = NULL;

    bool failedToStart = false;

    inline void render_report()
    {
        ImGui::InputText("Title", title, 32);
        ImGui::InputTextMultiline("Message", message, 512, ImVec2(0, 400));

        if (myProcess == NULL)
        {
            if (ImGui::Button("Send"))
            {
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                std::string server;

                if (clientActiveState == ClientState_Browser)
                {
                    server = "ServerBrowser";
                }
                else
                {
                    server = std::string(GlobalConfig.server_address) + ":" + std::to_string(GlobalConfig.port);
                }

                std::string t(title);
                std::string m(message);

                replaceAll(t, " ", "{sp}");
                replaceAll(m, " ", "{sp}");
                replaceAll(m, "\n", "{br}");

                if (!CreateProcessA(NULL, (LPSTR)(GlobalConfig.localpath + "\\bin\\BugReporter.exe" + " " + GlobalConfig.username + "|" + server + "|" + t + "|" + m).c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) failedToStart = true;
                myProcess = pi.hProcess;
                if (failedToStart)
                {
                    errMsg = "Unable to start process";
                    myProcess = NULL;
                    exCode = NULL;
                }
                else
                {
                    errMsg = "Sending report...";
                }
            }
        }
        else
        {
            GetExitCodeProcess(myProcess, &exCode);

            if (exCode == STILL_ACTIVE)
            {
                errMsg = "Sending report...";
            }
            else
            {
                if (!exCode)
                {
                    errMsg = "Report successfully sent!";
                    myProcess = NULL;
                    exCode = NULL;
                }
                else
                {
                    errMsg = "Failed to send report";
                    myProcess = NULL;
                    exCode = NULL;
                }
            }
        }

        if (errMsg != "") ImGui::Text("%s", errMsg.c_str());
    }

    inline void render() {
        
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        if (mainmenu::is_picking_key == -1) {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Pause Menu",
                nullptr,
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize);

            constexpr int padding = 100;
            auto width = MafiaSDK::GetIGraph()->Scrn_sx();
            auto height = MafiaSDK::GetIGraph()->Scrn_sy();
            ImGui::SetWindowSize(ImVec2(width - padding, height - padding));
            ImGui::SetWindowPos(ImVec2(padding / 2, padding / 2));

            if (ImGui::BeginTabBar("escmenu")) {

                if (ImGui::BeginTabItem("Info")) {
                    if(errMsg.size() > 0) errMsg = "";

                    if(myProcess) myProcess = NULL;
                    if(exCode) exCode = NULL;

                    if (strlen(title) > 0) memset(title, 0, 32);
                    if (strlen(message) > 0) memset(title, 0, 512);

                    if(failedToStart) failedToStart = false;

                    if (clientActiveState == ClientState_Connected) {
                        colored_text(R"(Welcome to Mafia: {ff0000}Oakwood{ffffff}, you are playing a '{00ff00}%s{ffffff}' version of this modification. 
Please report all your issues using the {00ff00}Report {ffffff}tab or on our {8f6eeb}discord {ffffff}server.)", OAK_BUILD_TYPE);
                        colored_text("Current server IP: {00ff00}%s:%d", GlobalConfig.server_address, GlobalConfig.port);
                    } else {
                        colored_text("You {ff0000}aren't {ffffff}connected to any server right now! :(");
                    }

                    if (ImGui::Button("Exit to menu")) {
                        if (clientActiveState == ClientState_Connected) {
                            librg_network_stop(&network_context);
                        }
                    }  ImGui::SameLine();

                    if (ImGui::Button("Quit")) {
                        librg_network_stop(&network_context);
                        exit(0);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Settings"))
                {
                    if (errMsg.size() > 0) errMsg = "";

                    if (myProcess) myProcess = NULL;
                    if (exCode) exCode = NULL;

                    if (strlen(title) > 0) memset(title, 0, 32);
                    if (strlen(message) > 0) memset(title, 0, 512);

                    if (failedToStart) failedToStart = false;

                    mainmenu::render_game_settings();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Report"))
                {
                    render_report();
                    ImGui::EndTabItem();
                }
                
                ImGui::EndTabBar();
            }
            
            ImGui::End();
        }
        else {
            mainmenu::draw_picking_state();
        }
    }
};
