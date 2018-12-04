/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

//
// Mod Includes
//

#define ZPL_IMPLEMENTATION
#include "librg/zpl.h"

#include <Oakwood/Oakwood.hpp>
#include <iostream>
#include "weapons.hpp"

//
// Entry point
//

struct VehicleSpawn {
    zpl_vec3 pos;
    float rot;
    const char* model;
};

std::vector<VehicleSpawn> vehicle_spawns = {
    {
        {-1991.89f, -5.09753f, 10.4476f},
        0.0f,
        "alfa00.i3d"
    },

    {
        { -1974.2f, -4.8862f, 22.5578f },
        0.0f,
        "FordHOT00.i3d"
    },

    {
        { -1981.11f, -4.98206f, 22.7471f },
        0.0f,
        "FThot00.i3d"
    },

    {
        { -1991.69f, -5.12453f, 22.3242f },
        0.0f,
        "TBirdold00.i3d"
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
        auto vehicle = gm->SpawnVehicle(vehicle_spawn.pos, vehicle_spawn.rot, vehicle_spawn.model);
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

    gm->SetOnPlayerHit([=](Player *attacker, Player *victim, float damage) {
        // do something cool when player is hit
    });

    gm->SetOnPlayerChat([=](Player *player, std::string msg) {
        gm->ChatPrint("#AAAAAA<#FF0000" + player->GetName() + "#AAAAAA>#FFFFFF " + msg);

        return true;
    });

    gm->AddCommandHandler("/car", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            gm->SendMessageToPlayer("Usage: /car [modelID]", player);
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            gm->SendMessageToPlayer("You can't spawn another car from inside of vehicle!", player);
            return true;
        }

        auto modelID = std::stoi(args[1]);

        auto position = player->GetPosition();
        auto dir = ComputeDirVector(player->GetRotation());
        zpl_vec3_muleq(&dir, 1.5f);
        zpl_vec3_addeq(&position, dir);
        auto rot = player->GetRotation() - 90.0f;
        gm->SpawnVehicleByID(position, rot, modelID);

        return true;
    });

    gm->AddCommandHandler("/putcar", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            gm->SendMessageToPlayer("Usage: /putcar [modelID]", player);
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            gm->SendMessageToPlayer("You can't spawn another car from inside of vehicle!", player);
            return true;
        }

        auto modelID = std::stoi(args[1]);

        auto position = player->GetPosition();
        auto dir = ComputeDirVector(player->GetRotation());
        zpl_vec3_muleq(&dir, 1.5f);
        zpl_vec3_addeq(&position, dir);
        auto rot = player->GetRotation() - 90.0f;
        auto vehicle = gm->SpawnVehicleByID(position, rot, modelID);

        player->PutToVehicle(vehicle, 0);

        return true;
    });

    gm->AddCommandHandler("/skin", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            gm->SendMessageToPlayer("Usage: /skin [modelID]", player);
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            gm->SendMessageToPlayer("You can't change skin inside of vehicle!", player);
            return true;
        }

        auto modelID = std::stoi(args[1]);

        player->SetModelByID(modelID);

        return true;
    });

    gm->AddCommandHandler("/healme", [=](Player *player, ArgumentList args) {
        player->SetHealth(100.0f);
        return true;
    });

    gm->AddCommandHandler("/tp", [=](Player *player, ArgumentList args) {
        if (args.size() != 1) {
            gm->SendMessageToPlayer("You need to specify player name!", player);
            return true;
        }

        
        return true;
    });

    gm->AddCommandHandler("%42", [=](Player *player, ArgumentList args) {
        gm->ChatPrint("Be careful!");
        return true;
    });

    gm->AddCommandHandler("/hidemycar", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (!vehicle) {
            gm->SendMessageToPlayer("You are not sitting in a car!", player);
            return true;
        }
        else if (vehicle->GetPlayerSeatID(player) != 0) {
            gm->SendMessageToPlayer("You are not a driver!", player);
            return true;
        }

        vehicle->ShowOnRadar(!vehicle->GetRadarVisibility());

        return true;
    });
}

OAK_MOD_SHUTDOWN {
    printf("Freeride is shutting down...\n");
}
