#pragma once
namespace effects {
    extern bool is_enabled;
    inline void load(std::string effect_file);
}

namespace chat {

    struct ChatCommand {
        std::string command_name;
        std::function<void(std::vector<std::string>)> command_ptr;
    };

    std::vector<ChatCommand> chat_commands;
    std::vector<std::pair<ImVec4, std::string>> chat_messages;	
    IDirect3DDevice9* main_device = nullptr;

    std::vector<const char*> chat_history;
    unsigned int chat_history_index;
    unsigned int chat_current_msg;

    constexpr unsigned int VK_T = 0x54;
    KeyToggle key_chat_open(VK_T);
    KeyToggle key_chat_send(VK_RETURN);
    KeyToggle key_chat_history_prev(VK_UP);
    KeyToggle key_chat_history_next(VK_DOWN);
    KeyToggle key_chat_caret_prev(VK_LEFT);
    KeyToggle key_chat_caret_next(VK_RIGHT);

    void add_debug(const std::string & msg)
    {
        chat_messages.push_back({
            ImColor(255, 255, 255, 255),
            "Debug: "  + msg
        });
    }

    auto register_command(const std::string & name, std::function<void(std::vector<std::string>)> ptr) {
        if(ptr != nullptr) {
            chat_commands.push_back({name, ptr});
        }
    }

    auto get_vector_of_args(std::string command_str) {
        return split(command_str, " ");
    }

    auto parse_command(std::string command_str) {
        if(!command_str.empty()) {
            for(auto command : chat_commands) {
                if(command_str.find(command.command_name) != std::string::npos) {
                    if(command.command_ptr != nullptr) {
                        command.command_ptr(get_vector_of_args(command_str));
                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    MafiaSDK::C_Car* new_car = nullptr;
    auto init(IDirect3DDevice9* device) {

        main_device = device;
    
        register_command("/q", [&](std::vector<std::string> args) {
            librg_network_stop(&network_context);
            exit(0);
        });

        register_command("/test", [&](std::vector<std::string> args) {
            auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();
            Vector3D frame_scale = { 1.0f, 1.0f, 1.0f };

            auto new_frame = new MafiaSDK::I3D_Frame();
            new_frame->SetScale(frame_scale);
            new_frame->SetPos(local_player->GetInterface()->humanObject.entity.position);
            new_frame->LoadModel("taxi00.i3d");
            new_frame->SetName("TestingPed");

            new_car = reinterpret_cast<MafiaSDK::C_Car*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Car));
            new_car->Init(new_frame);
            new_car->SetActive(1);
            MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_car);
            new_car->GetInterface()->entity.position = local_player->GetInterface()->humanObject.entity.position;
        
            
        });

        register_command("/npc", [&](std::vector<std::string> args) {
            librg_send(&network_context, NETWORK_NPC_CREATE, data, {});
        });
        
        register_command("/shade", [&](std::vector<std::string> args) {
            effects::load("Cinematic.fx");
            effects::is_enabled = true;
        });

        register_command("/savepos", [&](std::vector<std::string> args) {
            auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();

            std::ofstream pos_file("positions.txt");
            auto pos = local_player->GetInterface()->humanObject.entity.position;
            auto dir = local_player->GetInterface()->humanObject.entity.rotation;
            zpl_vec3 position  = EXPAND_VEC(pos);
            zpl_vec3 direction = EXPAND_VEC(dir);
            auto rot = DirToRotation180(direction);

            pos_file << position.x << " " << position.y << " " << position.z << ", " << rot << std::endl;
            add_debug("Position was stored!");
        });
    }

    auto render() {

        if(MafiaSDK::GetGMMenu() || !MafiaSDK::GetMission()->GetGame()) return; 
        ImGuiStyle& style = ImGui::GetStyle();

        if (input::InputState.input_blocked && MafiaSDK::IsWindowFocused()) {
            style.Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24, 200);
            style.Colors[ImGuiCol_TitleBg] =  ImColor(150, 0, 0, 200);
        }
        else {
            style.Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24, 50);
            style.Colors[ImGuiCol_TitleBg] = ImColor(150, 0, 0, 70);
        }

        ImGui::Begin("Mafia Multiplayer - Chat", 
            nullptr, 
            ImGuiWindowFlags_NoMove | 
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
            
            static char add_text[4096] = "";
            ImGui::SetKeyboardFocusHere(0);
            ImGui::InputText("", add_text, IM_ARRAYSIZE(add_text));
            
            if (key_chat_history_prev) {
                if (!chat_history.empty()) {
                    if (chat_history_index == 0) {
                        chat_history_index--;
                    }
                    else {
                        chat_history_index = chat_history.size();
                    }
                    strcpy(add_text, (const char*)chat_history[chat_history_index]);
                }
            }

            if (key_chat_history_next) {
                if (!chat_history.empty()) {
                    if (chat_history_index == chat_history.size()) {
                        chat_history_index++;
                    }
                    else {
                        chat_history_index = 0;
                    }
                    strcpy(add_text, (const char*)chat_history[chat_history_index]);
                }
            }

            if (key_chat_send) {
                
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

                    chat_history.push_back(add_text);
                    strcpy(add_text, "");
                }

                input::toggle_block_input();
            }
        }
        ImGui::SetScrollHere(1.0f);
        ImGui::End();
    }
}
