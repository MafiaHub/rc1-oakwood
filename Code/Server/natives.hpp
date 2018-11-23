#pragma once

extern "C" {
    
    //
    // General
    //

    OAKGEN_NATIVE();
    void oak_print_test() {
        printf("It works!");
    }

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
    void oak_player_spawn(librg_entity *entity) {
        player_send_spawn(entity);
    }

    OAKGEN_NATIVE();
    void oak_player_respawn(librg_entity *entity) {
        player_send_respawn(entity);
    }

    OAKGEN_NATIVE();
    void oak_player_set_position(librg_entity *entity, zpl_vec3 position) {
        entity->position = position;

        librg_send(&network_context, NETWORK_PLAYER_SET_POS, data, {
            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, &position, sizeof(position));
        });
    }

	OAKGEN_NATIVE();
	void oak_player_set_health(librg_entity *entity, float health) {
		if (!entity->user_data || entity->type != TYPE_PLAYER) return;

		auto player = (mafia_player*)entity->user_data;
		player->health = health;

		librg_send(&network_context, NETWORK_PLAYER_SET_HEALTH, data, {
			librg_data_went(&data, entity->id);
			librg_data_wf32(&data, health);
		});
	}

    OAKGEN_NATIVE();
    void oak_player_set_rotation(librg_entity *entity, zpl_vec3 rotation) {
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
        librg_send_to(&network_context, NETWORK_PLAYER_SET_CAMERA, entity->client_peer, data, {
            librg_data_wptr(&data, &pos, sizeof(pos));
            librg_data_wptr(&data, &rot, sizeof(rot));
        });
    }

    OAKGEN_NATIVE();
    void oak_player_unlock_camera(librg_entity *entity) {
        librg_send_to(&network_context, NETWORK_PLAYER_UNLOCK_CAMERA, entity->client_peer, data, {});
    }

    OAKGEN_NATIVE();
    void oak_player_play_animation(librg_entity *entity, const char* text) {
        librg_send(&network_context, NETWORK_PLAYER_PLAY_ANIMATION, data, {
            
            char animation[32];
            strcpy(animation, text);

            librg_data_went(&data, entity->id);
            librg_data_wptr(&data, animation, sizeof(char) * 32);
        });
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
    librg_entity* oak_vehicle_spawn(zpl_vec3 position, zpl_vec3 rotation, char* model) {
        return spawn_vehicle(position, rotation, model);
    }
}

auto set_up_natives() -> void {
    auto vt = &gm.vtable;
    #include "natives.generated.hpp"
}