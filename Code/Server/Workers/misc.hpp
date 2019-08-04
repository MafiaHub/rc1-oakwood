#pragma once

constexpr float VEHICLE_SELECTION_TIME = 2.0f;
constexpr float SCOREBOARD_UPDATE_TIME = 2.0f;
constexpr float GAMEMAP_UPDATE_TIME = 0.5f;

namespace misc {
    zpl_global f64 last_console_update  = 0.0f;
    zpl_global f64 last_fps_update      = 0.0f;
    zpl_global u32 fps_counter          = 0;
    zpl_global u32 computed_fps         = 0;

    void console_update_stats() {
        #ifndef _WIN32
            return;
        #endif

        f64 current_time = zpl_time_now();
        f64 diff = current_time - last_fps_update;
    
        fps_counter++;
        if (diff >= 1.0) {
            computed_fps = fps_counter;
            fps_counter = 0;
            last_fps_update = current_time;
        }

        //NOTE(DavoSK): Update our debug tag every 200ms
        if (current_time - last_console_update > 0.2f) {
            oak_console_draw("%c[%c%c%c] Oakwood Server | NET: %dKB / %dKB | TPS: %d (%.02f ms) | Players: %d / %d",
                132,
                130,
                oak_console_update_loader(),
                132,
                network_context.network.host->totalReceivedData / 1024,
                network_context.network.host->totalSentData / 1024,
                computed_fps,
                1000.0f / computed_fps,
                (u32)GlobalConfig.players,
                (u32)GlobalConfig.max_players);

            last_console_update = current_time;
        }
    }

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

    void gamemap_update() {
        zpl_local_persist f64 last_gamemap_update = 0.0f;

        if (zpl_time_now() - last_gamemap_update > GAMEMAP_UPDATE_TIME) {
            last_gamemap_update = zpl_time_now();

            std::vector<gamemap_info> gamemap;
            for (int i = 0; i < network_context.max_entities; i++) {
                auto entity = librg_entity_fetch(&network_context, i);
                if (entity) {

                    bool visible = false;
                    if (entity->type == TYPE_PLAYER) {
                        visible = oak_player_visibility_get((oak_player)entity->user_data, OAK_VISIBILITY_ICON);
                    }
                    else if (entity->type == TYPE_VEHICLE) {
                        visible = oak_vehicle_visibility_get((oak_vehicle)entity->user_data, OAK_VISIBILITY_ICON);
                    }
               
                    if (visible) {
                        gamemap.push_back({
                            entity->id,
                            (u8)entity->type,
                            entity->position
                        });
                    }
                }
            }

            if (gamemap.size() < 1) {
                return;
            }

            librg_send_all(&network_context, NETWORK_PLAYER_UPDATE_GAMEMAP, data, {
                librg_data_wu32(&data, gamemap.size());
                librg_data_wptr(&data, gamemap.data(), gamemap.size() * sizeof(gamemap_info));
            });
        }
    }
}
