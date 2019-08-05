#define OAK_GAMEMAP_UPDATE_TIME 0.5f

int oak_gamemap_update() {
    zpl_local_persist f64 last_gamemap_update = 0.0f;

    if (zpl_time_now() - last_gamemap_update > OAK_GAMEMAP_UPDATE_TIME) {
        last_gamemap_update = zpl_time_now();

        zpl_array_make(gamemap_info, gamemap, zpl_heap());

        int player_count;
        oak_player *players = oak_player_list(&player_count);

        for (int i = 0; i < player_count; ++i) {
            if (oak_player_visibility_get(players[i], OAK_VISIBILITY_ICON)) {
                mafia_player *player = oak_entity_player_get(players[i]);

                gamemap_info info = {
                    player->native_id,
                    (u8)OAK_PLAYER, // todo: OAK_PLAYER
                    player->position
                };

                zpl_array_append(gamemap, info);
            }
        }

        int vehicle_count;
        oak_vehicle *vehicles = oak_vehicle_list(&vehicle_count);

        for (int i = 0; i < vehicle_count; ++i) {
            mafia_vehicle *vehicle = oak_entity_vehicle_get(vehicles[i]);

            if (oak_vehicle_visibility_get(vehicle->oak_id, OAK_VISIBILITY_ICON)) {
                gamemap_info info = {
                    vehicle->native_id,
                    (u8)OAK_VEHICLE, // todo: OAK_VEHICLE
                    vehicle->native_entity->position
                };

                zpl_array_append(gamemap, info);
            }
        }


        if (zpl_array_count(gamemap) < 1) {
            zpl_array_free(gamemap);
            return -1;
        }

        librg_send_all(&network_context, NETWORK_TASK_UPDATE_GAMEMAP, data, {
            librg_data_wu32(&data, zpl_array_count(gamemap));
            librg_data_wptr(&data, gamemap, zpl_array_count(gamemap) * sizeof(gamemap_info));
        });

        zpl_array_free(gamemap);
    }

    return 0;
}
