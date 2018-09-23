#pragma once
struct inventory_item {
	int	weaponId;
	int	ammoLoaded;
	int	ammoHidden;
	int	_unk;
};

struct player_inventory {
	inventory_item items[8];
};

struct body_health {
	f32 left_hand;
	f32 right_hand;
	f32 left_leg;
	f32 right_leg;
};

struct mafia_player {
	mafia_player() : 
	current_weapon_id(0) {
		for (size_t i = 0; i < 8; i++)
			inventory.items[i] = { -1, 0, 0, 0 };
	}

	zplm_vec3_t rotation;
	zplm_vec3_t pose;
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
	Interpolator inter_rot;
	Interpolator inter_pos;
	Interpolator inter_pose;
	MafiaSDK::C_Human* ped;
#endif
};

struct mafia_weapon_drop {
	char model[32];
	inventory_item weapon;
#ifdef MAFIA_SDK_IMPLEMENTATION
	MafiaSDK::C_Drop_In_Weapon* weapon_drop_actor;
#endif
};