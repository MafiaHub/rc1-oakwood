namespace dialog
{
    bool isOpened = false;

    char *title, *message, *button1, *button2;
    u16 id, type, len1, len2, len3, len4;

    void add_messages()
    {
        librg_network_add(&network_context, NETWORK_DIALOG_OPEN, [](librg_message* msg) {
            len1 = librg_data_ru16(msg->data);
            len2 = librg_data_ru16(msg->data);
            len3 = librg_data_ru16(msg->data);
            len4 = librg_data_ru16(msg->data);
            id = librg_data_ru16(msg->data);
            type = librg_data_ru16(msg->data);

            zpl_string t = zpl_string_make_reserve(zpl_heap(), len1);
            librg_data_rptr(msg->data, t, len1);
            
            title = t;

            zpl_string m = zpl_string_make_reserve(zpl_heap(), len2);
            librg_data_rptr(msg->data, m, len2);

            message = m;
            
            zpl_string b1 = zpl_string_make_reserve(zpl_heap(), len3);
            librg_data_rptr(msg->data, b1, len3);

            button1 = b1;
            
            zpl_string b2 = zpl_string_make_reserve(zpl_heap(), len4);;
            librg_data_rptr(msg->data, b2, len4);

            button2 = b2;
            
            isOpened = true;
        });
    }

    inline const char color_marker_start = '{';
    inline const char color_marker_end = '}';

    inline std::string remove_colors(const std::string text) {
        std::regex r("\\{(\\s*?.*?)*?\\}");
        return std::regex_replace(text, r, "");
    }

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

    inline void draw_colored_text(const char* str)
    {
        char tempStr[4096];

        sprintf(tempStr, str);

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

    inline std::vector<std::string> Split(const std::string& str, int splitLength)
    {
        int NumSubstrings = str.length() / splitLength;
        std::vector<std::string> ret;

        for (auto i = 0; i < NumSubstrings; i++)
        {
            ret.push_back(str.substr(i * splitLength, splitLength));
        }

        // If there are leftover characters, create a shorter item at the end.
        if (str.length() % splitLength != 0)
        {
            ret.push_back(str.substr(splitLength * NumSubstrings));
        }


        return ret;
    }

    std::vector<std::string> Split(const std::string& str, const std::string& delim)
    {
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) pos = str.length();
            std::string token = str.substr(prev, pos - prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }

    inline void formatted_colored_text(const char* fmt, ...) {
        char tempStr[4096];
        
        va_list argPtr;
        va_start(argPtr, fmt);
        _vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
        va_end(argPtr);

        int toSplit = 64;
        std::string buf = "";

        std::vector<std::string> colWords = Split(std::string(tempStr), " ");
        std::vector<std::string> words = Split(remove_colors(std::string(tempStr)), " ");
        std::vector<std::string> lines;

        int size = 0;

        for (int i = 0; i < words.size(); i++)
        {
            int strSize = words[i].size();
            if ((size + strSize) < toSplit)
            {
                size += strSize;
                buf += (colWords[i] + " ");
            }
            else
            {
                buf += (colWords[i] + "\n");
                size = 0;
            }
        }

        draw_colored_text(buf.c_str());
    }

    char response [128] = "";

    bool render()
    {
        if (!isOpened) return false;

        bool canClose = false;

        input::block_input(true);

        ImGui::SetNextWindowPosCenter();
        ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        {
            formatted_colored_text(message);

            switch (type)
            {
            case DIALOG_MSGBOX:
                ImGui::Text("                                                  ");
                if (ImGui::Button(button1, ImVec2(120, 0)))
                {
                    librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 1);
                        librg_data_wptr(&data, "", 128);
                        });
                    input::block_input(false);
                    isOpened = false;
                }
                if (len4 > 1)
                {
                    ImGui::SameLine();
                    if (ImGui::Button(button2, ImVec2(120, 0)))
                    {
                        librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 0);
                        librg_data_wptr(&data, "", 128);
                            });
                        input::block_input(false);
                        isOpened = false;
                    }
                }
                break;

            case DIALOG_INPUT:
                ImGui::SetKeyboardFocusHere(0);
                ImGui::InputText(" ", response, 128, NULL);
                ImGui::Text("   Press ENTER to confirm, press ESC to cancel.   ");

                if (input::is_key_down(VK_RETURN) && isOpened)
                {
                    librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 1);
                        librg_data_wptr(&data, response, 128);
                        });
                    input::block_input(false);
                    isOpened = false;
                    memset(response, 0, sizeof(response));
                }
                else if (input::is_key_down(VK_ESCAPE) && isOpened)
                {
                    librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 0);
                        librg_data_wptr(&data, "", 128);
                        });
                    input::block_input(false);
                    isOpened = false;
                    memset(response, 0, sizeof(response));
                }
                break;

            case DIALOG_PASSWORD:
                ImGui::SetKeyboardFocusHere(0);
                ImGui::InputText(" ", response, 128, ImGuiInputTextFlags_Password);
                ImGui::Text("   Press ENTER to confirm, press ESC to cancel.   ");
                if (input::is_key_down(VK_RETURN) && isOpened)
                {
                    librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 1);
                        librg_data_wptr(&data, response, 128);
                        });
                    input::block_input(false);
                    isOpened = false;
                    memset(response, 0, sizeof(response));
                }
                else if (input::is_key_down(VK_ESCAPE) && isOpened)
                {
                    librg_send(&network_context, NETWORK_DIALOG_DONE, data, {
                        librg_data_wi32(&data, id);
                        librg_data_wi32(&data, 0);
                        librg_data_wptr(&data, "", 128);
                        });
                    input::block_input(false);
                    isOpened = false;
                    memset(response, 0, sizeof(response));
                }
                break;
            }
        }
        ImGui::End();

        return true;
    }
}
