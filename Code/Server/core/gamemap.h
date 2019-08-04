#define OAK_GAMEMAP_UPDATE_TIME 0.5f

int oak_gamemap_update() {
    zpl_local_persist f64 last_gamemap_update = 0.0f;

    if (zpl_time_now() - last_gamemap_update > OAK_GAMEMAP_UPDATE_TIME) {
        last_gamemap_update = zpl_time_now();


        zpl_array_make(gamemap_info, gamemap, zpl_heap());

        int player_count;
        mafia_player **players = cast(mafia_player **)oak_entity_list(OAK_PLAYER, &player_count);

        for (int i = 0; i < player_count; ++i)
        {
            mafia_player *player = players[i];
            if (oak_player_visibility_get(player->oak_id, OAK_VISIBILITY_ICON)) {
                gamemap_info info = {
                    player->oak_id,
                    (u8)TYPE_PLAYER, // todo: OAK_PLAYER
                    player->position
                };

                zpl_array_append(gamemap, info);
            }
        }

        int vehicle_count;
        mafia_vehicle **vehicles = cast(mafia_vehicle **)oak_entity_list(OAK_VEHICLE, &vehicle_count);

        for (int i = 0; i < vehicle_count; ++i)
        {
            mafia_vehicle *vehicle = vehicles[i];
            if (oak_vehicle_visibility_get(vehicle->oak_id, OAK_VISIBILITY_ICON)) {
                gamemap_info info = {
                    vehicle->oak_id,
                    (u8)TYPE_VEHICLE, // todo: OAK_VEHICLE
                    vehicle->librg_entity->position
                };

                zpl_array_append(gamemap, info);
            }
        }


        if (zpl_array_count(gamemap) < 1) {
            zpl_array_free(gamemap);
            return -1;
        }

        librg_send_all(&network_context, NETWORK_PLAYER_UPDATE_GAMEMAP, data, {
            librg_data_wu32(&data, zpl_array_count(gamemap));
            librg_data_wptr(&data, gamemap, zpl_array_count(gamemap) * sizeof(gamemap_info));
        });

        zpl_array_free(gamemap);
    }

    return 0;
}
