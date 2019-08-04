// void broadcast_msg_color(const char *text, u32 color) {
//     librg_send(&network_context, NETWORK_SEND_CONSOLE_MSG, data, {
//         librg_data_wu32(&data, strlen(text));
//         librg_data_wu32(&data, color);
//         librg_data_wptr(&data, (void*)text, strlen(text));
//     });
// }

// void chat_print(const char* text) {
//     librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
//         auto len = strlen(text);
//         librg_data_wu16(&data, len);
//         librg_data_wptr(&data, (void *)text, len);
//     });
// }

// void send_msg(const char* text, librg_entity *receiver) {
//     if (!receiver->client_peer)
//         return;

//     librg_send_to(&network_context, NETWORK_SEND_CHAT_MSG, receiver->client_peer, data, {
//         auto len = strlen(text);
//         librg_data_wu16(&data, len);
//         librg_data_wptr(&data, (void *)text, len);
//     });
// }

