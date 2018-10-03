/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

//
// Mod Includes
//

#include "Oakwood.hpp"
#include "weapons.hpp"

//
// Entry point
//

OAK_MOD_MAIN {

    // Set up mod information

    mod->name = "freeride";
    mod->author = "Oak Devs";
    mod->version = "v1.0.0";

    // Initialize the GameMode

    auto gm = new GameMode(mod);

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
        gm->ChatPrint(player->GetName() + " says: " + msg);

        return true;
    });
}