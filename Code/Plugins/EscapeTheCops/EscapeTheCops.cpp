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
#include <random>

#define MAX_COPS 8
#define TIME_START_GAME 30

enum {
    STATE_WAITING_FOR_PLAYERS,
    STATE_GAMEPLAY,
    STATE_ENDSCENE,
};

enum {
    CAMERA_POLICE_STATION,
    NUM_CAMS,
};

struct CopVehicleSpawn {
    zpl_vec3 pos, rot;
};

struct CameraLook {
    zpl_vec3 pos, dir;
};

//
// Entry point
//
GameMode *gm = nullptr;
std::vector<Player*> cops;
Player* escapee = nullptr;
u32 gameState = STATE_WAITING_FOR_PLAYERS;
Timer onStart;

CopVehicleSpawn copSpawns[MAX_COPS] = {
    {},
};

CameraLook cameraLooks[NUM_CAMS] = {
    {{-1226.206177f, -3.348627f, -710.803467f}, {-0.484838f, 0.073238f, - 0.87460f}},
};

void setCameraPos(Player* player, u32 idx) {
    player->SetCamera(cameraLooks[idx].pos, cameraLooks[idx].dir);
}

void handleEscapeeSpawn(Player* player) {
    escapee = player;

    // poziciu, model, zbrane, auto
}

void handleCopSpawn(Player* player) {
    cops.push_back(player);
    u32 idx = cops.size() - 1;

    // poziciu, model, zbrane, auto
}

void handleSpectator(Player* player) {
    // spectovanie hracov
}

OAK_MOD_MAIN /* (oak_api *mod) */ {

    // Set up mod information

    mod->name = "EscapeTheCops";
    mod->author = "MafiaHub Group";
    mod->version = "v1.0.0";

    // Initialize the GameMode

    gm = new GameMode(mod);

    onStart.Init((f64)TIME_START_GAME, 1, [=]() {
        std::vector<Player*> players;

        if (gm->players.GetNumberOfObjects() < 2) {
            return;
        }

        for (size_t i = 0; i < gm->players.GetNumberOfObjects(); i++)
        {
            players.push_back(gm->players.GetObjectByID(i));
        }

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(players.begin(), players.end(), g);

        handleEscapeeSpawn(players.back());
        players.pop_back();

        u32 max_cops = zpl_min(players.size(), MAX_COPS);

        for (size_t i = 0; i < max_cops; i++)
        {
            handleCopSpawn(players.back());
            players.pop_back();
        }

        for (size_t i = 0; i < max_cops; i++)
        {
            handleSpectator(players.back());
            players.pop_back();
        }

        gameState = STATE_GAMEPLAY;
        gm->BroadcastMessage("Let's hunt!");
        // ukazat enemaka na 5 sekund a potom spawnut policajtov
    });

    // Register several events
    gm->SetOnPlayerConnected([=](Player *player) {
        gm->BroadcastMessage("Player " + player->GetName() + " joined the server.");
        setCameraPos(player, CAMERA_POLICE_STATION);
        
        switch (gameState) {
        case STATE_WAITING_FOR_PLAYERS:
            if (gm->players.GetNumberOfObjects() < 2)
                gm->BroadcastMessage("Waiting for 1 extra player!");
            break;

        case STATE_GAMEPLAY:
            // turn on spectator mode
            break;

        case STATE_ENDSCENE:
            // game has already ended, we won't do anything specific here, set cam pos to end scene
            break;
        }
        player->ShowOnMap(true);
    });

    gm->SetOnPlayerDisconnected([=](Player *player) { 
        gm->BroadcastMessage("Player " + player->GetName() + " left the server."); 
    });

    gm->SetOnPlayerDied([=](Player *player) {
        player->Fadeout(true, 500, 0xFFFFFF);
        player->Fadeout(false, 500, 0xFFFFFF);

        handleSpectator(player);

        gm->BroadcastMessage("Player " + player->GetName() + " has died.");
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
        switch (gameState) {
        case STATE_WAITING_FOR_PLAYERS:
            if (gm->players.GetNumberOfObjects() > 2 && !onStart.IsRunning()) {
                onStart.Start();
                gm->BroadcastMessage("Everyone is here, let's wait 30 seconds now!");
            }
            else if (gm->players.GetNumberOfObjects() < 2) {
                onStart.Stop();
            }
            break;
        }
    });
}

OAK_MOD_SHUTDOWN {
    zpl_printf("Freeride is shutting down...\n");
}
