#pragma once

inline auto player_clientstreamer_update(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    player->health = librg_data_rf32(evnt->data);
    player->animation_state = librg_data_ru8(evnt->data);
    player->is_crouching = librg_data_ru8(evnt->data);
    player->is_aiming = librg_data_ru8(evnt->data);
    player->aiming_time = librg_data_ru64(evnt->data);
}

inline auto player_entityupdate(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wf32(evnt->data, player->health);
    librg_data_wu8(evnt->data, player->animation_state);
    librg_data_wu8(evnt->data, player->is_crouching);
    librg_data_wu8(evnt->data, player->is_aiming);
    librg_data_wu64(evnt->data, player->aiming_time);
}

inline auto player_entitycreate(librg_event* evnt) -> void {
    auto player = (mafia_player *)evnt->entity->user_data;
    librg_data_wi32(evnt->data, player->vehicle_id);
    librg_data_wi32(evnt->data, player->streamer_entity_id);
    librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3));
    librg_data_wptr(evnt->data, player->model, sizeof(char) * 32);
    librg_data_wptr(evnt->data, player->name, sizeof(char) * 32);
    librg_data_wu8(evnt->data, player->is_crouching);
    librg_data_wu8(evnt->data, player->is_aiming);
    librg_data_wptr(evnt->data, &player->inventory, sizeof(player_inventory));
    librg_data_wu32(evnt->data, player->current_weapon_id);
    librg_data_wf32(evnt->data, player->health);
}

inline auto player_connection_disconnect(librg_event* evnt) -> void {
    
    if (evnt->entity && evnt->entity->user_data) {
        auto player = (mafia_player*)evnt->entity->user_data;
        if (player->vehicle_id != -1) {
            auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
            if (vehicle_ent && vehicle_ent->user_data) {
                auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                for (int i = 0; i < 4; i++) {
                    if (vehicle->seats[i] == evnt->entity->id) {
                        vehicle->seats[i] = -1;

                        if (i == 0) {
                            auto streamer = mod_get_nearest_player(&network_context, vehicle_ent->position, evnt->entity->id);
                            if (streamer != nullptr)
                                librg_entity_control_set(&network_context, vehicle_ent->id, streamer->client_peer);
                            else 
                                librg_entity_control_remove(&network_context, vehicle_ent->id);
                        }
                    }
                }
            }
        }
    }
}

inline auto player_send_spawn(librg_entity* player_ent) -> void {
    auto player = (mafia_player*)player_ent->user_data;
    librg_send_to(&network_context, NETWORK_PLAYER_SPAWN, player_ent->client_peer, data, {
        librg_data_wptr(&data, &player_ent->position, sizeof(zpl_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * 32);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        librg_data_wu32(&data, player->current_weapon_id);
        librg_data_wf32(&data, player->health);
    });
}

inline auto player_send_message(librg_entity* player_ent, char* msg, u32 color = 0xFFFFFF) {
    librg_send_to(&network_context, NETWORK_PLAYER_SPAWN, player_ent->client_peer, data, {
        librg_data_wu32(&data, strlen(msg));
        librg_data_wu32(&data, color);
        librg_data_wptr(&data, msg, strlen(msg));
    });
}

inline auto player_send_respawn(librg_entity* player_ent) -> void {
    auto player = (mafia_player*)player_ent->user_data;

    librg_send(&network_context, NETWORK_PLAYER_RESPAWN, data, {
        librg_data_went(&data, player_ent->id);
        librg_data_wptr(&data, &player_ent->position, sizeof(zpl_vec3));
        librg_data_wptr(&data, &player->rotation, sizeof(zpl_vec3));
        librg_data_wptr(&data, player->model, sizeof(char) * 32);
        librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
        librg_data_wu32(&data, player->current_weapon_id);
        librg_data_wf32(&data, player->health);
    });
}
