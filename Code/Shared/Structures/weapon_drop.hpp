#pragma once

struct mafia_weapon_drop {
	char model[32];
	inventory_item weapon;
#ifdef MAFIA_SDK_IMPLEMENTATION
	MafiaSDK::C_Drop_In_Weapon* weapon_drop_actor;
#endif
};