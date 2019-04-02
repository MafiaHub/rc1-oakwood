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
   
    /* keys definitions */
    input::KeyToggle key_chat_open(VK_T);
    input::KeyToggle key_chat_send(VK_RETURN);
    input::KeyToggle key_chat_history_prev(VK_UP);
    input::KeyToggle key_chat_history_next(VK_DOWN);
    input::KeyToggle key_chat_caret_prev(VK_LEFT);
    input::KeyToggle key_chat_caret_next(VK_RIGHT);

    /* history */
    static char add_text[4096] = { 0 };
    static char backup_text[4096] = { 0 };

    /* methods */
    inline void add_debug(const std::string& msg) {
        chat_messages.push_back({
            ImColor(255, 255, 255, 255),
            "Debug: " + msg
        });
    }

    inline void add_message(const std::string& msg) {
        chat_messages.push_back({
           ImColor(255, 255, 255, 255),
           msg
        });
    }

    inline void register_command(const std::string & name, std::function<void(std::vector<std::string>)> ptr) {
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

    inline void init() {
        register_command("/q", [&](std::vector<std::string> args) {
            librg_network_stop(&network_context);
            ExitProcess(ERROR_SUCCESS);
        });

        register_command("/crash", [&](std::vector<std::string> args) {
            *(int*)0 = 42;
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

                    input_text_replace(data, chat_history[chat_history_index].c_str());
                }
            }
        }
        return FALSE;
    }

    inline void render() {

        if (key_chat_open && librg_is_connected(&network_context)) {
            is_focused = !is_focused;
            input::block_input(is_focused);
        }

        if (!MafiaSDK::GetMission()->GetGame() || input::is_key_down(VK_TAB) || 
            !librg_is_connected(&network_context)) 
            return;

        if (modules::pausemenu::is_enabled)
            return;

#ifdef OAKWOOD_DEBUG
        if (modules::debug::is_enabled)
            return;
#endif

        ImGui::Begin("Mafia: Oakwood - Chat",
            nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar);

        ImGui::SetWindowSize(ImVec2(400, 300));
        ImGui::SetWindowPos(ImVec2(20, 20));
        ImGui::BeginChild("scrolling");

        if (!chat_messages.empty()) {
            for (auto message : chat_messages) {
                ImGui::TextColored(message.first, message.second.c_str());
            }
        }

        ImGui::SetScrollHere(1.0f);
        ImGui::EndChild();

        if (!input::InputState.input_blocked && MafiaSDK::IsWindowFocused() && key_chat_open) {
            input::toggle_block_input();
        }

        if (input::InputState.input_blocked && MafiaSDK::IsWindowFocused()) {

            if (is_focused)
                ImGui::SetKeyboardFocusHere(0);

            ImGui::InputText("", add_text, IM_ARRAYSIZE(add_text), ImGuiInputTextFlags_CallbackAlways, inputTextHandler);

            if (key_chat_send) {
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
            }
        }
        ImGui::SetScrollHere(1.0f);
        ImGui::End();
    }
}
