/* Player */

inline auto send_spawn(librg_entity* player_ent) -> void {
    auto player = (mafia_player*)player_ent->user_data;
    librg_send(&network_context, NETWORK_PLAYER_SPAWN, data, {
        librg_data_wu32(&data, player_ent->id);
        librg_data_wptr(&data, &player_ent->position, sizeof(zpl_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * 32);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        librg_data_wu32(&data, player->current_weapon_id);
        librg_data_wf32(&data, player->health);
    });
}

/* Inventory */

librg_entity *spawn_weapon_drop(zpl_vec3 position, char *model, inventory_item item) {
    mafia_weapon_drop *drop = new mafia_weapon_drop();

    drop->weapon = item;
    strncpy(drop->model, model, strlen(model));

    auto new_weapon_entity			= librg_entity_create(&network_context, TYPE_WEAPONDROP);
    new_weapon_entity->position		= position;
    new_weapon_entity->position.y	+= 0.7f;
    new_weapon_entity->user_data	= drop;

    return new_weapon_entity;
}

inline auto inventory_debug(librg_entity* player_ent) -> void {
    auto player = (mafia_player*)player_ent->user_data;
    printf("-----------[INV]-----------\n");
    for (size_t i = 0; i < 8; i++) {
        auto daco = player->inventory.items[i];
        printf("%d %d %d\n", daco.weaponId, daco.ammoLoaded, daco.ammoHidden);
    }
    printf("Current wep: %d\n", player->current_weapon_id);
    printf("-----------[INV]-----------\n");
}

inline auto inventory_full(librg_entity* player_ent) -> bool {
    auto player = (mafia_player*)player_ent->user_data;
    for (size_t i = 0; i < 8; i++) {
        if (player->inventory.items[i].weaponId == -1)
            return false;
    }
    return true;
}

inline auto inventory_exists(librg_entity* player_ent, int id) -> bool {
    auto player = (mafia_player*)player_ent->user_data;
    for (size_t i = 0; i < 8; i++) {
        auto item = player->inventory.items[i];
        if (item.weaponId == id) {
            return true;
        }
    }
    return false;
}

inline auto inventory_add(
    librg_entity* player_ent, 
    inventory_item* item, 
    bool announce = false, 
    bool weapon_picked = false) -> void {

    auto player = (mafia_player*)player_ent->user_data;

    //inventory full dont add weapon
    if (inventory_full(player_ent)) {
        mod_debug("player_inventory_add inv full !");
        return;
    }

    //if weapon exists skip
    if (inventory_exists(player_ent, item->weaponId)) {
        mod_debug("player_inventory_add weapon exists !");
        return;
    }

    //serverside insert weapon into free slot
    for (size_t i = 0; i < 8; i++) {
        auto cur_item = player->inventory.items[i];
        if (cur_item.weaponId == -1) {
            player->inventory.items[i] = *item;
            break;
        }
    }

    //set new weapon as current
    player->current_weapon_id = item->weaponId;

    //non announce if we adding weapon while spawning 
    if (!announce) return;

    //check if weapon is picked case
    if (weapon_picked) {

        librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, player_ent->client_peer, data, {
            librg_data_went(&data, player_ent->id);
            librg_data_wptr(&data, item, sizeof(inventory_item));
        });

        mod_debug("sending weapon pickup");
        return;
    }

    //broadcast to others for giving a weapon
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_ADD, data, {
        librg_data_went(&data, player_ent->id);
        librg_data_wptr(&data, item, sizeof(inventory_item));
    });
}

inline auto inventory_remove(
    librg_entity* player_ent, 
    int id, 
    bool announce = false,
    bool weapon_dropped = false) -> void {

    //do weapon exists in inventory ?
    if (!inventory_exists(player_ent, id)) return;
    
    //remove weapon from inventory
    auto player = (mafia_player*)player_ent->user_data;
    for (size_t i = 0; i < 8; i++) {
        auto item = player->inventory.items[i];
        if (item.weaponId == id) {

            //if removed weapon was current set hands
            if(item.weaponId == player->current_weapon_id)
                player->current_weapon_id = 0;

            player->inventory.items[i] = { -1, 0, 0, 0 };
        }
    }

    if (!announce) return;
    
    if (weapon_dropped) {
        librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_DROP, player_ent->client_peer, data, {
            librg_data_went(&data, player_ent->id);
            librg_data_wu32(&data, id);
        });
        return;
    }

    //broadcast message
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_REMOVE, data, {
        librg_data_went(&data, player_ent->id);
        librg_data_wu32(&data, id);
    });
}

void die(librg_entity *entity, b32 forced = false) {
    auto player = (mafia_player*)entity->user_data;
    player->health = 0.0f;

    if (player->vehicle_id != -1) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == entity->id) {
                    vehicle->seats[i] = -1;
                    if (i == 0) {
                        mod_vehicle_assign_nearest_player(&network_context, vehicle_ent);
                    }
                    break;
                }
            }
        }
        
        player->vehicle_id = -1;
    }

    if (gm.on_player_died)
        gm.on_player_died(entity, player);
}

b32 put_to_vehicle(librg_entity *entity, librg_entity *vehicle_ent, int seat_id) {
    auto player = (mafia_player*)entity->user_data;
    auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

    if (vehicle->seats[seat_id] != -1)
        return false;

    if (player->vehicle_id != -1)
        return false;

    if (seat_id < 0 || seat_id > 3)
        return false;

    vehicle->seats[seat_id] = entity->id;
    player->vehicle_id = vehicle_ent->id;

    if (seat_id == 0 && vehicle->seats[0] == -1) {
        librg_entity_control_set(&network_context, vehicle_ent->id, entity->client_peer);
    }
    
    librg_send(&network_context, NETWORK_PLAYER_PUT_TO_VEHICLE, data, {
        librg_data_went(&data, entity->id);
        librg_data_went(&data, vehicle_ent->id);
        librg_data_wi32(&data, seat_id);
    });

    return true;
}

void play_anim(librg_entity *entity, const char *text) {
    librg_send(&network_context, NETWORK_PLAYER_PLAY_ANIMATION, data, {        
        char animation[32];
        strcpy(animation, text);

        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, animation, sizeof(char) * 32);
    });
}

void set_camera(librg_entity *entity, zpl_vec3 pos, zpl_vec3 rot) {
    librg_send_to(&network_context, NETWORK_PLAYER_SET_CAMERA, entity->client_peer, data, {
        librg_data_wptr(&data, &pos, sizeof(pos));
        librg_data_wptr(&data, &rot, sizeof(rot));
    });
}

void set_camera_target(librg_entity* entity, librg_entity* target) {
    i32 id = -1;

    if (target) {
        id = target->id;
    }

    librg_send_to(&network_context, NETWORK_PLAYER_SET_CAMERA_TARGET, entity->client_peer, data, {
        librg_data_went(&data, id);
    });
}

void unlock_camera(librg_entity *entity) {
    librg_send_to(&network_context, NETWORK_PLAYER_UNLOCK_CAMERA, entity->client_peer, data, {});
}

void send_announcement(librg_entity* entity, const char* text, f32 duration) {
    auto len = strlen(text);
    librg_send_to(&network_context, NETWORK_PLAYER_SEND_ANNOUNCEMENT, entity->client_peer, data, {
        librg_data_wu32(&data, len);
        librg_data_wf32(&data, duration);
        librg_data_wptr(&data, (void*)text, len);
    });
}

void send_race_start_flags(librg_entity* entity, u32 flags) {
    
    librg_send_to(&network_context, NETWORK_PLAYER_SEND_RACE_START_FLAGS, entity->client_peer, data, {
       librg_data_wu32(&data, flags);
    });
}

void set_pos(librg_entity *entity, zpl_vec3 position) {
    entity->position = position;
    librg_send(&network_context, NETWORK_PLAYER_SET_POS, data, {
        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, &position, sizeof(position));
    });
}

void set_rot(librg_entity *entity, zpl_vec3 rotation) {
    auto player = (mafia_player*)(entity->user_data);

    if(player) {
        player->rotation = rotation;
    }

    librg_send(&network_context, NETWORK_PLAYER_SET_ROT, data, {
        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, &rotation, sizeof(rotation));
    });
}

void set_health(librg_entity *entity, float health) {
    if (health == 0.0f) {
        die(entity);
    } else {
        auto player = (mafia_player*)entity->user_data;
        player->health = health;

        librg_send(&network_context, NETWORK_PLAYER_SET_HEALTH, data, {
            librg_data_went(&data, entity->id);
            librg_data_wf32(&data, health);
        });
    }
}

void set_model(librg_entity *entity, char *modelName) {
    auto player = (mafia_player *)entity->user_data;

    strncpy(player->model, modelName, 32);

    librg_send(&network_context, NETWORK_PLAYER_SET_MODEL, data, {
        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, (void*)player->model, sizeof(char) * 32);
    });
}

void fadeout(librg_entity *entity, bool fadeout, u32 duration, u32 color) {
    librg_send_to(&network_context, NETWORK_SEND_FADEOUT, entity->client_peer, data, {
        librg_data_wu8(&data, fadeout);
        librg_data_wu32(&data, duration);
        librg_data_wu32(&data, color);
    });
}

librg_entity *get_vehicle(librg_entity *entity) {
    auto player = (mafia_player*)entity->user_data;

    if (player->vehicle_id != -1) {
        auto vehicle = librg_entity_fetch(&network_context, player->vehicle_id);
        return vehicle;
    }

    return nullptr;
}

void set_map_vis(librg_entity *entity, b32 state) {
    auto player = (mafia_player*)entity->user_data;
    player->is_visible_on_map = state;

    librg_send(&network_context, NETWORK_PLAYER_MAP_VISIBILITY, data, {
        librg_data_went(&data, entity->id);
        librg_data_wu8(&data, (u8)state);
    });
}

void set_nameplate_vis(librg_entity* entity, b32 state) {
    auto player = (mafia_player*)entity->user_data;
    player->has_visible_nameplate = state;

    librg_send(&network_context, NETWORK_PLAYER_NAMEPLATE_VISIBILITY, data, {
        librg_data_went(&data, entity->id);
        librg_data_wu8(&data, (u8)state);
    });
}
