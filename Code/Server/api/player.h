// =======================================================================//
// !
// ! General
// !
// =======================================================================//

/**
 * Allocate a player struct and store it in the player buffer (sike)
 * (should be used on successfull conenction)
 * @param  event
 * @return
 */
oak_player oak_player_create(librg_event *e) {
    auto oak_id = oak_entity_next(OAK_PLAYER);
    auto entity = oak_entity_player_get(oak_id);

    entity->librg_id = e->entity->id;
    entity->librg_entity = e->entity;
    entity->reset();

    /* store index of our entity in user_data */
    e->entity->stream_range = 500.0f;
    e->entity->user_data = cast(void*)(uintptr)oak_id;

    /* set our player to be our controller */
    librg_entity_control_set(&network_context, e->entity->id, e->peer);
    oak_log("[info] player connected with oakid: %d\n", oak_id);

    return oak_id;
}

/**
 * Deallocate player struct and remove it from the buffer
 * @param  event
 * @return
 */
int oak_player_destroy(librg_event *e) {
    return oak_entity_free(OAK_PLAYER, (uintptr)e->entity->user_data);
}

/**
 * Check if player is invalid
 * @param  id
 * @return
 */
int oak_player_invalid(oak_player id) {
    return oak_entity_invalid(OAK_PLAYER, id);
}

// =======================================================================//
// !
// ! Actions
// !
// =======================================================================//

/**
 * Spawn a player ped in the world
 * @param  id
 * @return
 */
int oak_player_spawn(oak_player id) {
    if (oak_player_invalid(id)) return -1;

    auto player = oak_entity_player_get(id);
    auto entity = player->librg_entity;

    librg_send(&network_context, NETWORK_PLAYER_SPAWN, data, {
        librg_data_wu32(&data, entity->id);
        librg_data_wptr(&data, &entity->position, sizeof(zpl_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        librg_data_wu32(&data, player->current_weapon_id);
        librg_data_wf32(&data, player->health);
    });

    return 0;
}

/**
 * Remove player ped from the world
 * @param  id
 * @return
 */
int oak_player_despawn(oak_player id) {
    ZPL_ASSERT_MSG(0, "oak_player_despawn: not implemented");
    return -1;
}

/**
 * Kill a specified player
 * @param  id
 * @return
 */
int oak_player_kill(oak_player id) {
    if (oak_player_invalid(id)) return -1;
    auto res = oak_player_health_set(id, 0.0f);
    auto player = oak_entity_player_get(id);

    if (player->vehicle_id != -1) {
        auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
        auto vehicle = oak_entity_vehicle_get((oak_vehicle)vehicle_ent->user_data);

        if (vehicle) {
            for (int i = 0; i < 4; i++) {
                if (vehicle->seats[i] == player->librg_id) {
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

    return res;
}

int oak_player_kick(oak_player id, const char *reason, int length) {
    ZPL_PANIC("oak_player_kick: not implemnented");
    return -1;
}

/**
 * Play a speicific animation for specific player
 * Sends message to all players
 * @param  id
 * @param  text
 * @return
 */
int oak_player_playanim(oak_player id, const char *text, int length) {
    if (oak_player_invalid(id)) return -1;

    auto entity = oak_entity_player_get(id);
    char animation[32] = {};
    zpl_strncpy(animation, text, length);

    librg_send(&network_context, NETWORK_PLAYER_PLAY_ANIMATION, data, {
        librg_data_went(&data, entity->librg_id);
        librg_data_wptr(&data, animation, sizeof(char) * 32);
    });

    return 0;
}

// =======================================================================//
// !
// ! STREAM SETTERS (auto-streamed values)
// !
// =======================================================================//

#define OAK_PLAYER_SETTER(TYPE, CAST, NAME, ALIAS)                                      \
    int ZPL_JOIN3(oak_player_,NAME,_set)(oak_player id, TYPE NAME) {                    \
        if (oak_player_invalid(id)) return -1;                                          \
                                                                                        \
        auto entity = oak_entity_player_get(id);                                        \
                                                                                        \
        /* skip updates for the next change */                                          \
        librg_entity_control_ignore_next_update(&network_context, entity->librg_id);    \
                                                                                        \
        /* apply new value */                                                           \
        entity->ALIAS = *((CAST*)&NAME);                                                \
                                                                                        \
        return 0;                                                                       \
    }

OAK_PLAYER_SETTER(oak_vec3, zpl_vec3, direction, rotation);

#undef OAK_PLAYER_SETTER

int oak_player_health_set(oak_player id, float health) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    librg_send(&network_context, NETWORK_PLAYER_SET_HEALTH, data, {
        librg_data_went(&data, entity->librg_id);
        librg_data_wf32(&data, health);
    });

    return 0;
}

int oak_player_position_set(oak_player id, oak_vec3 position) {
    if (oak_player_invalid(id)) return -1;
    oak_entity_player_get(id)->librg_entity->position = EXPAND_VEC(position);
    return 0;
}

// =======================================================================//
// !
// ! DATA SETTERS (action-based values)
// !
// =======================================================================//

int oak_player_heading_set(oak_player id, float angle) {
    ZPL_ASSERT_MSG(0, "oak_player_heading_set: not implemented");
    return -1;
}

/**
 * Set player model to a specific model
 * Event is sent to all users
 * @param  id
 * @param  model
 * @return
 */
int oak_player_model_set(oak_player id, const char *model, int length) {
    if (oak_player_invalid(id)) return -1;

    auto entity = oak_entity_player_get(id);

    zpl_memset(entity->model, 0, OAK_PLAYER_MODEL_SIZE);
    zpl_memcopy(entity->model, model, length);

    librg_send(&network_context, NETWORK_PLAYER_SET_MODEL, data, {
        librg_data_went(&data, entity->librg_id);
        librg_data_wptr(&data, (void *)entity->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
    });

    return 0;
}

// =======================================================================//
// !
// ! GETTERS
// !
// =======================================================================//

const char *oak_player_name_get(oak_player id) {
    return oak_player_invalid(id) ? nullptr : oak_entity_player_get(id)->name;
}

const char *oak_player_model_get(oak_player id) {
    return oak_player_invalid(id) ? nullptr : oak_entity_player_get(id)->model;
}

float oak_player_health_get(oak_player id) {
    return oak_player_invalid(id) ? -1.0f : oak_entity_player_get(id)->health;
}

oak_vec3 oak_player_position_get(oak_player id) {
    oak_vec3 temp = {-1,-1,-1};

    if (!oak_player_invalid(id))
        zpl_memcopy(&temp, &oak_entity_player_get(id)->position, sizeof(oak_vec3));

    return temp;
}

oak_vec3 oak_player_direction_get(oak_player id) {
    ZPL_ASSERT_MSG(0, "oak_player_heading_get: not implemented");
    return {-1,-1,-1};
}

float oak_player_heading_get(oak_player id) {
    ZPL_ASSERT_MSG(0, "oak_player_heading_get: not implemented");
    return -1.0f;
}

// =======================================================================//
// !
// ! PLAYER VISIBILITY
// !
// =======================================================================//

/**
 * Set different player visibility values
 * @param  id
 * @param  type
 * @param  state
 * @return
 */
int oak_player_visibility_set(oak_player id, oak_visiblity_type type, int state) {
    auto entity = oak_entity_player_get(id); ZPL_ASSERT_NOT_NULL(entity);

    switch (type) {
        case OAK_VISIBILITY_ICON: {
            entity->is_visible_on_map = state;

            librg_send(&network_context, NETWORK_PLAYER_MAP_VISIBILITY, data, {
                librg_data_went(&data, entity->librg_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_NAME: {
            entity->has_visible_nameplate = state;

            librg_send(&network_context, NETWORK_PLAYER_NAMEPLATE_VISIBILITY, data, {
                librg_data_went(&data, entity->librg_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        default:
            ZPL_ASSERT_MSG(0, "oak_player_visibility_set: specified visibility type is not implemented!");
            break;
    }

    return -1;
}

/**
 * Get current visibility value for specific type
 * @param  id
 * @param  type
 * @return
 */
int oak_player_visibility_get(oak_player id, oak_visiblity_type type) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    switch (type) {
        case OAK_VISIBILITY_ICON: return (int)entity->is_visible_on_map; break;
        case OAK_VISIBILITY_NAME: return (int)entity->has_visible_nameplate; break;
        default: return -1;
    }
}
























// void set_camera(librg_entity *entity, zpl_vec3 pos, zpl_vec3 rot)
// {
//     librg_send_to(&network_context, NETWORK_CAMERA_SET_POS, entity->client_peer, data, {
//         librg_data_wptr(&data, &pos, sizeof(pos));
//         librg_data_wptr(&data, &rot, sizeof(rot));
//     });
// }

// void unlock_camera(librg_entity *entity)
// {
//     librg_send_to(&network_context, NETWORK_CAMERA_UNLOCK, entity->client_peer, data, {});
// }

// void set_camera_target(librg_entity *entity, librg_entity *target)
// {
//     auto player = (mafia_player *)entity->user_data;
//     i32 id = -1;

//     if (player->spec_id != -1)
//     {
//         librg_entity_visibility_set_for(&network_context, entity->id, target->id, LIBRG_DEFAULT_VISIBILITY);
//     }

//     if (target)
//     {
//         id = target->id;

//         librg_entity_visibility_set_for(&network_context, entity->id, target->id, LIBRG_ALWAYS_VISIBLE);
//     }

//     player->spec_id = id;

//     librg_send_to(&network_context, NETWORK_CAMERA_TARGET, entity->client_peer, data, {
//         librg_data_went(&data, id);
//     });
// }

// void send_announcement(librg_entity *entity, const char *text, f32 duration)
// {
//     auto len = strlen(text);
//     librg_send_to(&network_context, NETWORK_HUD_ALERT, entity->client_peer, data, {
//         librg_data_wu32(&data, len);
//         librg_data_wf32(&data, duration);
//         librg_data_wptr(&data, (void *)text, len);
//     });
// }

// void send_race_start_flags(librg_entity *entity, u32 flags)
// {

//     librg_send_to(&network_context, NETWORK_HUD_COUNTDOWN, entity->client_peer, data, {
//         librg_data_wu32(&data, flags);
//     });
// }

// void set_pos(librg_entity *entity, zpl_vec3 position)
// {
//     entity->position = position;
//     librg_send(&network_context, NETWORK_PLAYER_SET_POS, data, {
//         librg_data_went(&data, entity->id);
//         librg_data_wptr(&data, &position, sizeof(position));
//     });
// }

// void set_rot(librg_entity *entity, zpl_vec3 rotation)
// {
//     auto player = (mafia_player *)(entity->user_data);

//     if (player)
//     {
//         player->rotation = rotation;
//     }

//     librg_send(&network_context, NETWORK_PLAYER_SET_ROT, data, {
//         librg_data_went(&data, entity->id);
//         librg_data_wptr(&data, &rotation, sizeof(rotation));
//     });
// }

// void set_health(librg_entity *entity, float health)
// {
//     if (health == 0.0f)
//     {
//         die(entity);
//     }
//     else
//     {
//         auto player = (mafia_player *)entity->user_data;
//         player->health = health;

//         librg_send(&network_context, NETWORK_PLAYER_SET_HEALTH, data, {
//             librg_data_went(&data, entity->id);
//             librg_data_wf32(&data, health);
//         });
//     }
// }

// void set_model(librg_entity *entity, char *modelName)
// {
//     auto player = (mafia_player *)entity->user_data;

//     strncpy(player->model, modelName, 32);

//     librg_send(&network_context, NETWORK_PLAYER_SET_MODEL, data, {
//         librg_data_went(&data, entity->id);
//         librg_data_wptr(&data, (void *)player->model, sizeof(char) * 32);
//     });
// }

// void fadeout(librg_entity *entity, bool fadeout, u32 duration, u32 color)
// {
//     librg_send_to(&network_context, NETWORK_HUD_FADEOUT, entity->client_peer, data, {
//         librg_data_wu8(&data, fadeout);
//         librg_data_wu32(&data, duration);
//         librg_data_wu32(&data, color);
//     });
// }



/* Inventory */

// librg_entity *spawn_weapon_drop(zpl_vec3 position, char *model, inventory_item item)
// {
//     mafia_weapon_drop *drop = new mafia_weapon_drop();

//     drop->weapon = item;
//     strncpy(drop->model, model, strlen(model));

//     auto new_weapon_entity = librg_entity_create(&network_context, TYPE_WEAPONDROP);
//     new_weapon_entity->position = position;
//     new_weapon_entity->position.y += 0.7f;
//     new_weapon_entity->user_data = drop;

//     return new_weapon_entity;
// }

// inline auto inventory_debug(librg_entity *player_ent) -> void
// {
//     auto player = (mafia_player *)player_ent->user_data;
//     printf("-----------[INV]-----------\n");
//     for (size_t i = 0; i < 8; i++)
//     {
//         auto daco = player->inventory.items[i];
//         printf("%d %d %d\n", daco.weaponId, daco.ammoLoaded, daco.ammoHidden);
//     }
//     printf("Current wep: %d\n", player->current_weapon_id);
//     printf("-----------[INV]-----------\n");
// }

// inline auto inventory_full(librg_entity *player_ent) -> bool
// {
//     auto player = (mafia_player *)player_ent->user_data;
//     for (size_t i = 0; i < 8; i++)
//     {
//         if (player->inventory.items[i].weaponId == -1)
//             return false;
//     }
//     return true;
// }

// inline auto inventory_exists(librg_entity *player_ent, int id) -> bool
// {
//     auto player = (mafia_player *)player_ent->user_data;
//     for (size_t i = 0; i < 8; i++)
//     {
//         auto item = player->inventory.items[i];
//         if (item.weaponId == id)
//         {
//             return true;
//         }
//     }
//     return false;
// }

// inline auto inventory_add(
//     librg_entity *player_ent,
//     inventory_item *item,
//     bool announce = false,
//     bool weapon_picked = false) -> void
// {

//     auto player = (mafia_player *)player_ent->user_data;

//     //inventory full dont add weapon
//     if (inventory_full(player_ent))
//     {
//         mod_debug("player_inventory_add inv full !");
//         return;
//     }

//     //if weapon exists skip
//     if (inventory_exists(player_ent, item->weaponId))
//     {
//         mod_debug("player_inventory_add weapon exists !");
//         return;
//     }

//     //serverside insert weapon into free slot
//     for (size_t i = 0; i < 8; i++)
//     {
//         auto cur_item = player->inventory.items[i];
//         if (cur_item.weaponId == -1)
//         {
//             player->inventory.items[i] = *item;
//             break;
//         }
//     }

//     //set new weapon as current
//     player->current_weapon_id = item->weaponId;

//     //non announce if we adding weapon while spawning
//     if (!announce)
//         return;

//     //check if weapon is picked case
//     if (weapon_picked)
//     {

//         librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, player_ent->client_peer, data, {
//             librg_data_went(&data, player_ent->id);
//             librg_data_wptr(&data, item, sizeof(inventory_item));
//         });

//         mod_debug("sending weapon pickup");
//         return;
//     }

//     //broadcast to others for giving a weapon
//     librg_send(&network_context, NETWORK_PLAYER_WEAPON_ADD, data, {
//         librg_data_went(&data, player_ent->id);
//         librg_data_wptr(&data, item, sizeof(inventory_item));
//     });
// }

// inline auto inventory_remove(
//     librg_entity *player_ent,
//     int id,
//     bool announce = false,
//     bool weapon_dropped = false) -> void
// {

//     //do weapon exists in inventory ?
//     if (!inventory_exists(player_ent, id))
//         return;

//     //remove weapon from inventory
//     auto player = (mafia_player *)player_ent->user_data;
//     for (size_t i = 0; i < 8; i++)
//     {
//         auto item = player->inventory.items[i];
//         if (item.weaponId == id)
//         {

//             //if removed weapon was current set hands
//             if (item.weaponId == player->current_weapon_id)
//                 player->current_weapon_id = 0;

//             player->inventory.items[i] = {-1, 0, 0, 0};
//         }
//     }

//     if (!announce)
//         return;

//     if (weapon_dropped)
//     {
//         librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_DROP, player_ent->client_peer, data, {
//             librg_data_went(&data, player_ent->id);
//             librg_data_wu32(&data, id);
//         });
//         return;
//     }

//     //broadcast message
//     librg_send(&network_context, NETWORK_PLAYER_WEAPON_REMOVE, data, {
//         librg_data_went(&data, player_ent->id);
//         librg_data_wu32(&data, id);
//     });
// }
