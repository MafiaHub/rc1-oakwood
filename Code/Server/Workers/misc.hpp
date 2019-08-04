#pragma once

constexpr float VEHICLE_SELECTION_TIME = 2.0f;
constexpr float SCOREBOARD_UPDATE_TIME = 2.0f;

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

    void scoreboard_update() {
        zpl_local_persist f64 last_scoreboard_update = 0.0f;

        if (zpl_time_now() - last_scoreboard_update > SCOREBOARD_UPDATE_TIME) {
            last_scoreboard_update = zpl_time_now();

            std::vector<player_scoreboard_info> scoreboard;
            for (int i = 0; i < connected_players.size(); i++) {
                auto entity = connected_players.at(i);
                if (entity && entity->type == TYPE_PLAYER) {
                    auto player = oak_entity_player_get((oak_player)entity->user_data);
                    if (player) {
                        player_scoreboard_info player_info;
                        strcpy(player_info.nickname, player->name);
                        player_info.ping = entity->client_peer->roundTripTime;
                        player_info.server_id = scoreboard.size();
                        scoreboard.push_back(player_info);
                    }
                }
            }

            /* prevent writing data of 0 * sizeof() */
            if (scoreboard.size() < 1) {
                return;
            }

            librg_send_all(&network_context, NETWORK_PLAYER_UPDATE_SCOREBOARD, data, {
                librg_data_wu32(&data, scoreboard.size());
                librg_data_wptr(&data, scoreboard.data(), scoreboard.size() * sizeof(player_scoreboard_info));
            });
        }
    }
}
