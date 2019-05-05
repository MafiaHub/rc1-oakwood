#pragma once

constexpr float VEHICLE_SELECTION_TIME = 2.0f;
constexpr float SCOREBOARD_UPDATE_TIME = 2.0f;

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
            console::draw("%c[%c%c%c] Oakwood Server | NET: %dKB / %dKB | TPS: %d (%.02f ms) | Players: %d / %d",
                132,
                130,
                console::update_loader(),
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
                if (entity->user_data && entity->type & TYPE_VEHICLE) {
                    auto vehicle = (mafia_vehicle*)entity->user_data;

                    if (vehicle->seats[0] == -1) {
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
            for (int i = 0; i < network_context.max_entities; i++) {
                auto entity = librg_entity_fetch(&network_context, i);
                if (entity && entity->user_data && entity->type == TYPE_PLAYER) {
                    auto player = (mafia_player*)entity->user_data;
                    player_scoreboard_info player_info;
                    strcpy_s(player_info.nickname, player->name);
                    player_info.ping = entity->client_peer->roundTripTime;
                    player_info.server_id = entity->id;
                    scoreboard.push_back(player_info);
                }
            }

            for (int i = 0; i < network_context.max_entities; i++) {
                auto entity = librg_entity_fetch(&network_context, i);
                if (entity && entity->user_data && entity->type == TYPE_PLAYER) {
                    librg_send_to(&network_context, NETWORK_PLAYER_UPDATE_SCOREBOARD, entity->client_peer, data, {
                        librg_data_wu32(&data, scoreboard.size());
                        librg_data_wptr(&data, scoreboard.data(), scoreboard.size() * sizeof(player_scoreboard_info));
                    });
                }
            }
        }
    }
}
