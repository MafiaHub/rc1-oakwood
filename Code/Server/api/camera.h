#define OAK_CAMERA_TARGETING(player, target) do { \
        i32 id = -1; \
        if (player->spec_id != -1) \
            librg_entity_visibility_set_for(&network_context, player->native_id, target->native_id, LIBRG_DEFAULT_VISIBILITY); \
        if (target) { \
            id = target->native_id; \
            librg_entity_visibility_set_for(&network_context, player->native_id, target->native_id, LIBRG_ALWAYS_VISIBLE); \
        } \
        player->spec_id = id; \
        librg_send_to(&network_context, NETWORK_CAMERA_TARGET, player->native_entity->client_peer, data, { librg_data_went(&data, id); }); \
    } while (0)

/**
 * Set camera to spefific point
 * @param  oak_player
 * @param  oak_vec3   position
 * @param  oak_vec3   rotation
 * @return 0 on success
 */
int oak_camera_set(oak_player id, oak_vec3 pos, oak_vec3 rot) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);

    librg_send_to(&network_context, NETWORK_CAMERA_SET_POS, player->native_entity->client_peer, data, {
        librg_data_wptr(&data, &pos, sizeof(pos));
        librg_data_wptr(&data, &rot, sizeof(rot));
    });

    return 0;
}

/**
 * Unlocks camera
 * @param  id
 * @return
 */
int oak_camera_unlock(oak_player id) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);
    librg_message_send_to(&network_context, NETWORK_CAMERA_UNLOCK, player->native_entity->client_peer, NULL, 0);

    return 0;
}

/**
 * Specifies which player we need to target
 * @param  id
 * @param  targetid
 * @return
 */
int oak_camera_target_player(oak_player id, oak_player targetid) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);
    auto target = oak_entity_player_get(targetid); ZPL_ASSERT_NOT_NULL(target);

    OAK_CAMERA_TARGETING(player, target);

    return 0;
}

/**
 * Specifies which vehicle we need to target
 * @param  id
 * @param  targetid
 * @return
 */
int oak_camera_target_vehicle(oak_player id, oak_vehicle targetid) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);
    auto target = oak_entity_vehicle_get(targetid); ZPL_ASSERT_NOT_NULL(target);

    OAK_CAMERA_TARGETING(player, target);

    return 0;
}

/**
 * Unset our camera target
 * @param  id
 * @return
 */
int oak_camera_target_unset(oak_player id) {
    auto player = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(player);

    if (player->spec_id != -1) {
        librg_entity_visibility_set_for(&network_context, player->native_id, -1, LIBRG_DEFAULT_VISIBILITY);
    }

    player->spec_id = -1;

    /* send invalid player to reset */
    librg_send_to(&network_context, NETWORK_CAMERA_TARGET, player->native_entity->client_peer, data, {
        librg_data_went(&data, -1);
    });

    return 0;
}

#undef OAK_CAMERA_TARGETING