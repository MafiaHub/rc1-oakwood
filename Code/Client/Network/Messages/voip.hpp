librg_network_add(&network_context, NETWORK_SEND_VOIP_DATA, [](librg_message_t* msg) {

    u32 entity_id = librg_data_ru32(msg->data);
    u32 encoded_buffer_size = librg_data_ru32(msg->data);
    void* buffer_data = malloc(encoded_buffer_size);
    librg_data_rptr(msg->data, buffer_data, encoded_buffer_size);

    auto entity = librg_entity_fetch(&network_context, entity_id);
    if(entity && entity->user_data) {
        
        auto player = (mafia_player*)entity->user_data;
		voip::decode_and_push(player->voice_channel, (unsigned char*)buffer_data, encoded_buffer_size);
	}
    
    free(buffer_data);
});