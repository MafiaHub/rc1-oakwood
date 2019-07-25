#pragma once

struct body_health {
	f32 left_hand;
	f32 right_hand;
	f32 left_leg;
	f32 right_leg;
};

enum {
	CLIENTSIDE_PLAYER_WAITING_FOR_VEH = (1 << 10)
};

struct mafia_player {
    ~mafia_player() {
#ifdef MAFIA_SDK_IMPLEMENTATION
        if (nickname_texture != nullptr) {
            nickname_texture->Release();
            nickname_texture = nullptr;
        }
#endif
    }

	mafia_player() : 
	streamer_entity_id(-1),
	vehicle_id(-1),
    hwid(0),
    has_visible_nameplate(1),
	current_weapon_id(0) {
		zpl_zero_item(this);
		streamer_entity_id = -1;
		vehicle_id = -1;
		pose = { 1.0f, 1.0f, 1.0f };
        position = { 0.0f };
		rotation = { 0.0f, 0.0f, -1.0f };
		strcpy(model, "Tommy.i3d");

		for (size_t i = 0; i < 8; i++)
			inventory.items[i] = { -1, 0, 0, 0 };

#ifdef MAFIA_SDK_IMPLEMENTATION
		ped = nullptr;
		nickname_texture		= nullptr;
		last_talked				= 0;
		clientside_flags		= 0;
#endif
	}
	i32 streamer_entity_id;
    zpl_vec3 position;
	zpl_vec3 rotation;
	zpl_vec3 pose;
	f32 health;
    f32 aim;
	u8 animation_state;
	u8 is_aiming;
	u8 is_crouching;
	u8 is_visible_on_map;
    u8 has_visible_nameplate;
	char model[32];
	char name[32];
	u32 aiming_time;
	player_inventory inventory;
	u32 current_weapon_id;
	i32 vehicle_id;
    u32 ping;
    u64 hwid;
#ifdef MAFIA_SDK_IMPLEMENTATION
	struct {
		/*struct {
            zpl_vec3 start;
            zpl_vec3 target;
            zpl_vec3 error;
            f32 last_alpha;
            f64 start_time;
            f64 finish_time;
		} pos, rot, pose;*/
        lib_inter* pos, *rot, *pose;

        struct {
            float start;
            float target;
            f64 start_time;
            f32 alpha;
        } car_shooting; 
	} interp;
	
	u64 clientside_flags;
	f32 last_talked;
	MafiaSDK::C_Human* ped;
	IDirect3DTexture9* nickname_texture;
#endif
};
