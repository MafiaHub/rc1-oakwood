librg_network_add(&network_context, NETWORK_SEND_VOIP_DATA, [](librg_message* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    u32 encoded_buffer_size = librg_data_ru32(msg->data);
    void* buffer_data = malloc(encoded_buffer_size);
    librg_data_rptr(msg->data, buffer_data, encoded_buffer_size);

    if(entity) {
        mod_message_send_except(&network_context, NETWORK_SEND_VOIP_DATA, msg->peer, [&](librg_data *data) {
            librg_data_went(data, entity->id);
            librg_data_wf32(data, zpl_time_now());
            librg_data_wu32(data, encoded_buffer_size);
            librg_data_wptr(data, buffer_data, encoded_buffer_size);
        });
    }
    
    free(buffer_data);
});