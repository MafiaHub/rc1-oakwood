#pragma once

void init_cef_commands() {
    cef::register_native("update-input", [=](CefRefPtr<CefListValue> args) {
        input::block_input(atoi(args->GetString(1).ToString().c_str()));
    });

    cef::register_native("chat-msg", [=](CefRefPtr<CefListValue> args) {
        auto message = args->GetString(1).ToString();
        if (!message.empty())
        {
            bool is_command = false;

            if (message[0] == '/')
                is_command = parse_command(message);

            if (!is_command && librg_is_connected(&network_context))
            {
                librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
                    librg_data_wu16(&data, message.length());
                    librg_data_wptr(&data, (void *)message.c_str(), message.length());
                });
            }
        }
    });
}