#pragma once

#include <unordered_map>

namespace effects 
{
    extern bool is_enabled;
    inline void load(std::string effect_file);
} // namespace effects

namespace graphics 
{
    inline D3DSURFACE_DESC get_backbuffer_desc(IDirect3DDevice9 *device);
}

namespace chat 
{
    using ChatCommand = std::function<void(std::vector<std::string>)>;

    cef::object *main_browser = nullptr;
    std::unordered_map<std::string, ChatCommand> chat_commands;
    constexpr unsigned int VK_T = 0x54;
    input::KeyToggle key_chat_open(VK_T);

    auto register_command(const std::string &name, std::function<void(std::vector<std::string>)> ptr) {
        if (ptr != nullptr) {
            chat_commands.insert(std::make_pair(name, ptr));
        }
    }

    auto parse_command(std::string command_str) {
        if (chat_commands.empty())
            return false;

        auto command_args = split(command_str, " ");
        auto command_name = command_args[0];

        auto command_it = chat_commands.find(command_name);
        if (command_it != chat_commands.end()) {
            command_it->second(command_args);
            return true;
        }
    

        return false;
    }

    auto send_message(CefRefPtr<CefProcessMessage> hnd, json message) {
        auto send_args = hnd->GetArgumentList();
        {
            send_args->SetSize(1);
            send_args->SetString(0, message.dump());
        };

        main_browser->browser.get()->SendProcessMessage(PID_RENDERER, hnd);
    }

    auto add_message(std::string new_msg) {
        if (main_browser) {
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("executeEvent");
            json send_msg = {{"type", "chat-msg"}, {"msg", new_msg}};

            send_message(msg, send_msg);
        }
    }

    auto update() {
        if (key_chat_open) {
            // block input when T only when we dont writting
            // we unblock input from cef handle
            if (!input::InputState.input_blocked) input::block_input(true);

            if (main_browser) {
                CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("executeEvent");
                json send_msg = {{"type", "update-input"}, {"blocked", input::InputState.input_blocked}};
                
                send_message(msg, send_msg);
            }
        }
    }
    
    auto load_browser() {
        if (!global_device) return;

        auto desc = graphics::get_backbuffer_desc(global_device);
        chat::main_browser = cef::browser_create(global_device, (std::string("http://") + GlobalConfig.server_address + ":27010/chat.html").c_str(), desc.Width, desc.Height, 1);
    }

    #include "Natives/cef_commands.hpp"
    #include "Natives/chat_commands.hpp"

    auto init(IDirect3DDevice9 *device) {

        auto back_buffer = graphics::get_backbuffer_desc(device);
        
        /* TODO: Move this to a better place */
        init_cef_commands();

        init_chat_commands();
    }
} // namespace chat
