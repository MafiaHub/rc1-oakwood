
inline auto clientstreamer_update(librg_event* evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    librg_data_rptr(evnt->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    vehicle->engine_rpm			= librg_data_rf32(evnt->data);
    vehicle->engine_health 		= librg_data_rf32(evnt->data);
    vehicle->wheel_angle 		= librg_data_rf32(evnt->data);
    vehicle->fuel 				= librg_data_rf32(evnt->data);
    vehicle->accelerating		= librg_data_rf32(evnt->data);
    vehicle->hand_break 		= librg_data_rf32(evnt->data);
    vehicle->break_val 			= librg_data_rf32(evnt->data);
    vehicle->clutch 			= librg_data_rf32(evnt->data);
    vehicle->gear 				= librg_data_ri32(evnt->data);
    vehicle->horn 				= librg_data_ru8(evnt->data);
    vehicle->siren 				= librg_data_ru8(evnt->data);
    vehicle->engine_on			= librg_data_ru8(evnt->data);
}

inline auto entityupdate(librg_event* evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    librg_data_wptr(evnt->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_wf32(evnt->data, vehicle->engine_rpm);
    librg_data_wf32(evnt->data, vehicle->engine_health);
    librg_data_wf32(evnt->data, vehicle->wheel_angle);
    librg_data_wf32(evnt->data, vehicle->fuel);
    librg_data_wf32(evnt->data, vehicle->accelerating);
    librg_data_wf32(evnt->data, vehicle->hand_break);
    librg_data_wf32(evnt->data, vehicle->break_val);
    librg_data_wf32(evnt->data, vehicle->clutch);
    librg_data_wi32(evnt->data, vehicle->gear);
    librg_data_wu8(evnt->data, vehicle->horn);
    librg_data_wu8(evnt->data, vehicle->siren);
    librg_data_wu8(evnt->data, vehicle->engine_on);
}

inline auto entitycreate(librg_event* evnt) {
    auto vehicle = (mafia_vehicle *)evnt->entity->user_data;
    librg_data_wptr(evnt->data, &vehicle->rot_forward, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->rot_up, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->rot_speed, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &vehicle->speed, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &evnt->entity->position, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, vehicle->model, sizeof(char) * 32);
    librg_data_wptr(evnt->data, vehicle->seats, sizeof(i32) * 4);
    librg_data_wptr(evnt->data, vehicle->tyres, sizeof(mafia_vehicle_tyre) * 4);
    librg_data_wptr(evnt->data, vehicle->destroyed_components, sizeof(u8) * 15);

    auto deltas_count = vehicle->deform_deltas.size();
    librg_data_wu32(evnt->data, deltas_count);

    if(deltas_count)
        librg_data_wptr(evnt->data, vehicle->deform_deltas.data(), sizeof(mafia_vehicle_deform) *  vehicle->deform_deltas.size());

    librg_data_wf32(evnt->data, vehicle->engine_rpm);
    librg_data_wf32(evnt->data, vehicle->engine_health);
    librg_data_wf32(evnt->data, vehicle->health);
    librg_data_wu8(evnt->data, vehicle->horn);
    librg_data_wu8(evnt->data, vehicle->siren);
    librg_data_wu8(evnt->data, vehicle->sound_enabled);
    librg_data_wu8(evnt->data, vehicle->is_car_in_radar);
    librg_data_wf32(evnt->data, vehicle->hand_break);
    librg_data_wf32(evnt->data, vehicle->speed_limit);
    librg_data_wi32(evnt->data, vehicle->gear);
    librg_data_wf32(evnt->data, vehicle->break_val);
    librg_data_wf32(evnt->data, vehicle->clutch);
    librg_data_wf32(evnt->data, vehicle->wheel_angle);
    librg_data_wu8(evnt->data, vehicle->engine_on);
    librg_data_wf32(evnt->data, vehicle->fuel);
    librg_data_wf32(evnt->data, vehicle->accelerating);
    librg_data_wu8(evnt->data, vehicle->is_visible_on_map);
}
