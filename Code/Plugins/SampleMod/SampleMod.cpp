/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

//
// Mod Includes
//

#define ZPL_IMPLEMENTATION
#include "zpl.h"
#include "zpl_math.h"

#include <Oakwood/Framework.hpp>
#include "weapons.hpp"
#include <iostream>

//
// Entry point
//
GameMode *gm = nullptr;

struct VehicleSpawn {
    zpl_vec3 pos;
    float rot;
    int modelID;
};

inline auto mode_generate_spawn() -> zpl_vec3 {
    return { -1984.884277f, -5.032383f, 23.144674f };
}

#define VEHICLE_INACTIVE_REMOVAL_TIME 5.0f * 60.0f

// Represents vehicle spawned by commands
class SpawnedVehicle {
public:
    SpawnedVehicle(Vehicle *car) {
        spawnTime = zpl_time_now();
        spawnedVehicle = car;
    }

    bool CheckForRemoval() {
        if (zpl_time_now() - spawnTime > VEHICLE_INACTIVE_REMOVAL_TIME) {

            if (spawnedVehicle->IsBeingStreamed()) {
                spawnTime = zpl_time_now();
                return false;
            }

            spawnedVehicle->Destroy();
            mod_log("[SampleMod] Vehicle has been removed due to inactivity!");
            return true;
        }

        return false;
    }

private:
    f64 spawnTime;
    Vehicle *spawnedVehicle;
};

std::vector<SpawnedVehicle> spawnedVehicles;

std::vector<VehicleSpawn> vehicle_spawns = {
    {{-1991.89f, -5.09753f, 10.4476f}, 0.0f, 148}, // Manta Prototype
    {{-1974.2f, -4.8862f, 22.5578f}, 0.0f, 148}, // Manta Prototype
    {{-1981.11f, -4.98206f, 22.7471f}, 0.0f, 148}, // Manta Prototype
    {{-1991.69f, -5.12453f, 22.3242f}, 0.0f, 148}, // Manta Prototype
};


OAK_MOD_MAIN /* (oak_api *mod) */ {

    // Set up mod information

    mod->name = "freeride";
    mod->author = "Oak Devs";
    mod->version = "v1.0.0";

    // Initialize the GameMode

    gm = new GameMode(mod);

    // Spawn default vehicles
    for (auto vehicle_spawn : vehicle_spawns) {
        auto vehicle = gm->SpawnVehicleByID(vehicle_spawn.pos, vehicle_spawn.rot, vehicle_spawn.modelID);
        vehicle->ShowOnMap(true);
    }

    // Load extra cars from file
    std::string currentLine;
    std::ifstream inputFile("static/savedcars.txt");
    while (inputFile.good()) {
        std::getline(inputFile, currentLine);
        zpl_vec3 pos;
        int model_id;
        float dir;
        ::sscanf(currentLine.c_str(), "%d %f %f %f %f", &model_id, &dir, EXPLODE_VEC(&pos));
        auto vehicle = gm->SpawnVehicleByID(pos, dir, model_id);
        vehicle->ShowOnMap(true);
    }

    // Register several events
    gm->SetOnPlayerConnected([=](Player *player) {
        gm->BroadcastMessage("Player " + player->GetName() + " joined the server.");

        add_weapons(player);

        player->Spawn(mode_generate_spawn());
        player->ShowOnMap(true);
    });

    gm->SetOnPlayerDisconnected([=](Player *player) { 
        gm->BroadcastMessage("Player " + player->GetName() + " left the server."); 
    });

    gm->SetOnPlayerDied([=](Player *player) {
        player->Fadeout(true, 500, 0xFFFFFF);
        player->Fadeout(false, 500, 0xFFFFFF);

        auto wep = get_weapon_by_id(player->GetCurrentWeapon());

        if (wep) {
            //gm->SpawnWeaponDrop(player->GetPosition(), wep->model, wep->item);
        }

        player->ClearInventory();
        player->SetHealth(100);

        add_weapons(player);

        player->Spawn(mode_generate_spawn());
        player->ShowOnMap(true);

        gm->BroadcastMessage("Player " + player->GetName() + " has died.");
    });

    gm->SetOnPlayerHit([=](Player *attacker, Player *victim, float damage) {
        // do something cool when player is hit
    });

    gm->SetOnPlayerChat([=](Player *player, std::string msg) {
        if (msg[0] == '/') {
            gm->SendMessageToPlayer("Unknown command: " + msg, player);
            return true;
        }

        gm->ChatPrint("<" + player->GetName() + "> " + msg);

        return true;
    });

    gm->SetOnServerTick([=]() {
        // Handle spawned cars
        {
            spawnedVehicles.erase(
                std::remove_if(
                    spawnedVehicles.begin(),
                    spawnedVehicles.end(),
                    [](SpawnedVehicle& v) { return v.CheckForRemoval(); }
                ),
                spawnedVehicles.end()
            );
        }
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

        auto modelID = StringToInteger(args[1]);

        auto position = player->GetPosition();
        auto dir = ComputeDirVector(player->GetRotation());
        dir *= 1.5f;
        position += dir;
        auto rot = player->GetRotation() - 90.0f;
        auto vehicle = gm->SpawnVehicleByID(position, rot, modelID);
        vehicle->ShowOnMap(true);

        spawnedVehicles.push_back(SpawnedVehicle(vehicle));
        
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

        auto modelID = StringToInteger(args[1]);
        auto vehicle = gm->SpawnVehicleByID(player->GetPosition(), player->GetRotation(), modelID);
        vehicle->ShowOnMap(true);

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

        auto modelID = StringToInteger(args[1]);

        player->SetModelByID(modelID);

        return true;
    });

    gm->AddCommandHandler("/healme", [=](Player *player, ArgumentList args) {
        player->SetHealth(100.0f);
        return true;
    });

    gm->AddCommandHandler("/hidemycar", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (!vehicle) {
            gm->SendMessageToPlayer("You are not sitting in a car!", player);
            return true;
        } else if (vehicle->GetPlayerSeatID(player) != 0) {
            gm->SendMessageToPlayer("You are not a driver!", player);
            return true;
        }

        vehicle->ShowOnRadar(!vehicle->GetRadarVisibility());

        return true;
    });

    gm->AddCommandHandler("/tp", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (vehicle) {
            gm->SendMessageToPlayer("You are sitting in a car!", player);
            return true;
        }

        auto playerId = StringToInteger(args[1]);

        if (playerId < 0 || playerId >= gm->players.GetNumberOfObjects()) {
            gm->SendMessageToPlayer("Invalid ID!", player);
            return true;
        }

        auto sndPlayer = gm->players.GetObjectByID(playerId);

        if (!sndPlayer) {
            gm->SendMessageToPlayer("Invalid ID!", player);
            return true;
        }

        player->SetPosition(sndPlayer->GetPosition());
        return true;
    });

    gm->AddCommandHandler("/listplayers", [=](Player *player, ArgumentList args) { 
        gm->SendMessageToPlayer("Online players:", player);

        for (int32_t i = 0; i < gm->players.GetNumberOfObjects(); i++) {
            gm->SendMessageToPlayer(std::to_string(i)+". "+gm->players.GetObjectByID(i)->GetName(), player);
        }

        return true;
    });

    gm->AddCommandHandler("/pete", [=](Player *player, ArgumentList args) {
        if (player->GetVehicle()) {
            gm->SendMessageToPlayer("you need to be on foot!", player);
            return true;
        }

        player->SetPosition({61.4763f, 4.72524f, 107.708f});

        return true;
    });

    gm->AddCommandHandler("/kill", [=](Player *player, ArgumentList args) {
        player->Die();
        return true;
    });

    gm->AddCommandHandler("/anim", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            gm->SendMessageToPlayer("USAGE: /anim [name]", player);
            return true;
        }

        auto animName = gm->ImplodeArgumentList(args);

        player->PlayAnimation(animName);
        return true;
    });
}

OAK_MOD_SHUTDOWN {
    zpl_printf("Freeride is shutting down...\n");
}
