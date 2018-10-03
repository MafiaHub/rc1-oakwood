librg_network_add(&network_context, NETWORK_PLAYER_SHOOT, [](librg_message_t* msg) {
    
    zpl_vec3 pos;
    auto entity = librg_entity_find(&network_context, msg->peer);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

    mod_message_send_except(&network_context, NETWORK_PLAYER_SHOOT, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, entity->id);
        librg_data_wptr(data, &pos, sizeof(zpl_vec3));
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_THROW_GRENADE, [](librg_message_t* msg) {
    
    zpl_vec3 pos;
    auto entity = librg_entity_find(&network_context, msg->peer);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

    printf("Debug player throw grenade: %f %f %f\n", pos.x, pos.y, pos.z);
    mod_message_send_except(&network_context, NETWORK_PLAYER_THROW_GRENADE, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, entity->id);
        librg_data_wptr(data, &pos, sizeof(zpl_vec3));
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message_t* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    u32 index = librg_data_ru32(msg->data);
    
    //set serverside current weapon index for player
    auto player = (mafia_player*)entity->user_data;
    player->current_weapon_id = index;

    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, entity->id);
        librg_data_wu32(data, index);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, [](librg_message_t* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    
    //process inventory here :) TODO !
    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, entity->id);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, [](librg_message_t* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    
    //process inventory here :) TODO !
    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, entity->id);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_DROP, [](librg_message_t* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    inventory_item item = {0};
    char model[32] = "";
    
    librg_data_rptr(msg->data, &item, sizeof(inventory_item));
    librg_data_rptr(msg->data, model, sizeof(char) * 32);
    
    spawn_weapon_drop(entity->position, model, item);
    
    player_inventory_remove(entity, item.weaponId, true, true);
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message_t* msg) {

    auto player_entity		= librg_entity_find(&network_context, msg->peer);
    librg_entity_id id		= librg_data_rent(msg->data);
    auto entity				= librg_entity_fetch(&network_context, id);
    auto weapon_drop		= (mafia_weapon_drop*)entity->user_data;

    player_inventory_add(player_entity, &weapon_drop->weapon, true, true);

    if (entity) {
        if (entity->user_data) 
            delete entity->user_data;
        librg_entity_destroy(&network_context, id);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_HIT, [](librg_message_t* msg) {

    auto sender_ent = librg_entity_find(&network_context, msg->peer);
    librg_entity_id target_id = librg_data_rent(msg->data);
    u32 hit_type = librg_data_ru32(msg->data);
    zpl_vec3 unk1, unk2, unk3;
    librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));
    f32 damage = librg_data_rf32(msg->data);
    u32 player_part = librg_data_ru32(msg->data);

    auto player = (mafia_player*)sender_ent->user_data;
    if (player) {
        if (player->health - damage < 0.0f)
            player->health = 0.0f;
        else player->health -= damage;
    }

    mod_message_send_except(&network_context, NETWORK_PLAYER_HIT, msg->peer, [&](librg_data_t *data) {
        librg_data_went(data, sender_ent->id);
        librg_data_went(data, target_id);
        librg_data_wu32(data, hit_type);
        librg_data_wptr(data, (void*)&unk1, sizeof(zpl_vec3));
        librg_data_wptr(data, (void*)&unk2, sizeof(zpl_vec3));
        librg_data_wptr(data, (void*)&unk3, sizeof(zpl_vec3));
        librg_data_wf32(data, damage);
        librg_data_wu32(data, player_part);
    });
});