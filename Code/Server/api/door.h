oak_door oak_door_create(const char *name, int length) {
    auto oak_id = oak_entity_next(OAK_DOOR);
    auto entity = oak_entity_door_get(oak_id);
    auto native = librg_entity_create(&network_context, OAK_DOOR);

    entity->librg_id = native->id;
    entity->librg_entity = native;
    native->user_data = (void *)(uintptr)oak_id;
    native->position = {0};

    entity->angle       = 0.0f;
    entity->open_side   = 0;

    zpl_memset(entity->name, 0, OAK_MAX_DOOR_NAME_SIZE);
    zpl_memcopy(entity->name, name, length);

    return oak_id;
}

int oak_door_destroy(oak_door id) {
    ZPL_PANIC("oak_door_destroy: not implemented");
    return -1;
}

int oak_door_invalid(oak_door id) {
    return oak_entity_invalid(OAK_DOOR, id);
}

const char *oak_door_name_get(oak_door id) {
    return oak_door_invalid(id) ? nullptr : oak_entity_door_get(id)->name;
}

float oak_door_angle_get(oak_door id) {
    return oak_door_invalid(id) ? -1.0f : oak_entity_door_get(id)->angle;
}
