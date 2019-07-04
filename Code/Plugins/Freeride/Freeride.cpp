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
#include <Oakwood/PlayerModelListing.hpp>
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

struct SpawnLocation {
    std::string name;
    zpl_vec3 pos;
};

SpawnLocation spawnLocs[] = {
    { "tommy", {8.62861251831f, 22.8868865967f, -602.147888184f}},
    { "oakhill", {738.030334473f, 106.889381409f, -228.563537598f}},
    { "hoboken", {537.296386719f, -5.01502513885f, 77.8488616943f}},
    { "downtown", {-188.796401978f, 18.6846675873f, -668.6328125f}},
    { "hospital", {-760.439697266f, 12.6204996109f, 753.350646973f}},
    { "central", {-1091.47839355f, -7.27131414413f, 5.55286931992f}},
    { "china", {-1709.24157715f, 16.0029373169f, 582.041442871f}},
    { "salieri", {-1774.59301758f, -4.88487052917f, -2.40491962433f}},
    { "work", {-2550.85546875f, -3.96487784386f, -554.806213379f}},
    { "pete", {61.4763f, 4.72524f, 107.708f}},
    { "racing", {-3534.42993164f, 7.05113887787f, -651.973388672f}}
};

inline auto mode_generate_spawn() -> zpl_vec3 {
    u32 idx = rand() % zpl_count_of(spawnLocs);
    return spawnLocs[idx].pos;
}

#define VEHICLE_INACTIVE_REMOVAL_TIME 5.0f * 60.0f

// Represents vehicle spawned by commands
class SpawnedVehicle {
public:
    SpawnedVehicle(Vehicle *car, Player *plr) {
        spawnTime = zpl_time_now();
        spawnedVehicle = car;
        user = plr;
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

    Vehicle *spawnedVehicle;
    Player *user;
private:
    f64 spawnTime;
};

std::vector<SpawnedVehicle> spawnedVehicles;

std::vector<VehicleSpawn> vehicle_spawns = {
    {{-1991.89f, -5.09753f, 10.4476f}, 0.0f, 148}, // Manta Prototype
    {{-1974.2f, -4.8862f, 22.5578f}, 0.0f, 148}, // Manta Prototype
    {{-1981.11f, -4.98206f, 22.7471f}, 0.0f, 148}, // Manta Prototype
    {{-1991.69f, -5.12453f, 22.3242f}, 0.0f, 148}, // Manta Prototype
};

void setRandomModel(Player* player) {
    srand((unsigned int)time(NULL));
    u32 random_model = rand() % (zpl_count_of(PlayerModelCatalogue) - 1);
    player->SetModelByID(random_model);
}

std::vector<u64> admins;

void loadAdmins() {
    std::string currentLine;
    std::ifstream inputFile("config/admins.txt");
    while (!inputFile.fail() && !inputFile.eof()) {
        std::getline(inputFile, currentLine);
        if (currentLine.length() == 0) continue;
        u64 hwid;
        char name[128] = { 0 };
        ::sscanf(currentLine.c_str(), "%llu %s", &hwid, name);
        printf("Registering admin: '%s'(%llu)\n", name, hwid);
        admins.push_back(hwid);
    }
    inputFile.close();
}

bool isPlayerAdmin(Player* plr) {
    for (auto id : admins) {
        if (id == plr->GetHWID()) return true;
    }

    return false;
}

OAK_MOD_MAIN /* (oak_api *mod) */ {

    // Set up mod information

    mod->name = "Freeride";
    mod->author = "MafiaHub Group";
    mod->version = "v1.0.0";

    // Initialize the GameMode

    gm = new GameMode(mod);
    loadAdmins();

    // Spawn default vehicles
    for (auto vehicle_spawn : vehicle_spawns) {
        auto vehicle = gm->SpawnVehicleByID(vehicle_spawn.pos, vehicle_spawn.rot, vehicle_spawn.modelID);
        vehicle->ShowOnMap(true);
    }

    // Load extra cars from file
    std::string currentLine;
    std::ifstream inputFile("static/savedcars.txt");
    while (!inputFile.fail() && !inputFile.eof()) {
        std::getline(inputFile, currentLine);
        if (currentLine.length() == 0) continue;
        zpl_vec3 pos;
        int model_id;
        float dir;
        ::sscanf(currentLine.c_str(), "%d %f %f %f %f", &model_id, &dir, EXPLODE_VEC(&pos));
        auto vehicle = gm->SpawnVehicleByID(pos, dir, model_id);
        vehicle->ShowOnMap(true);
    }
    inputFile.close();

    // Register several events
    gm->SetOnPlayerConnected([=](Player *player) {
        gm->BroadcastMessage("Player " + player->GetName() + " joined the server.");

        add_weapons(player);
        
        setRandomModel(player);
        player->Spawn(mode_generate_spawn());
        player->ShowOnMap(true);
    });

    gm->SetOnPlayerDisconnected([=](Player *player) { 
        gm->BroadcastMessage("Player " + player->GetName() + " left the server."); 
    });

    gm->SetOnPlayerKeyPressed([=](Player* player, int key, bool pressed) {
        
    });

    gm->SetOnVehicleDestroyed([=](Vehicle *vehicle) {
        /* auto it = std::find_if(spawnedVehicles.begin(), spawnedVehicles.end(), [&](const SpawnedVehicle& vs) {
            return vs.spawnedVehicle == vehicle;
        });

        if (it != spawnedVehicles.end()) {
            spawnedVehicles.erase(it);
        } */
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

        setRandomModel(player);
        player->Spawn(mode_generate_spawn());
        player->ShowOnMap(true);

        gm->BroadcastMessage("Player " + player->GetName() + " has died.");
    });

    gm->SetOnPlayerHit([=](Player *attacker, Player *victim, float damage) {
        // do something cool when player is hit
    });

    gm->SetOnPlayerChat([=](Player *player, std::string msg) {
        if (msg[0] == '/') {
            player->SendChatMessage("Unknown command: " + msg);
            return true;
        }

        gm->BroadcastChatMessage("<" + player->GetName() + "> " + msg);

        return true;
    });

    gm->SetOnServerTick([=]() {
        // Handle spawned cars
        /* {
            spawnedVehicles.erase(
                std::remove_if(
                    spawnedVehicles.begin(),
                    spawnedVehicles.end(),
                    [](SpawnedVehicle& v) { return v.CheckForRemoval(); }
                ),
                spawnedVehicles.end()
            );
        } */
    });

    gm->AddCommandHandler("/car", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("Usage: /car [modelID]");
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            player->SendChatMessage("You can't spawn another car from inside of vehicle!");
            return true;
        }

        auto modelID = StringToInteger(args[1]);

        if (modelID == -1) return true;
/* 
        auto oldCar = std::find_if(spawnedVehicles.begin(), spawnedVehicles.end(), [&](const SpawnedVehicle& vs) {
            return vs.user == player;
        });

        if (oldCar != spawnedVehicles.end()) {
            oldCar->spawnedVehicle->Destroy();
            spawnedVehicles.erase(oldCar);
        }
 */
        auto position = player->GetPosition();
        auto dir = ComputeDirVector(player->GetRotation());
        dir *= 1.5f;
        position += dir;
        auto rot = player->GetRotation() - 90.0f;
        auto vehicle = gm->SpawnVehicleByID(position, rot, modelID);
        if (!vehicle) return true;
        vehicle->ShowOnMap(true);

        //spawnedVehicles.push_back(SpawnedVehicle(vehicle, player));
        
        return true;
    });

    gm->AddCommandHandler("/putcar", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("Usage: /putcar [modelID]");
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            player->SendChatMessage("You can't spawn another car from inside of vehicle!");
            return true;
        }

        auto modelID = StringToInteger(args[1]);
        
        if (modelID == -1) return true;

/*         auto oldCar = std::find_if(spawnedVehicles.begin(), spawnedVehicles.end(), [&](const SpawnedVehicle& vs) {
            return vs.user == player;
        });

        if (oldCar != spawnedVehicles.end()) {
            oldCar->spawnedVehicle->Destroy();
            spawnedVehicles.erase(oldCar);
        }
 */
        auto vehicle = gm->SpawnVehicleByID(player->GetPosition(), player->GetRotation(), modelID);
        if (!vehicle) return true;
        vehicle->ShowOnMap(true);

        player->PutToVehicle(vehicle, 0);

        //spawnedVehicles.push_back(SpawnedVehicle(vehicle, player));

        return true;
    });

    gm->AddCommandHandler("/skin", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("Usage: /skin [modelID]");
            return true;
        }

        if (player->GetVehicle() != nullptr) {
            player->SendChatMessage("You can't change skin inside of vehicle!");
            return true;
        }

        auto modelID = StringToInteger(args[1]);

        if (modelID == -1) return true;

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
            player->SendChatMessage("You are not sitting in a car!");
            return true;
        } else if (vehicle->GetPlayerSeatID(player) != 0) {
            player->SendChatMessage("You are not a driver!");
            return true;
        }

        vehicle->ShowOnRadar(!vehicle->GetRadarVisibility());

        return true;
    });

    gm->AddCommandHandler("/tp", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /tp [id]");
            return true;
        }

        if (vehicle) {
            player->SendChatMessage("You are sitting in a car!");
            return true;
        }

        auto playerId = StringToInteger(args[1]);

        if (playerId == -1) return true;

        if (playerId < 0 || playerId >= (int)gm->players.GetNumberOfObjects()) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        auto sndPlayer = gm->players.GetObjectByID(playerId);

        if (!sndPlayer) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        player->SetPosition(sndPlayer->GetPosition());
        return true;
    });

    gm->AddCommandHandler("/spectate", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /spectate [id]");
            return true;
        }

        auto playerId = StringToInteger(args[1]);

        if (playerId == -1) return true;

        if (playerId < 0 || playerId >= (int)gm->players.GetNumberOfObjects()) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        auto sndPlayer = gm->players.GetObjectByID(playerId);

        if (!sndPlayer || sndPlayer == player) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        player->SetCameraTarget(sndPlayer);
        
        return true;
    });

    gm->AddCommandHandler("/stop", [=](Player* player, ArgumentList args) {
        player->ResetCamera();
        return true;
    });

    gm->AddCommandHandler("/list", [=](Player *player, ArgumentList args) { 
        player->SendChatMessage("Online players:");

        for (int32_t i = 0; i < (int)gm->players.GetNumberOfObjects(); i++) {
            player->SendChatMessage(std::to_string(i)+". "+gm->players.GetObjectByID(i)->GetName());
        }

        return true;
    });

    gm->AddCommandHandler("/telelist", [=](Player *player, ArgumentList args) {
        for (auto loc : spawnLocs) {
            player->SendChatMessage(loc.name);
        }

        return true;
    });

    gm->AddCommandHandler("/tele", [=](Player* player, ArgumentList args) {
        if (player->GetVehicle()) {
            player->SendChatMessage("you need to be on foot!");
            return true;
        }

        auto locName = gm->ImplodeArgumentList(args);

        for (auto loc : spawnLocs) {
            if (loc.name == locName) {
                player->SetPosition(loc.pos);
                return true;
            }
        }

        player->SendChatMessage("Location doesn't exist! Use /telelist to see all available locations");

        return true;
    });

    gm->AddCommandHandler("/kill", [=](Player *player, ArgumentList args) {
        player->Die();
        return true;
    });

    gm->AddCommandHandler("/anim", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /anim [name]");
            return true;
        }

        auto animName = gm->ImplodeArgumentList(args);

        player->PlayAnimation(animName);
        return true;
    });

    gm->AddCommandHandler("/repair", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (!vehicle) return true;

        vehicle->Repair();

        return true;
    });

    gm->AddCommandHandler("/delcar", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (!vehicle) return true;

        if (vehicle->GetPlayerSeatID(player) != 0) return true;

        auto it = std::find_if(spawnedVehicles.begin(), spawnedVehicles.end(), [&](const SpawnedVehicle& vs) {
            return vs.spawnedVehicle == vehicle;
        });

        if (it != spawnedVehicles.end()) {
            spawnedVehicles.erase(it);
        }

        vehicle->Destroy();

        return true;
    });

    gm->AddCommandHandler("/hideme", [=](Player* player, ArgumentList args) {
        player->ShowOnMap(!player->GetMapVisibility());
        player->ShowNameplate(!player->GetNameplateVisibility());
        return true;
    });

    gm->AddCommandHandler("/transparency", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /transparency [value 0.0 - 1.0]");
            return true;
        }

        auto vehicle = player->GetVehicle();

        if (!vehicle) return true;

        auto val = StringToReal(args[1]);

        if (val == -1) return true;

        val = zpl_clamp(val, 0, 1);

        vehicle->SetTransparency(val);

        return true;
    });

    gm->AddCommandHandler("/fuel", [=](Player *player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /fuel [value]");
            return true;
        }

        auto vehicle = player->GetVehicle();

        if (!vehicle) return true;

        auto val = StringToReal(args[1]);

        if (val == -1) return true;

        vehicle->SetFuel(val);

        return true;
    });

    gm->AddCommandHandler("/cols", [=](Player *player, ArgumentList args) {
        auto vehicle = player->GetVehicle();

        if (!vehicle) return true;

        auto state = !vehicle->GetCollisionState();

        vehicle->SetCollisionState(state);

        if (!state) {
            vehicle->SetTransparency(0.5f);
        }
        else {
            vehicle->SetTransparency(1.0f);
        }

        return true;
    });

    // Admin tools

    gm->AddCommandHandler("/ban", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /ban [id]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto playerId = StringToInteger(args[1]);

        if (playerId == -1) return true;

        auto sndPlayer = gm->players.GetObjectByID(playerId);

        if (!sndPlayer) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        sndPlayer->Ban();

        return true;
    });

    gm->AddCommandHandler("/kick", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /kick [id]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto playerId = StringToInteger(args[1]);

        if (playerId == -1) return true;
        auto sndPlayer = gm->players.GetObjectByID(playerId);

        if (!sndPlayer) {
            player->SendChatMessage("Invalid ID!");
            return true;
        }

        sndPlayer->Kick();

        return true;
    });

    gm->AddCommandHandler("/addwh", [=](Player* player, ArgumentList args) {
        if (args.size() < 3) {
            player->SendChatMessage("USAGE: /addwh [hwid] [name]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto hwid = StringToLong(args[1]);
        auto name = args[2];

        gm->AddWhitelist(hwid, name);

        return true;
    });

    gm->AddCommandHandler("/unban", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /unban [hwid]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto hwid = StringToLong(args[1]);

        gm->Unban(hwid);

        return true;
    });

    gm->AddCommandHandler("/rmwh", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /rmwh [hwid]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto hwid = StringToLong(args[1]);

        gm->RemoveWhitelist(hwid);

        return true;
    });

    gm->AddCommandHandler("/togglewh", [=](Player* player, ArgumentList args) {
        if (args.size() < 2) {
            player->SendChatMessage("USAGE: /unban [state]");
            return true;
        }

        if (!isPlayerAdmin(player)) {
            player->SendChatMessage("Not an admin!");
            return true;
        }

        auto state = StringToInteger(args[1]);

        gm->ToggleWhitelist(state);

        return true;
    });

}

OAK_MOD_SHUTDOWN {
    zpl_printf("Freeride is shutting down...\n");
}
