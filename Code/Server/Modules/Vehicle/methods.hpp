librg_entity *spawn_vehicle(zpl_vec3 position, zpl_vec3 rotation, char *model, b32 show_in_radar) {

    mafia_vehicle *vehicle = new mafia_vehicle();

    strncpy(vehicle->model, model, strlen(model));
    vehicle->rot_forward = rotation;
    vehicle->health             = 100.0f;
    vehicle->engine_health      = 100.0f;
    vehicle->fuel               = 60.0f;
    vehicle->sound_enabled      = 1;
    vehicle->is_car_in_radar    = show_in_radar;
    vehicle->rot_up	            = { 0.0f, 1.0f, 0.0f };

    auto new_vehicle_entity		    = librg_entity_create(&network_context, TYPE_VEHICLE);
    new_vehicle_entity->position	= position;
    new_vehicle_entity->user_data	= vehicle;

    auto streamer = mod_get_nearest_player(&network_context, new_vehicle_entity->position);
    if (streamer != nullptr) {
        librg_entity_control_set(&network_context, new_vehicle_entity->id, streamer->client_peer);
    }

    return new_vehicle_entity;
}

void destroy_vehicle(librg_entity *entity) {
    if (entity == nullptr) return;

    if (gm.on_vehicle_destroyed)
        gm.on_vehicle_destroyed(entity);

    delete entity->user_data;
    entity->user_data = nullptr;
    librg_entity_destroy(&network_context, entity->id);
}

void set_pos(librg_entity *entity, zpl_vec3 position) {
    entity->position = position;

    librg_send(&network_context, NETWORK_VEHICLE_SET_POS, data, {
        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, &position, sizeof(position));
    });
}

void set_dir(librg_entity *entity, zpl_vec3 dir) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->rot_forward = dir;

    librg_send(&network_context, NETWORK_VEHICLE_SET_DIR, data, {
        librg_data_went(&data, entity->id);
        librg_data_wptr(&data, &dir, sizeof(dir));
    });
}

int get_player_seat_id(librg_entity *entity, librg_entity *player) {
    auto vehicle = (mafia_vehicle*)entity->user_data;

    for (size_t i = 0; i < 4; i++)
    {
        if (vehicle->seats[i] == player->id)
            return i;
    }

    return -1;
}

void set_radar_vis(librg_entity *entity, b32 state) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->is_car_in_radar = state;

    librg_send(&network_context, NETWORK_VEHICLE_RADAR_VISIBILITY, data, {
        librg_data_went(&data, entity->id);
        librg_data_wu8(&data, (u8)state);
    });
}
