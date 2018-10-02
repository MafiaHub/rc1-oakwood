/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

#include "Oakwood.hpp"


struct weapon {
    const char *model;
    inventory_item item;
};

weapon colt = { "2c1911.i3d",{ 9, 50, 50, 0 } };
weapon grenade = { "2grenade.i3d",{ 15, 1, 1, 0 } };

inline auto mode_generate_spawn() -> zpl_vec3 {
    return { -1984.884277f, -5.032383f, 23.144674f };
}

weapon *weaponlist[] = {
    &colt,
    &grenade,
};

auto add_weapons(Player *player) {
    for (int i = 0; i < zpl_count_of(weaponlist); i++) {
        player->AddItem(&weaponlist[i]->item);
    }
}

auto get_weapon_by_id(u32 id) -> weapon* {
    for (int i = 0; i < zpl_count_of(weaponlist); i++) {
        if (weaponlist[i]->item.weaponId == id)
            return weaponlist[i];
    }

    return nullptr;
}

OAK_MOD_MAIN {
    mod->name = "freeride";
    mod->author = "Oak Devs";
    mod->version = "v1.0.0";

    auto gm = new GameMode(mod);

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
}