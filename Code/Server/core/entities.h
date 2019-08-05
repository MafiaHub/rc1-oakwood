zpl_global struct {
    mafia_player   players[OAK_MAX_PLAYERS];
    mafia_vehicle  vehicles[OAK_MAX_VEHICLES];
    mafia_door     doors[OAK_MAX_DOORS];

    u32 ids[OAK_ENTITY_TYPE_LAST];
    u32 counts[OAK_ENTITY_TYPE_LAST];
    u32 cursors[OAK_ENTITY_TYPE_LAST];
} oak__entities_data = {};

zpl_global u32 oak_ent_limits[OAK_ENTITY_TYPE_LAST] = {
    OAK_MAX_PLAYERS,
    OAK_MAX_VEHICLES,
    OAK_MAX_WEAPON_DROPS,
    OAK_MAX_DOORS,
    OAK_MAX_NPCS,
};

int oak_entities_init() {
    zpl_memset(&oak__entities_data, 0, sizeof(oak__entities_data));
    return 0;
}

int oak_entities_count_get(oak_type type) {
    ZPL_ASSERT(type >= 0 && type < OAK_ENTITY_TYPE_LAST);

    return oak__entities_data.counts[type];
}

/**
 * Iterate and find new free entity slot
 * @param  type
 * @return
 */
u32 oak_entity_next(oak_type type) {
    ZPL_ASSERT(type >= 0 && type < OAK_ENTITY_TYPE_LAST);
    ZPL_ASSERT_MSG(oak__entities_data.counts[type] < oak_ent_limits[type], "reached max_entities limit");

    ++oak__entities_data.counts[type];

    if (oak__entities_data.cursors[type] >= (oak_ent_limits[type] - 1) || oak_ent_limits[type] == 0) {
        oak__entities_data.cursors[type] = 0;
    }

    for (; oak__entities_data.cursors[type] < oak_ent_limits[type]; ++oak__entities_data.cursors[type]) {
        auto id = oak__entities_data.cursors[type];
        auto entity = oak_entity_get(type, id);
        ZPL_ASSERT(entity);

        if (entity->is_valid) continue;
        zpl_zero_item(entity);

        entity->oak_id = id;
        entity->is_valid = 1;

        return id;
    }

    return -1;
}

/**
 * Fetch oak_object from speicified id (if any)
 * @param  type
 * @param  id
 * @return
 */
oak_object *oak_entity_get(oak_type type, u32 id) {
    ZPL_ASSERT(type >= 0 && type < OAK_ENTITY_TYPE_LAST);
    ZPL_ASSERT(id < oak_ent_limits[type]);

    switch (type) {
        case OAK_PLAYER: return &oak__entities_data.players[id]; break;
        case OAK_VEHICLE: return &oak__entities_data.vehicles[id]; break;
        case OAK_DOOR: return &oak__entities_data.doors[id]; break;
        default:
            ZPL_PANIC("oak_entity_get: specified entity is not handled");
            break;
    }

    return nullptr;
}

/**
 * Retrieve an ID stored in a librg_entity
 * @param  entity
 * @return
 */
int oak_entity_get_id_from_native(librg_entity *entity) {
    ZPL_ASSERT_NOT_NULL(entity);

    return (int)(uintptr)(entity->user_data);
}

/**
 * Check if specified entity id is valid
 * @param  oak_type
 * @param  id
 * @return
 */
int oak_entity_invalid(oak_type type, u32 id) {
    if (!(type >= 0 && type < OAK_ENTITY_TYPE_LAST))
        return 1;

    if (!(id < oak_ent_limits[type]))
        return 1;

    auto entity = oak_entity_get(type, id);
    return !entity->is_valid;
}

/**
 * Free specified entity slot
 * @param  type
 * @param  id
 * @return
 */
int oak_entity_free(oak_type type, u32 id) {
    ZPL_ASSERT(type >= 0 && type < OAK_ENTITY_TYPE_LAST);
    ZPL_ASSERT(oak__entities_data.counts[type] > 0);

    auto entity = oak_entity_get(type, id);
    entity->is_valid = 0;

    oak__entities_data.counts[type]--;

    return 0;
}

/* implement entity selects */

#define OAK_ENTITY_SELECTOR(TYPE, NAME, ETYPE)                          \
    TYPE *ZPL_JOIN3(oak_entity_,NAME,_get)(ZPL_JOIN2(oak_,NAME) id) {   \
        ZPL_ASSERT(id >= 0 && id < oak_ent_limits[ETYPE]);              \
        auto entity = &oak__entities_data.ZPL_JOIN2(NAME,s)[id];        \
        return (TYPE *)(entity->is_valid ? entity : nullptr);           \
    }

OAK_ENTITY_SELECTOR(mafia_player, player, OAK_PLAYER);
OAK_ENTITY_SELECTOR(mafia_vehicle, vehicle, OAK_VEHICLE);
OAK_ENTITY_SELECTOR(mafia_door, door, OAK_DOOR);

#undef OAK_ENTITY_SELECTOR

/* implement entity lists */

#define OAK_ENTITY_LIST(NAME, MAX_LIMIT)                            \
ZPL_JOIN2(oak_, NAME) *ZPL_JOIN3(oak_,NAME,_list)(int *count) {     \
    zpl_local_persist ZPL_JOIN2(oak_, NAME) buffer[MAX_LIMIT] = {}; \
    int length = 0;                                                 \
                                                                    \
    /* clean up old data */                                         \
    zpl_memset(buffer, 0, MAX_LIMIT*sizeof(int));                   \
                                                                    \
    for (int i=0; i<MAX_LIMIT; ++i)                                 \
        if (!ZPL_JOIN3(oak_,NAME,_invalid)(i))                      \
            buffer[length++] = i;                                   \
                                                                    \
    if (count) *count = length;                                     \
    return buffer;                                                  \
}


OAK_ENTITY_LIST(player, OAK_MAX_PLAYERS);
OAK_ENTITY_LIST(vehicle, OAK_MAX_VEHICLES);
OAK_ENTITY_LIST(door, OAK_MAX_DOORS);

#undef OAK_ENTITY_LIST
