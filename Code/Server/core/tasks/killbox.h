#define OAK_KILLBOX_CHECK_TIME 2.0f

int oak_killbox_update() {
    zpl_local_persist f64 last_check_time = 0.0f;

    if (zpl_time_now() - last_check_time > OAK_KILLBOX_CHECK_TIME) {
        last_check_time = zpl_time_now();

        /* Handle players first */

        int player_count;
        auto players = oak_player_list(&player_count);

        for (int i = 0; i < player_count; i++) {
            mafia_player *entity = oak_entity_player_get(players[i]);

            if (entity->position.y <= oak_config_killbox_get()) {
                oak_player_kill(players[i]);
            }
        }

        /* Handle vehicles after */

        int vehicle_count;
        auto vehicles = oak_vehicle_list(&vehicle_count);

        for (int i = 0; i < vehicle_count; i++) {
            mafia_vehicle *entity = oak_entity_vehicle_get(players[i]);

            if (entity->native_entity->position.y <= oak_config_killbox_get()) {
                oak_vehicle_despawn(vehicles[i]);
            }
        }
    }

    return 0;
}
