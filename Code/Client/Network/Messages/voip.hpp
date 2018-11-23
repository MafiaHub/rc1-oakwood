librg_network_add(&network_context, NETWORK_SEND_VOIP_DATA, [](librg_message* msg) {

    u32 entity_id = librg_data_ru32(msg->data);
    f32 send_time = librg_data_rf32(msg->data);

    u32 encoded_buffer_size = librg_data_ru32(msg->data);
    void* buffer_data = malloc(encoded_buffer_size);
    librg_data_rptr(msg->data, buffer_data, encoded_buffer_size);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    
    if(entity && entity->user_data && zpl_time_now() - send_time < 2.0f) {
        
        auto player = (mafia_player*)entity->user_data;
        voip::decode_and_push(player->voice_channel, (unsigned char*)buffer_data, encoded_buffer_size, zpl_time_now() - player->last_talked);

        BASS_3DVECTOR pos = EXPAND_VEC(entity->position);
        BASS_3DVECTOR rot = EXPAND_VEC(player->rotation);
        BASS_3DVECTOR top = { 0.0f, 1.0f, 0.0f };
        BASS_3DVECTOR my_rot = EXPAND_VEC(local_player.ped->GetInterface()->humanObject.entity.rotation);
        BASS_3DVECTOR my_pos = EXPAND_VEC(local_player.ped->GetInterface()->humanObject.entity.position);
    
        BASS_Set3DPosition(&my_pos, NULL, &my_rot, &top);
        BASS_ChannelSet3DPosition(player->voice_channel->playback_stream, &pos, &rot, NULL);
        BASS_Apply3D();

        player->last_talked = zpl_time_now();
    }
    
    free(buffer_data);
});