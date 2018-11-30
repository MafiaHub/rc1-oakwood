#pragma once
namespace effects {
    extern bool is_enabled;
    inline void load(std::string effect_file);
}

namespace graphics {
    inline D3DSURFACE_DESC get_backbuffer_desc(IDirect3DDevice9* device);
}

namespace chat {

    struct ChatCommand {
        std::string command_name;
        std::function<void(std::vector<std::string>)> command_ptr;
    };
    
    cef::object* main_browser = nullptr;
    std::vector<ChatCommand> chat_commands;    
    constexpr unsigned int VK_T = 0x54;
    KeyToggle key_chat_open(VK_T);
  
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

    auto add_message(std::string new_msg) {
        if (main_browser) {
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("executeEvent");
            json send_msg = {
                { "type", "chat-msg" },
                { "msg", new_msg }
            };

            auto send_args = msg->GetArgumentList(); {
                send_args->SetSize(1);
                send_args->SetString(0, send_msg.dump());
            };

            main_browser->browser.get()->SendProcessMessage(PID_RENDERER, msg);
        }
    }

    auto update() {
        if (key_chat_open) {

            // block input when T only when we dont writting
            // we unblock input from cef handle
            if(!input::InputState.input_blocked)
                input::block_input(true);

            if (main_browser) {
                CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("callEvent");
                json send_msg = {
                    { "type", "update-input" },
                    { "blocked", input::InputState.input_blocked }
                };

                auto send_args = msg->GetArgumentList();
                send_args->SetSize(1);
                send_args->SetString(0, send_msg.dump());
                main_browser->browser.get()->SendProcessMessage(PID_RENDERER, msg);
            }
        }
    }

    auto init(IDirect3DDevice9* device) {

        auto back_buffer = graphics::get_backbuffer_desc(device);
        main_browser = cef::browser_create(device, "file:///D:/Games/Steam/steamapps/common/Mafia/Mafia/ceftest/index.html", back_buffer.Width, back_buffer.Height, 1);

        cef::register_native("update-input", [=](CefRefPtr<CefListValue> args) {
            input::block_input(atoi(args->GetString(1).ToString().c_str()));
        });

        cef::register_native("chat-msg", [=](CefRefPtr<CefListValue> args) {

            auto message = args->GetString(1).ToString();   
            if (!message.empty()) {
                bool is_command = false;

                if (message[0] == '/')
                    is_command = parse_command(message);

                if (!is_command && librg_is_connected(&network_context)) {
                    librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
                        librg_data_wu16(&data, message.length());
                        librg_data_wptr(&data, (void *)message.c_str(), message.length());
                    });
                }
            }
        });

        register_command("/q", [&](std::vector<std::string> args) {
            librg_network_stop(&network_context);
            exit(0);
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
        });
    }
}
