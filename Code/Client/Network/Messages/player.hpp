librg_network_add(&network_context, NETWORK_PLAYER_RESPAWN, [](librg_message_t* msg) {

    //read data
    zpl_vec3 position, rotation;
    player_inventory inventory;
    char model[32];

    librg_entity_id player_id = librg_data_rent(msg->data);
    librg_data_rptr(msg->data, &position, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, &rotation, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, model, sizeof(char) * 32);
    librg_data_rptr(msg->data, &inventory, sizeof(player_inventory));
    u32 current_wep = librg_data_ru32(msg->data);
    f32 health = librg_data_rf32(msg->data);

    bool is_local_player = player_id == local_player.entity.id;

    if (!is_local_player) {
        auto player_ent = librg_entity_fetch(&network_context, player_id);

        if (player_ent && player_ent->user_data) {
            auto player = (mafia_player*)player_ent->user_data;
            
            auto new_ped = player_spawn(
                position,
                rotation,
                inventory,
                model,
                current_wep,
                health,
                is_local_player,
                0);

            printf("respawn remote !\n");

            //remove old ped
            //find new way how to properly remove players :)
            if (player->ped) {
                player_despawn(reinterpret_cast<MafiaSDK::C_Player*>(player->ped));
                player->ped = new_ped;
            }

            //interpolator init
            player->inter_pos.init(position);
            player->inter_rot.init(rotation);
        }
    } else {
        auto new_ped = player_spawn(
            position,
            rotation,
            inventory,
            model,
            current_wep,
            health,
            is_local_player,
            0);

        MafiaSDK::GetMission()->GetGame()->GetCamera()->SetMode(true, 1);
        MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(new_ped);
        MafiaSDK::GetMission()->GetGame()->GetIndicators()->PlayerSetWingmanLives(100);

        if (local_player.ped) {
			player_despawn(reinterpret_cast<MafiaSDK::C_Player*>(local_player.ped));
        }

        local_player.dead = false;
        local_player.ped = new_ped;
        MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(false, 500, 0xFFFFFF);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SPAWN, [](librg_message_t* msg) {
    
    zpl_vec3 position, rotation;
    player_inventory inventory;
    char model[32];

    librg_data_rptr(msg->data, &position, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, &rotation, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, model, sizeof(char) * 32);
    librg_data_rptr(msg->data, &inventory, sizeof(player_inventory));
    u32 current_wep = librg_data_ru32(msg->data);
    f32 health = librg_data_rf32(msg->data);

    auto ped = player_spawn(
        position, 
        rotation,
        inventory, 
        model, 
        current_wep, 
        health, 
        true, 
        0);

    local_player.ped = ped;
    
    MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(ped);
});

librg_network_add(&network_context, NETWORK_PLAYER_SHOOT, [](librg_message_t* msg) {
    zpl_vec3 pos;
    librg_entity_id id = librg_data_rent(msg->data);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));
    Vector3D target = EXPAND_VEC(pos);

    auto entity = librg_entity_fetch(&network_context, id);
    if(entity) {
        auto player = (mafia_player*)entity->user_data;
        *(BYTE*)((DWORD)player->ped + 0x4A4) = 50;
        player->ped->Do_Shoot(true, target);
        player->ped->Do_Shoot(false, target);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_THROW_GRENADE, [](librg_message_t* msg) {
    zpl_vec3 pos;
    librg_entity_id id = librg_data_rent(msg->data);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));
    Vector3D target = EXPAND_VEC(pos);

    auto entity = librg_entity_fetch(&network_context, id);
    if(entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_ThrowGranade(target);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message_t* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    u32 index = librg_data_ru32(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->current_weapon_id = index;
        player->ped->G_Inventory_SelectByID(index);
        player->ped->Do_ChangeWeapon(0, 0);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, [](librg_message_t* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_Reload();
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, [](librg_message_t* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_Holster();
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_DROP, [](librg_message_t* msg) {

    librg_entity_id id = librg_data_rent(msg->data);
    u32 remove_id = librg_data_ru32(msg->data);

    auto entity = librg_entity_fetch(&network_context, id); 
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->G_Inventory_RemoveWeapon(remove_id);

        if (remove_id == player->current_weapon_id)
            player->current_weapon_id = 0;

        player->ped->G_Inventory_SelectByID(0);
        player->ped->Do_ChangeWeapon(0, 0);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message_t* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    auto player = (mafia_player*)entity->user_data;
    S_GameItem weapon_item;
    librg_data_rptr(msg->data, &weapon_item, sizeof(inventory_item));

    //give player weapon
    ((MafiaSDK::C_Human*)player->ped)->G_Inventory_AddItem(weapon_item);
    if (player->current_weapon_id == weapon_item.weaponId) return;

    //set current weapon to new picked one
    player->current_weapon_id = weapon_item.weaponId;

    //apply model
    player->ped->G_Inventory_SelectByID(weapon_item.weaponId);
    player->ped->Do_ChangeWeapon(0, 0);
    player->ped->ChangeWeaponModel();

    mod_debug("weapon pickup");
});

librg_network_add(&network_context, NETWORK_PLAYER_HIT, [](librg_message_t* msg) {

    librg_entity_id sender_id = librg_data_rent(msg->data);
    auto sender_ent = librg_entity_fetch(&network_context, sender_id);

    u32 hit_type = librg_data_ru32(msg->data);
    Vector3D unk1, unk2, unk3;
    librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));

    f32 damage = librg_data_rf32(msg->data);
    u32 player_part = librg_data_ru32(msg->data);

    if (sender_ent && sender_ent->user_data && local_player.ped) {
        auto player = (mafia_player*)(sender_ent->user_data);
        hit_hook_skip = false;
        local_player.ped->Hit(hit_type, unk1, unk2, unk3, damage, player->ped, player_part, NULL);
        hit_hook_skip = true;
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_DIE, [](librg_message_t* msg) {
    auto entity_id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity->user_data) {
        auto player = (mafia_player*)(entity->user_data);
        if (player) {
            //player->ped->Intern_ForceDeath();
        }
    }
});

librg_network_add(&network_context, NETWORK_SEND_CONSOLE_MSG, [](librg_message_t* msg) {
    u32 msg_size	= librg_data_ru32(msg->data);
    u32 msg_color	= librg_data_ru32(msg->data);
    char* text = reinterpret_cast<char*>(malloc(msg_size));
    librg_data_rptr(msg->data, text, msg_size);
    text[msg_size]  = '\0';
    MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText(reinterpret_cast<const char*>(text), msg_color);
});