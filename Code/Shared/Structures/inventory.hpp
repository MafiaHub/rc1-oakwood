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
