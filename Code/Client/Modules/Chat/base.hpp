namespace chat {

    /* command processing section */
    struct ChatCommand {
        std::string command_name;
        std::function<void(std::vector<std::string>)> command_ptr;
    };

    std::vector<ChatCommand> chat_commands;
    std::vector<std::pair<ImVec4, std::string>> chat_messages;
    std::vector<std::string> chat_history;

    i64 chat_history_index;
    unsigned int chat_current_msg;
    constexpr unsigned int VK_T = 0x54;
    bool is_focused = false;
    bool new_msg_arrived = false;

    /* keys definitions */
    input::KeyToggle key_chat_open(VK_T);
    input::KeyToggle key_chat_send(VK_RETURN);
    input::KeyToggle key_chat_history_prev(VK_UP);
    input::KeyToggle key_chat_history_next(VK_DOWN);
    input::KeyToggle key_chat_caret_prev(VK_LEFT);
    input::KeyToggle key_chat_caret_next(VK_RIGHT);

    /* history */
    static char add_text[OAK_MAX_CHAT] = { 0 };
    static char backup_text[OAK_MAX_CHAT] = { 0 };

    /* methods */
    inline void add_debug(const std::string& msg) {
        chat_messages.push_back({
            ImColor(255, 255, 255, 255),
            "Debug: " + msg
            });
    }

    inline void clear_messages() {
        chat_messages.clear();
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

    inline std::string remove_colors(const std::string text) {
        std::regex r("\\{(\\s*?.*?)*?\\}");
        return std::regex_replace(text, r, "");
    }

    template < typename T>
    std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T& element)
    {
        std::pair<bool, int > result;
        // Find given element in vector
        auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);
        if (it != vecOfElements.end())
        {
            result.second = distance(vecOfElements.begin(), it);
            result.first = true;
        }
        else
        {
            result.first = false;
            result.second = -1;
        }
        return result;
    }

    inline void add_message(const std::string& msg) {
        int toSplit = 70;
        std::string buf = "";
        
        std::vector<std::string> colWords = Split(msg, " ");
        std::vector<std::string> words = Split(remove_colors(msg), " ");
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
                std::pair<bool, int> result = findInVector<std::string>(words, words[i]);

                if (result.first && result.second == words.size() - 1)
                {
                    std::vector<std::string> longToSplit = Split(remove_colors(colWords[i]), 28);
                    for (auto str : longToSplit)
                    {
                        buf += (colWords[i] + "\n");
                        size = 0;
                    }
                }
                else
                {
                    buf += (colWords[i] + "\n");
                    size = 0;
                }
            }
        }

        chat_messages.push_back({ ImColor(255, 255, 255, 255), buf });

        new_msg_arrived = true;
    }

    inline void register_command(const std::string& name, std::function<void(std::vector<std::string>)> ptr) {
        if (ptr != nullptr) {
            chat_commands.push_back({ name, ptr });
        }
    }

    inline auto get_vector_of_args(std::string command_str) {
        return split(command_str, " ");
    }

    inline bool parse_command(std::string command_str) {
        if (!command_str.empty()) {
            for (auto command : chat_commands) {
                if (command_str.find(command.command_name) != std::string::npos) {
                    if (command.command_ptr != nullptr) {
                        command.command_ptr(get_vector_of_args(command_str));
                        return true;
                    }
                }
            }
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

    int yPos = 1;
    int ySize = 400;

    int vehYPos = 265;
    int vehYSize = 260;

    int plYPos = 1;
    int plYSize = 400;

    inline void init() {
        zpl_local_persist bool itpl = false;

        if (itpl) {
            return;
        }

        itpl = true;

        vehYPos = (MafiaSDK::GetIGraph()->Scrn_sy() / 2) - 95;

        register_command("/q", [&](std::vector<std::string> args) {
            librg_network_stop(&network_context);
            ExitProcess(ERROR_SUCCESS);
        });

        register_command("/crash", [&](std::vector<std::string> args) {
            *(int*)0 = 42;
        });

        register_command("/ver", [&](std::vector<std::string> args) {
#if _DEBUG == 1
            const char* wat = "{aaaaaa}({ffff00}Development Build{aaaaaa})";
#elif OAK_BUILD_CHANNEL == 4
            const char* wat = "{aaaaaa}({ffff00}Pre-Release Build{aaaaaa})";
#else
            const char* wat = "";
#endif
            char msg[128];
            char msg2[128];
            char msg3[128];
            sprintf(msg, "{ffffff}Mafia: {ff0000}Oakwood {00ff00}v%s %s", OAK_VERSION, wat);
            sprintf(msg2, "{ffffff}Build date: {00ff00}%s", __DATE__);
            sprintf(msg3, "{ffffff}Build time: {00ff00}%s", __TIME__);
            add_message(std::string(msg));
            add_message(std::string(msg2));
            add_message(std::string(msg3));
        });

        register_command("/savepos", [&](std::vector<std::string> args) {
            auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();

            if (!local_player) return;

            std::ofstream pos_file("positions.txt");
            auto pos = local_player->GetInterface()->humanObject.entity.position;
            auto dir = local_player->GetInterface()->humanObject.entity.rotation;
            zpl_vec3 position = EXPAND_VEC(pos);
            zpl_vec3 direction = EXPAND_VEC(dir);
            auto rot = DirToRotation180(direction);

            pos_file << position.x << " " << position.y << " " << position.z << ", " << rot << std::endl;
        });
    }

    void input_text_replace(ImGuiInputTextCallbackData *data, const char *str) {
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, str);
    }

    int inputTextHandler(ImGuiInputTextCallbackData *data) {
        b32 hist_prev = key_chat_history_prev;
        b32 hist_next = key_chat_history_next;

        if (hist_prev || hist_next) {
            if (!chat_history.empty()) {
                if (chat_history_index == 0 && hist_next) {
                    input_text_replace(data, backup_text);
                } else {
                    if (hist_prev && chat_history_index != chat_history.size() - 1) {
                        if (chat_history_index == -1)
                            strcpy(backup_text, data->Buf);
                        
                        chat_history_index++;
                    } else if (hist_next && chat_history_index > 0)
                        chat_history_index--;

                    if (chat_history_index < 0)
                        chat_history_index = 0;

                    input_text_replace(data, chat_history[chat_history_index].c_str());
                }
            }
        }
        return FALSE;
    }

    inline bool check_input() {
        // Chat is always enabled by other means...
        return false;
    }

    inline void set_chat_y(int pos)
    {
        yPos = pos;
        new_msg_arrived = true;
    }

    inline void set_chat_ysize(int size)
    {
        ySize = size;
        new_msg_arrived = true;
    }

    inline void render() {
        if (key_chat_open && !is_focused) {
            is_focused = true;
            input::block_input(is_focused);
        }

        bool chat_send = key_chat_send;

        if (!MafiaSDK::GetMission()->GetGame() || input::is_key_down(VK_TAB)) 
            return;

        ImGui::Begin("Mafia: Oakwood - Chat",
            nullptr,
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar);

        ImGui::SetWindowSize(ImVec2(600, ySize));
        ImGui::SetWindowPos(ImVec2(1, yPos));

        ImGui::PushFontShadow(0xFF000000);

        ImGui::BeginChild("scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_NoScrollbar);

        if (!chat_messages.empty()) {
            for (auto message : chat_messages) {
                colored_text("%s", message.second.c_str());
            }
        }

        if (new_msg_arrived) {
            ImGui::SetScrollHere(1.0f);
            new_msg_arrived=false;
        }

        if (chat_send) {
            is_focused = false;
            if (strlen(add_text)) {
                bool is_command = false;

                if (add_text[0] == '/')
                    is_command = parse_command(add_text);

                if (!is_command) {
                    librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
                        librg_data_wu16(&data, zpl_strlen(add_text));
                        librg_data_wptr(&data, (void *)add_text, zpl_strlen(add_text));
                    });
                }

                chat_history.insert(chat_history.begin(), std::string(add_text));
                chat_history_index = -1;
                strcpy(add_text, "");
            }

            input::toggle_block_input();
            ImGui::SetScrollHere(1.0f);
        }

        ImGui::EndChild();
        ImGui::PopFontShadow();

        if (input::InputState.input_blocked && MafiaSDK::IsWindowFocused()) {

            if (is_focused)
            {
                ImGui::SetKeyboardFocusHere(0);
                ImGui::PushItemWidth(600);
                ImGui::InputText("", add_text, IM_ARRAYSIZE(add_text), ImGuiInputTextFlags_CallbackAlways, inputTextHandler);
                ImGui::PopItemWidth();
            }
        }
        ImGui::SetScrollHere(1.0f);
        ImGui::End();
        
    }
}
