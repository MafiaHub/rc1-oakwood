#define OAK_VEHICLE_SELECTION_TIME 2.5f

void oak_vehicles_update() {
    zpl_local_persist f64 last_streamers_selection = 0.0f;

    if (zpl_time_now() - last_streamers_selection > OAK_VEHICLE_SELECTION_TIME) {
        last_streamers_selection = zpl_time_now();

        librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | OAK_VEHICLE), [](librg_ctx *ctx, librg_entity *entity) {
            auto vehicle = oak_entity_vehicle_get_from_native(entity);

            if (vehicle && vehicle->seats[0] == -1) {
                oak_vehicle_streamer_assign_nearest(vehicle->oak_id);
            }
        });
    }
}
