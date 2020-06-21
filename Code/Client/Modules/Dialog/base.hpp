namespace dialog
{
    #include "messages.hpp"

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

    inline void draw_colored_text(const char* fmt, ...)
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

    char response [128] = "";

    bool render()
    {
        if (!isOpened) return false;

        input::block_input(true);

        ImGui::SetNextWindowPosCenter();
        ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        {
            draw_colored_text(message);

            switch (type)
            {
            case DIALOG_MSGBOX:
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

                if (ImGui::Button(button1, ImVec2(120, 0)) || input::is_key_down(VK_RETURN))
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
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button(button2, ImVec2(120, 0)) || input::is_key_down(VK_ESCAPE))
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
                if (ImGui::Button(button1, ImVec2(120, 0)) || input::is_key_down(VK_RETURN))
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
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button(button2, ImVec2(120, 0)) || input::is_key_down(VK_ESCAPE))
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
