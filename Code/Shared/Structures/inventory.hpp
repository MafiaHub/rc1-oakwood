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

struct weapon {
    const char* model;
    inventory_item item;
};

weapon weaponlist[] = {
    {NULL, NULL}, // DO NOT USE
    {NULL, NULL}, // DO NOT USE
    {"2boxer.i3d", {2, 0, 0, 0}}, // Boxer (Knuckle Duster)
    {"2knife.i3d", {3, 0, 0, 0}}, // Knife
    {"2bbat1.i3d", {4, 0, 0, 0}}, // Baseball Bat
    {"2mol.i3d", {5, 0, 0, 0}}, // Molotov
    {"2coltDS.i3d", {6, 6, 30, 0}}, // Colt Detective Special
    {"2sw27.i3d", {7, 6, 30, 0}}, // S&W Model 27 Magnum
    {"2sw10.i3d", {8, 6, 30, 0}}, // S&W Model 10 M&P
    {"2c1911.i3d", {9, 7, 35, 0}}, // Colt 1911
    {"2tommy.i3d", {10, 50, 200, 0}}, // Tommy Gun
    {"2shotgun.i3d", {11, 8, 32, 0}}, // Pump-action Shotgun
    {"2sawoff2.i3d", {12, 2, 20, 0}}, // Sawnoff Shotgun
    {"2m1903.i3d", {13, 5, 20, 0}}, // US Rifle M1903 Springfield
    {"2mosin.i3d", {14, 5, 20, 0}}, // Mosin-Nagant 1891/30
    {"2grenade.i3d", {15, 0, 0, 0}}, // Grenade
    {"2key.i3d", {16, 0, 0, 0}}, // Key
    {"2bucket.i3d", {17, 0, 0, 0}}, // Bucket
    {"2flshlight.i3d", {18, 0, 0, 0}}, // Flashlight
    {"2xkniha 7.i3d", {19, 0, 0, 0}}, // Book (Documents)
    {"2bar.i3d", {20, 0, 0, 0}}, // Bar
    {"2papers.i3d", {21, 0, 0, 0}}, // Papers
    {"2bomb.i3d", {22, 0, 0, 0}}, // Dat BOI, which goes BOOM.
    {"9klice.i3d", {23, 0, 0, 0}}, // Door Keys
    {"9klic1.i3d", {24, 0, 0, 0}}, // Safe Key
    {"2crowbar.i3d", {25, 0, 0, 0}}, // Crowbar (λ STYLE)
    {"fmv9letenka.i3d", {26, 0, 0, 0}}, // Fly Tickets
    {"9balik.i3d", {27, 0, 0, 0}}, // Box (Balík)
    {"2prkno.i3d", {28, 0, 0, 0}}, // Plank (Board)
    {"2cbottle.i3d", {29, 0, 0, 0}}, // Broken Bottle
    {"9klic1.i3d", {30, 0, 0, 0}}, // Small Key
    {"2swr.i3d", {31, 0, 0, 0}}, // Sword
    {"hlavapsa.i3d", {32, 0, 0, 0}}, // Dog's head
};
