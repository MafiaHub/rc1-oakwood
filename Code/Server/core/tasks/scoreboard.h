#define OAK_SCOREBOARD_UPDATE_TIME 2.0f

int oak_scoreboard_update() {
    zpl_local_persist f64 last_scoreboard_update = 0.0f;

    if (zpl_time_now() - last_scoreboard_update > OAK_SCOREBOARD_UPDATE_TIME) {
        last_scoreboard_update = zpl_time_now();

        zpl_array_make(player_scoreboard_info, scoreboard, zpl_heap());

        int player_count;
        oak_player *players = oak_player_list(&player_count);

        for (int i = 0; i < player_count; i++) {
            mafia_player *entity = oak_entity_player_get(players[i]);

            player_scoreboard_info player_info;
            strcpy(player_info.nickname, entity->name);
            player_info.ping = entity->native_entity->client_peer->roundTripTime;
            player_info.server_id = entity->oak_id;
            zpl_array_append(scoreboard, player_info);
        }

        /* prevent writing data of 0 * sizeof() */
        if (zpl_array_count(scoreboard) < 1) {
            zpl_array_free(scoreboard);
            return -1;
        }

        librg_send_all(&network_context, NETWORK_TASK_UPDATE_SCOREBOARD, data, {
            librg_data_wu32(&data, zpl_array_count(scoreboard));
            librg_data_wptr(&data, scoreboard, zpl_array_count(scoreboard) * sizeof(player_scoreboard_info));
        });

        zpl_array_free(scoreboard);
    }

    return 0;
}
