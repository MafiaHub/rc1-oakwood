#pragma once

struct body_health {
	f32 left_hand;
	f32 right_hand;
	f32 left_leg;
	f32 right_leg;
};

#ifdef MAFIA_SDK_IMPLEMENTATION
struct voip_channel_t {
	HSTREAM playback_stream;
	OpusDecoder *decoder;
};
#endif

enum {
	CLIENTSIDE_PLAYER_WAITING_FOR_VEH = (1 << 10)
};

struct mafia_player {
	mafia_player() : 
	streamer_entity_id(-1),
	vehicle_id(-1),
	current_weapon_id(0) {
		zpl_zero_item(this);
		streamer_entity_id = -1;
		vehicle_id = -1;
		pose = { 1.0f, 1.0f, 1.0f };
		rotation = { 0.0f, 0.0f, -1.0f };
		strcpy(model, "Tommy.i3d");

		for (size_t i = 0; i < 8; i++)
			inventory.items[i] = { -1, 0, 0, 0 };

#ifdef MAFIA_SDK_IMPLEMENTATION
		nickname_texture		= nullptr;
		voice_channel			= nullptr;
		last_talked				= 0;
		clientside_flags		= 0;
#endif
	}
	i32 streamer_entity_id;
	zpl_vec3 rotation;
	zpl_vec3 pose;
	f32 health;
	u8 animation_state;
	u8 is_aiming;
	u8 is_crouching;
	char model[32];
	char name[32];
	u32 aiming_time;
	player_inventory inventory;
	u32 current_weapon_id;
	i32 vehicle_id;
    u32 ping;
#ifdef MAFIA_SDK_IMPLEMENTATION
	struct {
		struct {
			zpl_vec3 start;
			zpl_vec3 target;
			f32  lastAlpha;
			f64  startTime;
			f64  finishTime;
		} pos, rot, pose;
	} interp;
	
	u64 clientside_flags;
	f32 last_talked;
	MafiaSDK::C_Human* ped;
	IDirect3DTexture9* nickname_texture;
	voip_channel_t* voice_channel;
#endif
};
