/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

//
// Mod Includes
//

#include <Oakwood/Oakwood.hpp>
#include <iostream>
#include "weapons.hpp"

//
// Entry point
//

struct VehicleSpawn {
	zpl_vec3 pos;
	zpl_vec3 rot;
	const char* model;
};

std::vector<VehicleSpawn> vehicle_spawns = {
	{
		{-1991.89f, -5.09753f, 10.4476f},
		{0.00678947f, 0.0f, -0.999977f},
		"merced500K00.i3d"
	},

	{
		{ -1974.2f, -4.8862f, 22.5578f },
		{ 0.00678947f, 0.0f, 0.999977f },
		"arrow00.i3d"
	},

	{
		{ -1981.11f, -4.98206f, 22.7471f },
		{ 0.00678947f, 0.0f, 0.999977f },
		"blackha00.i3d"
	},

	{
		{ -1991.69f, -5.12453f, 22.3242f },
		{ 0.00678947f, 0.0f, 0.999977f },
		"blackha00.i3d"
	},
};

OAK_MOD_MAIN {

    // Set up mod information

    mod->name = "freeride";
    mod->author = "Oak Devs";
    mod->version = "v1.0.0";

    // Initialize the GameMode

    auto gm = new GameMode(mod);

	// Spawn default vehicles
	for (auto vehicle_spawn : vehicle_spawns) {
		gm->SpawnVehicle(vehicle_spawn.pos, vehicle_spawn.rot, vehicle_spawn.model);
	}

    // Register several events

    gm->SetOnPlayerConnected([=](Player *player) {
        gm->BroadcastMessage("Player " + player->GetName() + " joined the server.");

        player->SetPosition(mode_generate_spawn());

        add_weapons(player);

        player->Spawn();
    });

    gm->SetOnPlayerDisconnected([=](Player *player) {
        gm->BroadcastMessage("Player " + player->GetName() + " left the server.");
    });

    gm->SetOnPlayerDied([=](Player *player) {
 
        player->Fadeout(true, 500, 0xFFFFFF);
        player->Fadeout(false, 500, 0xFFFFFF);

        auto wep = get_weapon_by_id(player->GetCurrentWeapon());

        if (wep) {
            gm->SpawnWeaponDrop(player->GetPosition(), wep->model, wep->item);
        }

        player->ClearInventory();

        player->SetPosition(mode_generate_spawn());

        player->SetHealth(100);

        add_weapons(player);

        player->Respawn();

        gm->BroadcastMessage("Player " + player->GetName() + " has died.");
    });

    gm->SetOnPlayerChat([=](Player *player, std::string msg) {
        if(msg.find("/car") != std::string::npos) {
            auto position = player->GetPosition();
            position.x += 1.5f;
			gm->SpawnVehicle(position, player->GetRotation(), "taxi00.i3d");
			gm->ChatPrint(player->GetName() + " spawned taxi!");
        }

		if (msg.find("/savepos") != std::string::npos) {
			std::ofstream pos_file("positions.txt");
			auto position = player->GetPosition();
			auto rot = player->GetRotation();

			pos_file << position.x << " " << position.y << " " << position.z << ", " << rot.x << " " << rot.y << " " << rot.z << std::endl;

		}

        gm->ChatPrint(player->GetName() + " says: " + msg);

        return true;
    });
}