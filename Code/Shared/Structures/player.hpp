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

struct mafia_player {
	mafia_player() : 
	streamer_entity_id(-1),
	current_weapon_id(0) {
		for (size_t i = 0; i < 8; i++)
			inventory.items[i] = { -1, 0, 0, 0 };

#ifdef MAFIA_SDK_IMPLEMENTATION
		nickname_texture = nullptr;
		voice_channel = nullptr;
		last_talked = 0;
#endif
	}
	i32 streamer_entity_id;
	zpl_vec3_t rotation;
	zpl_vec3_t pose;
	f32 health;
	u8 animation_state;
	u8 is_aiming;
	u8 is_crouching;
	char model[32];
	char name[32];
	DWORD aiming_time;
	player_inventory inventory;
	u32 current_weapon_id;
#ifdef MAFIA_SDK_IMPLEMENTATION
	f32 inter_delta;
	f32 last_talked;
    interpolate3_hermite_t inter_pos;
	interpolate3_hermite_t inter_rot;
	interpolate3_hermite_t inter_pose;
	MafiaSDK::C_Human* ped;
	IDirect3DTexture9* nickname_texture;
	voip_channel_t* voice_channel;
#endif
};
