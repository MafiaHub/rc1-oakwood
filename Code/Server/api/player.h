// =======================================================================//
// !
// ! General
// !
// =======================================================================//

struct weapon {
    const char* model;
    inventory_item item;
};

weapon weapons[] = {
    {NULL, NULL}, // DO NOT USE
    {NULL, NULL}, // DO NOT USE
    {"2boxer.i3d", {2, 0, 0, 0}}, // Boxer (Knuckle Duster)
    {"2knife.i3d", {3, 0, 0, 0}}, // Knife
    {"2bbat1.i3d", {4, 0, 0, 0}}, // Baseball Bat
    {"2mol.i3d", {5, 0, 0, 0}}, // Molotov
    {"2coltDS.i3d", {6, 6, 30, 0}}, // Colt Detective Special
    {"2sw27.i3d", {7, 6, 30, 0}}, // S&W Model 27 Magnum
    {"2sw10.i3d", {8, 6, 30, 0}}, // S&W Model 10 M&P
    {"2c1911.i3d", {9, 7, 35, 0}}, // Colt 1911
    {"2tommy.i3d", {10, 50, 200, 0}}, // Tommy Gun
    {"2shotgun.i3d", {11, 8, 32, 0}}, // Pump-action Shotgun
    {"2sawoff2.i3d", {12, 2, 20, 0}}, // Sawnoff Shotgun
    {"2m1903.i3d", {13, 5, 20, 0}}, // US Rifle M1903 Springfield
    {"2mosin.i3d", {14, 5, 20, 0}}, // Mosin-Nagant 1891/30
    {"2grenade.i3d", {15, 0, 0, 0}}, // Grenade
    {"2key.i3d", {16, 0, 0, 0}}, // Key
    {"2bucket.i3d", {17, 0, 0, 0}}, // Bucket
    {"2flshlight.i3d", {18, 0, 0, 0}}, // Flashlight
    {"2xkniha 7.i3d", {19, 0, 0, 0}}, // Book (Documents)
    {"2bar.i3d", {20, 0, 0, 0}}, // Bar
    {"2papers.i3d", {21, 0, 0, 0}}, // Papers
    {"2bomb.i3d", {22, 0, 0, 0}}, // Dat BOI, which goes BOOM.
    {"9klice.i3d", {23, 0, 0, 0}}, // Door Keys
    {"9klic1.i3d", {24, 0, 0, 0}}, // Safe Key
    {"2crowbar.i3d", {25, 0, 0, 0}}, // Crowbar (λ STYLE)
    {"fmv9letenka.i3d", {26, 0, 0, 0}}, // Fly Tickets
    {"9balik.i3d", {27, 0, 0, 0}}, // Box (Balík)
    {"2prkno.i3d", {28, 0, 0, 0}}, // Plank (Board)
    {"2cbottle.i3d", {29, 0, 0, 0}}, // Broken Bottle
    {"9klic1.i3d", {30, 0, 0, 0}}, // Small Key
    {"2swr.i3d", {31, 0, 0, 0}}, // Sword
    {"hlavapsa.i3d", {32, 0, 0, 0}}, // Dog's head
};

/**
 * Allocate a player struct and store it in the player buffer (sike)
 * (should be used on successful connection)
 * @param  event
 * @return
 */
oak_player oak_player_create(librg_event *e) {
    auto oak_id = oak_entity_next(OAK_PLAYER);
    auto entity = oak_entity_player_get(oak_id);

    entity->native_id = e->entity->id;
    entity->native_entity = e->entity;
    entity->reset();

    /* store index of our entity in user_data */
    e->entity->stream_range = 500.0f;
    e->entity->user_data = cast(void*)(uintptr)oak_id;
    oak_player_visibility_set(oak_id, OAK_VISIBILITY_ICON, 1);

    /* set our player to be our controller */
    librg_entity_control_set(oak_network_ctx_get(), e->entity->id, e->peer);
    oak_log("^F[^5INFO^F] Player connected with oakID: ^A%d^R\n", oak_id);

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
int oak_player_spawn(oak_player id, oak_vec3 position, float heading) {
    if (oak_player_invalid(id)) return -1;

    auto player = oak_entity_player_get(id);
    auto entity = player->native_entity;

    /* use player var as temp storage */
    player->rotation = ComputeDirVector(heading);

    for (size_t i = 0; i < 8; i++)
    {
        player->inventory.items[i].weaponId = 0;
        player->inventory.items[i].ammoLoaded = 0;
        player->inventory.items[i].ammoHidden = 0;
    }

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SPAWN, data, {
        librg_data_wu32(&data, entity->id);
        librg_data_wptr(&data, &position, sizeof(oak_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * OAK_PLAYER_MODEL_SIZE);
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
    if (oak_player_invalid(id)) return -1;

    auto player = oak_entity_player_get(id);

    if (player->vehicle_id != -1) {
        if (librg_entity_valid(oak_network_ctx_get(), player->vehicle_id)) {
            auto vid = oak_entity_vehicle_get_from_native(
                librg_entity_fetch(oak_network_ctx_get(), player->vehicle_id))->oak_id;

            oak_vehicle_player_remove(vid, player->oak_id);
        }
    }

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_DESPAWN, data, {
        librg_data_went(&data, player->native_id);
    });

    return 0;
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
        auto vehicle_ent = librg_entity_fetch(oak_network_ctx_get(), player->vehicle_id);

        if (!vehicle_ent) {
            player->vehicle_id = -1;
            return -2;
        }

        auto vehicle = oak_entity_vehicle_get_from_native(vehicle_ent);

        if (vehicle) {
            oak_vehicle_player_remove(vehicle->oak_id, player->oak_id);
        }
    }

    return res;
}

int oak_player_kick(oak_player id, const char *reason, int length) {
    // todo: send reason

    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    librg_network_kick(oak_network_ctx_get(), player->native_entity->client_peer);
    return 0;
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

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_PLAY_ANIMATION, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, animation, sizeof(char) * 32);
    });

    return 0;
}

// =======================================================================//
// !
// ! STREAM SETTERS (auto-streamed values)
// !
// =======================================================================//

int oak_player_health_set(oak_player id, float health) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    entity->health = health;

    if (entity->died_ingame) {
        librg_send_except(oak_network_ctx_get(), NETWORK_PLAYER_SET_HEALTH, entity->native_entity->client_peer, data, {
            librg_data_went(&data, entity->native_id);
            librg_data_wf32(&data, health);
        });
    }
    else {
        librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_HEALTH, data, {
            librg_data_went(&data, entity->native_id);
            librg_data_wf32(&data, health);
        });
    }

    return 0;
}

int oak_player_position_set(oak_player id, oak_vec3 position) {
    if (oak_player_invalid(id)) return -1;
    auto entity = oak_entity_player_get(id);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_POS, data, {
        librg_data_went(&data, entity->native_id);
        librg_data_wptr(&data, &position, sizeof(zpl_vec3));
    });

    return 0;
}

int oak_player_direction_set(oak_player id, oak_vec3 direction) {
    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    player->rotation = EXPAND_VEC(direction);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_ROT, data, {
        librg_data_went(&data, player->native_id);
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
    });

    return 0;
}

int oak_player_heading_set(oak_player id, float angle) {
    auto player = oak_entity_player_get(id);

    if (!player) {
        return -1;
    }

    player->rotation = ComputeDirVector(angle);

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_ROT, data, {
        librg_data_went(&data, player->native_id);
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
    });

    return 0;
}

int oak_player_give_weapon(oak_player id, int weapId, int ammoLoaded, int ammoInInventory)
{
    if (oak_player_invalid(id)) return -1;
    auto player = oak_entity_player_get(id);

    inventory_item gun = weapons[weapId].item;

    gun.ammoHidden = ammoInInventory;
    gun.ammoLoaded = ammoLoaded;

    for (size_t i = 0; i < 8; i++)
    {
        auto cur_item = player->inventory.items[i];
        if (cur_item.weaponId == -1) // If player doesn't hold any weapon
        {
            player->inventory.items[i] = gun;
            break;
        }
        else if (i == 7) // If inventory is full, so just give the weapon into the last slot in Inventory
        {
            player->inventory.items[i] = gun;
        }
    }

    player->current_weapon_id = gun.weaponId;

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_WEAPON_ADD, data, {
        librg_data_wu32(&data, player->native_entity->id);
        librg_data_wi32(&data, weapId);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        });

    return 0;
}

int oak_player_remove_weapon(oak_player id, int weapId)
{
    if (oak_player_invalid(id)) return -1;
    auto player = oak_entity_player_get(id);

    for (size_t i = 0; i < 8; i++)
    {
        auto cur_item = player->inventory.items[i];
        if (cur_item.weaponId == weapId)
        {
            player->inventory.items[i].weaponId = 0;
            player->inventory.items[i].ammoLoaded = 0;
            player->inventory.items[i].ammoHidden = 0;
        }
    }

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_WEAPON_REMOVE, data, {
        librg_data_wu32(&data, player->native_entity->id);
        librg_data_wi16(&data, (short)weapId);
        });

    return 0;
}

// =======================================================================//
// !
// ! DATA SETTERS (action-based values)
// !
// =======================================================================//

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

    librg_send(oak_network_ctx_get(), NETWORK_PLAYER_SET_MODEL, data, {
        librg_data_went(&data, entity->native_id);
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
    int code = oak_player_invalid(id);

    /* entity is valid, or exists, but marked as invalid */
    /* extract raw data to access values even if player removed (disconnected) */
    if (code == 0 || code == 3) {
        return oak__entities_data.players[id].name;
    }

    return nullptr;
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
    oak_vec3 temp = { -1,-1,-1 };

    if (!oak_player_invalid(id))
        zpl_memcopy(&temp, &oak_entity_player_get(id)->rotation, sizeof(oak_vec3));

    return temp;
}

float oak_player_heading_get(oak_player id) {
    float temp = -1.0f;

    if (!oak_player_invalid(id))
        temp = DirToRotation180(oak_entity_player_get(id)->rotation);

    return temp;
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

            librg_send(oak_network_ctx_get(), NETWORK_PLAYER_MAP_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
                librg_data_wu8(&data, (u8)state);
            });

            return 0;
        } break;

        case OAK_VISIBILITY_NAME: {
            entity->has_visible_nameplate = state;

            librg_send(oak_network_ctx_get(), NETWORK_PLAYER_NAMEPLATE_VISIBILITY, data, {
                librg_data_went(&data, entity->native_id);
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
