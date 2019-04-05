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

    mod_vehicle_assign_nearest_player(&network_context, new_vehicle_entity);
    
    return new_vehicle_entity;
}

void destroy_vehicle(librg_entity *entity) {
    if (entity == nullptr) return;

    if (gm.on_vehicle_destroyed)
        gm.on_vehicle_destroyed(entity);

    auto vehicle = (mafia_vehicle*)entity->user_data;

    for(size_t i = 0; i < 4; i++)
    {
        auto pid = vehicle->seats[i];

        if (pid == -1) continue;

        auto player_ent = librg_entity_fetch(&network_context, pid);

        if (player_ent && player_ent->user_data) {
            auto player = (mafia_player*)player_ent->user_data;

            player->vehicle_id = -1;

            mod_message_send(&network_context, NETWORK_PLAYER_FROM_CAR, [&](librg_data *data) {
                librg_data_went(data, player_ent->id);
                librg_data_went(data, entity->id);
                librg_data_wu32(data, i);
            });
        }
    }

    librg_entity_destroy(&network_context, entity->id);
    delete vehicle;
    
    entity->user_data = nullptr;
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

void set_map_vis(librg_entity *entity, b32 state) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->is_visible_on_map = state;

    librg_send(&network_context, NETWORK_VEHICLE_MAP_VISIBILITY, data, {
        librg_data_went(&data, entity->id);
        librg_data_wu8(&data, (u8)state);
    });
}

void set_transparency(librg_entity *entity, f32 transparency) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->transparency = transparency;

    librg_send(&network_context, NETWORK_VEHICLE_SET_TRANSPARENCY, data, {
        librg_data_went(&data, entity->id);
        librg_data_wf32(&data, transparency);
    });
}

void set_collision_state(librg_entity *entity, b32 state) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->collision_state = state;

    librg_send(&network_context, NETWORK_VEHICLE_SET_COLLISION_STATE, data, {
        librg_data_went(&data, entity->id);
        librg_data_wu8(&data, (u8)state);
    });
}

void repair(librg_entity *entity) {
    auto vehicle = (mafia_vehicle*)entity->user_data;

    librg_send(&network_context, NETWORK_VEHICLE_REPAIR, data, {
        librg_data_went(&data, entity->id);
    });
}

void set_fuel(librg_entity *entity, f32 fuel) {
    auto vehicle = (mafia_vehicle*)entity->user_data;
    vehicle->fuel = fuel;
}
