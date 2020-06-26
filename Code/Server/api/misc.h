int oak_killbox_set(float level) {
    return oak_config_killbox_set(level);
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
