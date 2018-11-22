librg_network_add(&network_context, NETWORK_PLAYER_SHOOT, [](librg_message* msg) {
    
    zpl_vec3 pos;
    auto entity = librg_entity_find(&network_context, msg->peer);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

    mod_message_send_except(&network_context, NETWORK_PLAYER_SHOOT, msg->peer, [&](librg_data *data) {
        librg_data_went(data, entity->id);
        librg_data_wptr(data, &pos, sizeof(zpl_vec3));
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_THROW_GRENADE, [](librg_message* msg) {
    
    zpl_vec3 pos;
    auto entity = librg_entity_find(&network_context, msg->peer);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));

    printf("Debug player throw grenade: %f %f %f\n", pos.x, pos.y, pos.z);
    mod_message_send_except(&network_context, NETWORK_PLAYER_THROW_GRENADE, msg->peer, [&](librg_data *data) {
        librg_data_went(data, entity->id);
        librg_data_wptr(data, &pos, sizeof(zpl_vec3));
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    u32 index = librg_data_ru32(msg->data);
    
    //set serverside current weapon index for player
    auto player = (mafia_player*)entity->user_data;
    player->current_weapon_id = index;

    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, msg->peer, [&](librg_data *data) {
        librg_data_went(data, entity->id);
        librg_data_wu32(data, index);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, [](librg_message* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    
    //process inventory here :) TODO !
    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, msg->peer, [&](librg_data *data) {
        librg_data_went(data, entity->id);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, [](librg_message* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    if (!entity) return;
    
    //process inventory here :) TODO !
    mod_message_send_except(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, msg->peer, [&](librg_data *data) {
        librg_data_went(data, entity->id);
    });
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_DROP, [](librg_message* msg) {

    auto entity = librg_entity_find(&network_context, msg->peer);
    inventory_item item = {0};
    char model[32] = "";
    
    librg_data_rptr(msg->data, &item, sizeof(inventory_item));
    librg_data_rptr(msg->data, model, sizeof(char) * 32);
    
    spawn_weapon_drop(entity->position, model, item);
    
    player_inventory_remove(entity, item.weaponId, true, true);
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message* msg) {

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
