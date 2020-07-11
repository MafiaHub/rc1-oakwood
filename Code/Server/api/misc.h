int oak_killbox_set(float level) {
    return oak_config_killbox_set(level);
}

int oak_play_audio_stream(oak_player sender, const char* url, int length)
{
    if (oak_player_invalid(sender)) return -1;

    auto player = oak_entity_player_get(sender);

    if (!player) return -1;

    librg_send_to(oak_network_ctx_get(), NETWORK_PLAY_STREAM, player->native_entity->client_peer, data, {
                librg_data_wu16(&data, length);
                librg_data_wptr(&data, (void*)url, length);
        });
    return 0;
}

int oak_pause_audio_stream(oak_player sender)
{
    if (oak_player_invalid(sender)) return -1;

    auto player = oak_entity_player_get(sender);

    if (!player) return -1;

    librg_send_to(oak_network_ctx_get(), NETWORK_PAUSE_STREAM, player->native_entity->client_peer, data, {});
    return 0;
}

int oak_set_stream_volume(oak_player sender, float volume)
{
    if (oak_player_invalid(sender)) return -1;

    auto player = oak_entity_player_get(sender);

    if (!player) return -1;

    librg_send_to(oak_network_ctx_get(), NETWORK_SET_STREAM_VOLUME, player->native_entity->client_peer, data, {
                librg_data_wf32(&data, volume);
        });
    return 0;
}

int oak_stop_audio_stream(oak_player sender)
{
    if (oak_player_invalid(sender)) return -1;

    auto player = oak_entity_player_get(sender);

    if (!player) return -1;

    librg_send_to(oak_network_ctx_get(), NETWORK_STOP_STREAM, player->native_entity->client_peer, data, {});
    return 0;
}

int oak_create_explosion(oak_player sender, oak_vec3 pos, float radius, float force)
{
    if (oak_player_invalid(sender)) return -1;

    auto entity = oak_entity_player_get(sender);

    if (!entity) return -1;

    zpl_vec3 forward = ComputeDirVector(DirToRotation360(entity->rotation));
    zpl_vec3 newPos = { pos.x - forward.x, pos.y, pos.z - forward.z };

    librg_send(oak_network_ctx_get(), NETWORK_CREATE_EXPL, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wf32(&data, radius);
                librg_data_wf32(&data, force);
                librg_data_wptr(&data, &newPos, sizeof(newPos));
        });

    return 0;
}

float oak_killbox_get() {
    return oak_config_killbox_get();
}
