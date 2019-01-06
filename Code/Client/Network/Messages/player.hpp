/*librg_network_add(&network_context, NETWORK_PLAYER_RESPAWN, [](librg_message* msg) {

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

    bool is_local_player = player_id == local_player.entity_id;

    if (!is_local_player) {
        auto player_ent = librg_entity_fetch(&network_context, player_id);

        if (player_ent && player_ent->user_data) {
            auto player = (mafia_player*)player_ent->user_data;
            strncpy(player->model, model, 32);
            
            auto new_ped = player_spawn(
                position,
                rotation,
                inventory,
                model,
                current_wep,
                health,
                is_local_player,
                -1,
                false);

            if (player->ped) {
                player_despawn(player->ped);
                player->ped = new_ped;
            }
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
            -1,
            false);

        auto player = get_local_player();
        strncpy(player->model, model, 32);
        player_inventory_send();
    }
});*/


//NOTE(DavoSK): When local player spawns
//When local player dies we are removing librg_entity
//And before we calling spawn we are create new blank one 
//We need to realocate local_player entity

librg_network_add(&network_context, NETWORK_PLAYER_SPAWN, [](librg_message* msg) {
    
    u32 local_player_entity_id  = librg_data_ru32(msg->data);
    auto new_player_entity      = librg_entity_fetch(&network_context, local_player_entity_id);
    auto new_player_data        = new mafia_player;

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
        0,
        false);

    new_player_data->ped = ped;
    new_player_entity->user_data = new_player_data;
    local_player.entity_id = local_player_entity_id;
});

librg_network_add(&network_context, NETWORK_PLAYER_HIJACK, [](librg_message *msg) {
    auto sender_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto seat = librg_data_ri32(msg->data);

    if (sender_ent && vehicle_ent && sender_ent->user_data && vehicle_ent->user_data) {
        auto sender = (mafia_player*)sender_ent->user_data;
        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

        if (vehicle->seats[seat] != -1) {

            auto driver_ent = librg_entity_fetch(&network_context, vehicle->seats[seat]);
            if (driver_ent && driver_ent->user_data) {
                auto driver = (mafia_player*)driver_ent->user_data;
                driver->vehicle_id = -1;
            }

            vehicle->seats[seat] = -1;

            if(vehicle->car && sender_ent->id != local_player.entity_id)
                sender->ped->Do_ThrowCocotFromCar(vehicle->car, seat);
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_USE_ACTOR, [](librg_message *msg) {
    auto sender_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto action = librg_data_ri32(msg->data);
    auto seat_id = librg_data_ri32(msg->data);
    auto unk3 = librg_data_ri32(msg->data);

    if(sender_ent && vehicle_ent && sender_ent->user_data && vehicle_ent->user_data) {
        auto sender = (mafia_player*)sender_ent->user_data;
        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

        if(action == 1) {
            vehicle->seats[seat_id] = sender_ent->id;
            sender->vehicle_id = vehicle_ent->id;
        } else if (action == 2) {
            vehicle->seats[seat_id] = -1;
            sender->vehicle_id = -1;
        }

        if(sender_ent->id != local_player.entity_id)
            sender->ped->Use_Actor(vehicle->car, action, seat_id, unk3);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_USE_DOORS, [](librg_message *msg) {
    auto sender_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto door_name_len = librg_data_ru32(msg->data);
    char door_name[32];
    librg_data_rptr(msg->data, door_name, door_name_len);
    door_name[door_name_len] = '\0';

    auto door_state = librg_data_ru32(msg->data);
    if (sender_ent && sender_ent->user_data) {
        auto sender = (mafia_player *)sender_ent->user_data;
        auto found_door = (MafiaSDK::C_Door*)MafiaSDK::GetMission()->FindActorByName(door_name);
        if(found_door && sender->ped) {
            hooks::door_setstate_orignal(found_door, (MafiaSDK::C_Door_Enum::States)door_state, sender->ped, TRUE, TRUE);
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_FROM_CAR, [](librg_message *msg) {
    
    auto sender_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto vehicle_ent = librg_entity_fetch(&network_context, librg_data_ru32(msg->data));
    auto seat_id = librg_data_ru32(msg->data);

    if (sender_ent && sender_ent->user_data && vehicle_ent && vehicle_ent->user_data) {
        auto sender = (mafia_player*)sender_ent->user_data;
        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
        
        sender->ped->Intern_FromCar();
        vehicle->seats[seat_id] = -1;
        sender->vehicle_id = -1;
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SHOOT, [](librg_message* msg) {
    zpl_vec3 pos;
    librg_entity_id id = librg_data_rent(msg->data);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));
    S_vector target = EXPAND_VEC(pos);

    auto entity = librg_entity_fetch(&network_context, id);
    if(entity) {
        auto player = (mafia_player*)entity->user_data;
        *(BYTE*)((DWORD)player->ped + 0x4A4) = 50;
        player->ped->Do_Shoot(true, target);
        player->ped->Do_Shoot(false, target);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_THROW_GRENADE, [](librg_message* msg) {
    zpl_vec3 pos;
    librg_entity_id id = librg_data_rent(msg->data);
    librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));
    S_vector target = EXPAND_VEC(pos);

    auto entity = librg_entity_fetch(&network_context, id);
    if(entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_ThrowGranade(target);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message* msg) {
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

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, [](librg_message* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_Reload();
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, [](librg_message* msg) {
    librg_entity_id id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        player->ped->Do_Holster();
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_DROP, [](librg_message* msg) {

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

librg_network_add(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message* msg) {
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
});

librg_network_add(&network_context, NETWORK_PLAYER_HIT, [](librg_message* msg) {

    librg_entity_id victim_id = librg_data_rent(msg->data);
    librg_entity_id sender_id = librg_data_rent(msg->data);

    auto sender_ent = librg_entity_fetch(&network_context, sender_id);
    auto victim_ent = librg_entity_fetch(&network_context, victim_id);

    u32 hit_type = librg_data_ru32(msg->data);
    S_vector unk1, unk2, unk3;
    librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
    librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));

    f32 damage = librg_data_rf32(msg->data);
    f32 health = librg_data_rf32(msg->data);
    u32 player_part = librg_data_ru32(msg->data);

    if (sender_ent && sender_ent->user_data && 
        victim_ent && victim_ent->user_data) {
        
        auto player = (mafia_player*)(sender_ent->user_data);
        auto victim = (mafia_player*)(victim_ent->user_data);

        hit_hook_skip = false;
        victim->ped->Hit(hit_type, unk1, unk2, unk3, damage, player->ped, player_part, NULL);
        hit_hook_skip = true;

        victim->ped->GetInterface()->health = victim->health = health;
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, [](librg_message *msg) {
    auto entity_id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    
    if (entity) {
        auto player = (mafia_player *)entity->user_data;

        if (player) {
            librg_data_rptr(msg->data, &player->inventory, sizeof(player_inventory));
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SET_POS, [](librg_message* msg) {
    auto entity_id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity) {
        librg_data_rptr(msg->data, &entity->position, sizeof(entity->position));

        auto player = (mafia_player *)entity->user_data;
        if (player && player->ped)
        {
            auto player_int = player->ped->GetInterface();
            player_int->entity.position = EXPAND_VEC(entity->position);
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SET_HEALTH, [](librg_message* msg) {
    auto entity_id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity) {
        auto health = librg_data_rf32(msg->data);

        auto player = (mafia_player*)entity->user_data;

        if (player) {
            player->health = health;

            if (player->ped) {
                auto player_int = player->ped->GetInterface();

                if (player->ped == get_local_ped())
                    MafiaSDK::GetIndicators()->PlayerSetWingmanLives((int)(health/2.0f));

                player_int->health = health;
            }
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SET_MODEL, [](librg_message* msg) {
    auto entity_id = librg_data_rent(msg->data);
    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity) {
        char modelName[32] = { 0 };
        librg_data_rptr(msg->data, modelName, sizeof(char) * 32);

        auto player = (mafia_player*)entity->user_data;

        if (player) {
            strncpy(player->model, modelName, 32);

            if (player->ped) {
                ((MafiaSDK::C_Human*)player->ped)->Intern_ChangeModel(modelName);
            }
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SET_ROT, [](librg_message* msg) {
    auto entity_id = librg_data_rent(msg->data);
    zpl_vec3 rot;
    librg_data_rptr(msg->data, &rot, sizeof(rot));

    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        if(player) {
            player->rotation = rot;
        }
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_SET_CAMERA, [](librg_message* msg) {
    
    S_vector pos, rot;
    librg_data_rptr(msg->data, &pos, sizeof(pos));
    librg_data_rptr(msg->data, &rot, sizeof(rot));

    if(MafiaSDK::GetMission()->GetGame()) {
        auto camera = MafiaSDK::GetMission()->GetGame()->GetCamera();
        camera->LockAt(pos, rot);
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_UNLOCK_CAMERA, [](librg_message* msg) {
    if(MafiaSDK::GetMission()->GetGame()) {
        auto camera = MafiaSDK::GetMission()->GetGame()->GetCamera();
        camera->Unlock();
    }
});

librg_network_add(&network_context, NETWORK_PLAYER_PLAY_ANIMATION, [](librg_message* msg) {
    auto entity_id = librg_data_rent(msg->data);
    char animation[32];
    librg_data_rptr(msg->data, animation, sizeof(char) * 32);

    auto entity = librg_entity_fetch(&network_context, entity_id);
    if (entity) {
        auto player = (mafia_player*)entity->user_data;
        if (player) {
            player->ped->Do_PlayAnim(animation);
        }
    }
});

librg_network_add(&network_context, NETWORK_SEND_CONSOLE_MSG, [](librg_message* msg) {
    u32 msg_size	= librg_data_ru32(msg->data);
    u32 msg_color	= librg_data_ru32(msg->data);
    char* text = reinterpret_cast<char*>(malloc(msg_size));
    librg_data_rptr(msg->data, text, msg_size);
    text[msg_size]  = '\0';
    MafiaSDK::GetIndicators()->ConsoleAddText(reinterpret_cast<const char*>(text), msg_color);
});

librg_network_add(&network_context, NETWORK_PLAYER_PUT_TO_VEHICLE, [](librg_message* msg) {
    auto player_ent = librg_entity_fetch(&network_context, librg_data_rent(msg->data));
    auto vehicle_id = librg_data_rent(msg->data);
    int seat_id = librg_data_ri32(msg->data);
    
    if (player_ent && player_ent->user_data) {
        auto player = (mafia_player*)player_ent->user_data;
        player->vehicle_id = vehicle_id;
        player->clientside_flags |= CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
    }
});
