#pragma once

#define NATIVE_CHECK_ENTITY_TYPE(entity, ent_type) if (!entity || !entity->user_data || entity->type != ent_type)
#define NATIVE_CHECK_ENTITY(entity) if (!entity || !entity->user_data)

extern "C" {
    
    //
    // General
    //

    OAKGEN_NATIVE();
    void oak_broadcast_msg_color(const char* text, u32 color) {
        librg_send(&network_context, NETWORK_SEND_CONSOLE_MSG, data, {
            librg_data_wu32(&data, strlen(text));
            librg_data_wu32(&data, color);
            librg_data_wptr(&data, (void*)text, strlen(text));
        });
    }

    OAKGEN_NATIVE();
    void oak_broadcast_msg(const char* text) {
        oak_broadcast_msg_color(text, 0xFFFFFF);
    }

    OAKGEN_NATIVE();
    void oak_chat_print(const char* text) {
        librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
            auto len = strlen(text);
            librg_data_wu16(&data, len);
            librg_data_wptr(&data, (void *)text, len);
        });
    }

    OAKGEN_NATIVE();
    void oak_send_msg(const char* text, librg_entity *receiver) {
        if (!receiver->client_peer)
            return;

        librg_send_to(&network_context, NETWORK_SEND_CHAT_MSG, receiver->client_peer, data, {
            auto len = strlen(text);
            librg_data_wu16(&data, len);
            librg_data_wptr(&data, (void *)text, len);
        });
    }


    //
    // Player
    //

    OAKGEN_NATIVE();
    void oak_player_fadeout(librg_entity *entity, bool fadeout, u32 duration, u32 color) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        librg_send_to(&network_context, NETWORK_SEND_FADEOUT, entity->client_peer, data, {
            librg_data_wu8(&data, fadeout);
            librg_data_wu32(&data, duration);
            librg_data_wu32(&data, color);
        });
    }

    OAKGEN_NATIVE();
    void oak_player_inventory_add(librg_entity *entity, inventory_item *item) {
        player_inventory_add(entity, item);
    }

    OAKGEN_NATIVE();
    void oak_player_spawn(librg_entity *entity, zpl_vec3 pos) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER){};

        entity->position = EXPAND_VEC(pos);
        player_send_spawn(entity);
    }

    OAKGEN_NATIVE();
    void oak_player_set_model(librg_entity *entity, char *modelName) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        auto player = (mafia_player*)entity->user_data;

        if (player && modelName) {
            strncpy(player->model, modelName, 32);

            librg_send(&network_context, NETWORK_PLAYER_SET_MODEL, data, {
                librg_data_went(&data, entity->id);
                librg_data_wptr(&data, (void*)player->model, sizeof(char) * 32);
            });
        }
    }

    OAKGEN_NATIVE();
    void oak_player_set_position(librg_entity *entity, zpl_vec3 position) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};
        librg_send(&network_context, NETWORK_PLAYER_SET_POS, data, {
            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, &position, sizeof(position));
        });
    }

    OAKGEN_NATIVE();
    void oak_player_set_health(librg_entity *entity, float health) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        auto player = (mafia_player*)entity->user_data;
        player->health = health;

        librg_send(&network_context, NETWORK_PLAYER_SET_HEALTH, data, {
            librg_data_went(&data, entity->id);
            librg_data_wf32(&data, health);
        });
    }

    OAKGEN_NATIVE();
    void oak_player_set_rotation(librg_entity *entity, zpl_vec3 rotation) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        auto player = (mafia_player*)(entity->user_data);
        if(player) {
            player->rotation = rotation;
        }

        librg_send(&network_context, NETWORK_PLAYER_SET_ROT, data, {
            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, &rotation, sizeof(rotation));
        });
    }

    OAKGEN_NATIVE();
    void oak_player_set_camera(librg_entity *entity, zpl_vec3 pos, zpl_vec3 rot) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        librg_send_to(&network_context, NETWORK_PLAYER_SET_CAMERA, entity->client_peer, data, {
            librg_data_wptr(&data, &pos, sizeof(pos));
            librg_data_wptr(&data, &rot, sizeof(rot));
        });
    }

    OAKGEN_NATIVE();
    void oak_player_unlock_camera(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        librg_send_to(&network_context, NETWORK_PLAYER_UNLOCK_CAMERA, entity->client_peer, data, {});
    }

    OAKGEN_NATIVE();
    void oak_player_play_animation(librg_entity *entity, const char* text) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        librg_send(&network_context, NETWORK_PLAYER_PLAY_ANIMATION, data, {
            
            char animation[32];
            strcpy(animation, text);

            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, animation, sizeof(char) * 32);
        });
    }

    OAKGEN_NATIVE();
    librg_entity* oak_player_get_vehicle(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return nullptr; };

        auto player = (mafia_player*)entity->user_data;

        if (player->vehicle_id != -1) {
            auto vehicle = librg_entity_fetch(&network_context, player->vehicle_id);
            return vehicle;
        }

        return nullptr;
    }

    OAKGEN_NATIVE();
    b32 oak_player_put_to_vehicle(librg_entity *entity, librg_entity *vehicle_ent, int seat_id) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return false; };
        NATIVE_CHECK_ENTITY_TYPE(vehicle_ent, TYPE_VEHICLE) { return false; };

        auto player = (mafia_player*)entity->user_data;
        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;


        if (vehicle->seats[seat_id] != -1)
            return false;

        if (player->vehicle_id != -1)
            return false;

        if (seat_id < 0 || seat_id > 3)
            return false;

        vehicle->seats[seat_id] = entity->id;
        player->vehicle_id = vehicle_ent->id;

        if (seat_id == 0 && vehicle->seats[0] == -1) {
            librg_entity_control_set(&network_context, vehicle_ent->id, entity->client_peer);
        }
        
        librg_send(&network_context, NETWORK_PLAYER_PUT_TO_VEHICLE, data, {
            librg_data_went(&data, entity->id);
            librg_data_went(&data, vehicle_ent->id);
            librg_data_wi32(&data, seat_id);
        });

        return true;
    }

    OAKGEN_NATIVE();
    b32 oak_player_die(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return false; };

        librg_send(&network_context, NETWORK_PLAYER_DIE, data, {
            librg_data_went(&data, entity->id);
        });

        return true;
    }

    //
    // Weapon drop
    //

    OAKGEN_NATIVE();
    librg_entity* oak_drop_spawn(zpl_vec3 position, char *model, inventory_item item) {
        return spawn_weapon_drop(position, model, item);
    }

    //
    // Vehicle 
    // 

    OAKGEN_NATIVE();
    librg_entity* oak_vehicle_spawn(zpl_vec3 position, zpl_vec3 rotation, char* model, b32 show_in_radar) {
        return spawn_vehicle(position, rotation, model, show_in_radar);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_show_on_radar(librg_entity *entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};

        auto vehicle = (mafia_vehicle*)entity->user_data;
        vehicle->is_car_in_radar = state;

        librg_send(&network_context, NETWORK_VEHICLE_RADAR_VISIBILITY, data, {
            librg_data_went(&data, entity->id);
            librg_data_wu8(&data, (u8)state);
        });
    }

    OAKGEN_NATIVE();
    int oak_vehicle_get_player_seat_id(librg_entity *entity, librg_entity *player) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) { return -1; };
        NATIVE_CHECK_ENTITY_TYPE(player, TYPE_PLAYER) { return -1; };

        auto vehicle = (mafia_vehicle*)entity->user_data;

        for (size_t i = 0; i < 4; i++)
        {
            if (vehicle->seats[i] == player->id)
                return i;
        }

        return -1;
    }
    
    OAKGEN_NATIVE();
    void oak_vehicle_set_position(librg_entity *entity, zpl_vec3 position) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        entity->position = position;

        librg_send(&network_context, NETWORK_VEHICLE_SET_POS, data, {
            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, &position, sizeof(position));
        });
    }

    OAKGEN_NATIVE();
    void oak_vehicle_set_direction(librg_entity *entity, zpl_vec3 dir) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        
        auto vehicle = (mafia_vehicle*)entity->user_data;
        vehicle->rot_forward = dir;

        librg_send(&network_context, NETWORK_VEHICLE_SET_DIR, data, {
            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, &dir, sizeof(dir));
        });
    }
}

auto set_up_natives() -> void {
    auto vt = &gm.vtable;
    #include "natives.generated.hpp"
}
