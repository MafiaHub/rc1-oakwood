#pragma once

struct weapon {
    const char *model;
    inventory_item item;
};

weapon colt = { "2c1911.i3d",{ 9, 50, 50, 0 } };
weapon grenade = { "2grenade.i3d",{ 15, 1, 1, 0 } };
weapon palka = { "2palka.i3d",{ 4, 1, 1, 0 } };

weapon *weaponlist[] = {
    &grenade,
    &colt,
    &palka
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
