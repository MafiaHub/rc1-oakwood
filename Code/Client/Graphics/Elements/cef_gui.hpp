#pragma once

#include <unordered_map>
inline void mod_shutdown();
namespace effects 
{
    extern bool is_enabled;
    inline void load(std::string effect_file);
}

namespace graphics 
{
    inline D3DSURFACE_DESC get_backbuffer_desc(IDirect3DDevice9 *device);
}

namespace cefgui 
{
    cef::object *main_browser = nullptr;
    using ChatCommand = std::function<void(std::vector<std::string>)>;
    std::unordered_map<std::string, ChatCommand> chat_commands;
    constexpr unsigned int VK_T = 0x54;

    input::KeyToggle key_chat_open(VK_T);
    input::KeyToggle key_reload(VK_F2);
    input::KeyToggle key_unrequire(VK_ESCAPE);

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
            if (!input::InputState.input_blocked) input::block_input(true);
            if (main_browser) {
                CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("executeEvent");
                json send_msg = {{"type", "update-input"}, {"blocked", input::InputState.input_blocked}};                
                send_message(msg, send_msg);
            }
        }

        if (key_reload) {
            if (main_browser) {
                main_browser->browser->Reload();
            }
        }

        if (key_unrequire) {
            input::block_input(false);
        }
    }
    
    auto load_browser() {
        if (!global_device) return;

        auto desc = graphics::get_backbuffer_desc(global_device);
        if (!cefgui::main_browser)
            cefgui::main_browser = cef::browser_create(global_device, (std::string("http://") + GlobalConfig.server_address + ":27010/app.html").c_str(), desc.Width, desc.Height, 1);
    }

    #include "Natives/cef_commands.hpp"
    #include "Natives/chat_commands.hpp"

    auto init(IDirect3DDevice9 *device) {

        /* TODO: Move this to a better place */
        init_cef_commands();
        init_chat_commands();
    }
} // namespace chat
