int oak_killbox_set(float level) {
    return oak_config_killbox_set(level);
}

float oak_killbox_get() {
    return oak_config_killbox_get();
}


/* TEMP WEAPON STUFF */
struct tmp_weapon {
    const char *model;
    inventory_item item;
};

tmp_weapon colt = { "2c1911.i3d",{ 9, 50, 50, 0 } };
tmp_weapon tommy = { "2tommy.i3d",{ 10, 50, 50, 0 } };
tmp_weapon grenade = { "2grenade.i3d",{ 15, 1, 1, 0 } };
//weapon palka = { "2palka.i3d",{ 4, 1, 1, 0 } };

tmp_weapon *tmp_weaponlist[] = {
    &tommy,
    &colt,
    &grenade
  //  &palka
};

int oak_temp_weapons_spawn(oak_player id) {
    if (oak_player_invalid(id)) return -1;
    auto player = oak_entity_player_get(id);

    for (int g = 0; g < zpl_count_of(tmp_weaponlist); g++) {
        auto gun = &tmp_weaponlist[g]->item;

        //serverside insert weapon into free slot
        for (size_t i = 0; i < 8; i++)
        {
            auto cur_item = player->inventory.items[i];
            if (cur_item.weaponId == -1)
            {
                player->inventory.items[i] = *gun;
                break;
            }
        }

        //set new weapon as current
        player->current_weapon_id = gun->weaponId;
    }

    return 0;
}
