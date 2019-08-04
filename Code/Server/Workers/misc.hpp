#pragma once

constexpr float VEHICLE_SELECTION_TIME = 2.0f;


namespace misc {

    void vehicles_streamer_update() {
        zpl_local_persist f64 last_streamers_selection = 0.0f;

        if (zpl_time_now() - last_streamers_selection > VEHICLE_SELECTION_TIME) {
            last_streamers_selection = zpl_time_now();

            librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | TYPE_VEHICLE), [](librg_ctx *ctx, librg_entity *entity) {
                if (entity->type & TYPE_VEHICLE) {
                    auto vehicle = oak_entity_vehicle_get((oak_vehicle)entity->user_data);

                    if (vehicle && vehicle->seats[0] == -1) {
                        mod_vehicle_assign_nearest_player(&network_context, entity);
                    }
                }
            });
        }
    }


}
