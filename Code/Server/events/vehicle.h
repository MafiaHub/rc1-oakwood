/* EVENTS */

void oak_ev_vehicle_create(librg_event *e) {
    auto vehicle = oak_entity_vehicle_get((oak_vehicle)e->entity->user_data);
    ZPL_ASSERT_NOT_NULL(vehicle);

    librg_data_wptr(e->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &e->entity->position, sizeof(zpl_vec3));
    librg_data_wptr(e->data, vehicle->model, sizeof(char) * 32);
    librg_data_wptr(e->data, vehicle->seats, sizeof(i32) * OAK_MAX_SEATS);
    librg_data_wptr(e->data, vehicle->tyres, sizeof(mafia_vehicle_tyre) * 4);
    librg_data_wptr(e->data, vehicle->destroyed_components, sizeof(u8) * 15);

    auto deltas_count = vehicle->deform_deltas.size();
    librg_data_wu32(e->data, deltas_count);

    if (deltas_count)
        librg_data_wptr(e->data, vehicle->deform_deltas.data(), sizeof(mafia_vehicle_deform) *  vehicle->deform_deltas.size());

    librg_data_wf32(e->data, vehicle->engine_rpm);
    librg_data_wf32(e->data, vehicle->engine_health);
    librg_data_wf32(e->data, vehicle->health);
    librg_data_wu8(e->data, vehicle->horn);
    librg_data_wu8(e->data, vehicle->siren);
    librg_data_wu8(e->data, vehicle->sound_enabled);
    librg_data_wu8(e->data, vehicle->is_car_in_radar);
    librg_data_wf32(e->data, vehicle->hand_break);
    librg_data_wf32(e->data, vehicle->speed_limit);
    librg_data_wi32(e->data, vehicle->gear);
    librg_data_wf32(e->data, vehicle->break_val);
    librg_data_wf32(e->data, vehicle->clutch);
    librg_data_wf32(e->data, vehicle->wheel_angle);
    librg_data_wu8(e->data, vehicle->engine_on);
    librg_data_wf32(e->data, vehicle->fuel);
    librg_data_wf32(e->data, vehicle->accelerating);
    librg_data_wu8(e->data, vehicle->is_visible_on_map);
    librg_data_wf32(e->data, vehicle->transparency);
    librg_data_wu8(e->data, vehicle->collision_state);
}

void oak_ev_vehicle_update(librg_event *e) {
    auto vehicle = oak_entity_vehicle_get((oak_vehicle)e->entity->user_data);
    ZPL_ASSERT_NOT_NULL(vehicle);

    librg_data_wptr(e->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_wptr(e->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_wptr(e->data, vehicle->seats, sizeof(i32) * OAK_MAX_SEATS);
    librg_data_wf32(e->data, vehicle->engine_rpm);
    librg_data_wf32(e->data, vehicle->engine_health);
    librg_data_wf32(e->data, vehicle->wheel_angle);
    librg_data_wf32(e->data, vehicle->fuel);
    librg_data_wf32(e->data, vehicle->accelerating);
    librg_data_wf32(e->data, vehicle->hand_break);
    librg_data_wf32(e->data, vehicle->break_val);
    librg_data_wf32(e->data, vehicle->clutch);
    librg_data_wi32(e->data, vehicle->gear);
    librg_data_wu8(e->data, vehicle->horn);
    librg_data_wu8(e->data, vehicle->siren);
    librg_data_wu8(e->data, vehicle->engine_on);
}

void oak_ev_vehicle_remove(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_vehicle_client_add(librg_event *e) {
    // NOTE: not implemented
}

void oak_ev_vehicle_client_update(librg_event *e) {
    auto vehicle = oak_entity_vehicle_get((oak_vehicle)e->entity->user_data);

    /* handle cases when entity is already deleted on the server */
    /* but we are still receiving updates from the clients */
    if (!vehicle) {
        librg_event_reject(e);
        return;
    }

    librg_data_rptr(e->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_rptr(e->data, &vehicle->speed, sizeof(zpl_vec3));
    vehicle->engine_rpm         = librg_data_rf32(e->data);
    vehicle->engine_health      = librg_data_rf32(e->data);
    vehicle->wheel_angle        = librg_data_rf32(e->data);
    vehicle->fuel               = librg_data_rf32(e->data);
    vehicle->accelerating       = librg_data_rf32(e->data);
    vehicle->hand_break         = librg_data_rf32(e->data);
    vehicle->break_val          = librg_data_rf32(e->data);
    vehicle->clutch             = librg_data_rf32(e->data);
    vehicle->gear               = librg_data_ri32(e->data);
    vehicle->horn               = librg_data_ru8(e->data);
    vehicle->siren              = librg_data_ru8(e->data);
    vehicle->engine_on          = librg_data_ru8(e->data);
}

void oak_ev_vehicle_client_remove(librg_event *e) {
    // NOTE: not implemented
}

/* MESSAGES */

int oak_vehicle_register() {
    librg_network_add(&network_context, NETWORK_VEHICLE_EXPLODE, [](librg_message* msg) {
        auto vehicle_id = librg_data_ru32(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent) {
            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);

            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {
                mod_message_send(&network_context, NETWORK_VEHICLE_EXPLODE, [&](librg_data *data) {
                    librg_data_wu32(data, vehicle_ent->id);
                });
  
                oak_vehicle_despawn((oak_vehicle)vehicle_ent->user_data);
            }
        }
    });

    return 0;
}
