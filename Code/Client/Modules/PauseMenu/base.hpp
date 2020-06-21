namespace pausemenu {
    input::KeyToggle esc_key(VK_ESCAPE);

    inline bool check_input() {
        bool state = !!esc_key;
        if (state) {
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

    inline void render() {
        
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        if (mainmenu::is_picking_key == -1) {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Pause Menu",
                nullptr,
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoCollapse);

            constexpr int padding = 100;
            auto width = MafiaSDK::GetIGraph()->Scrn_sx();
            auto height = MafiaSDK::GetIGraph()->Scrn_sy();
            ImGui::SetWindowSize(ImVec2(width - padding, height - padding));
            ImGui::SetWindowPos(ImVec2(padding / 2, padding / 2));

            if (ImGui::BeginTabBar("escmenu")) {

                if (ImGui::BeginTabItem("Info")) {
                    if (clientActiveState == ClientState_Connected) {
                        colored_text(R"(Welcome to Mafia: {ff0000}Oakwood{ffffff}, you are playing a '{00ff00}%s{ffffff}' version of this modification. 
Please report all your issues on our {8f6eeb}discord {ffffff}server.)", OAK_BUILD_TYPE);
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

                mainmenu::render_game_settings();

                ImGui::EndTabBar();
            }
            
            ImGui::End();
        }
        else {
            mainmenu::draw_picking_state();
        }
    }
};
