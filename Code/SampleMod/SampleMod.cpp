/*

    SampleMod is an example gamemode written for Oakwood MP.

*/

#include "mod_api.hpp"

struct weapon {
    const char *model;
    inventory_item item;
};

weapon colt = { "2c1911.i3d", {9, 50, 50, 0} };
weapon grenade = { "2grenade.i3d", {15, 1, 1, 0} };

oak_api *gm = 0;

inline auto mode_generate_spawn() -> zpl_vec3 {
	return { -1984.884277f, -5.032383f, 23.144674f };
}

weapon *weaponlist[] = {
    &colt,
    &grenade,
};

auto add_weapons(librg_entity_t *entity) {
    for (int i = 0; i < zpl_count_of(weaponlist); i++) {
        gm->vtable.player_inventory_add(entity, &weaponlist[i]->item);
    }
}

auto get_weapon_by_id(u32 id) -> weapon* {
    for (int i = 0; i < zpl_count_of(weaponlist); i++) {
        if (weaponlist[i]->item.weaponId == id)
            return weaponlist[i];
    }

    return nullptr;
}

ZPL_EVENT(freeride_player_connected) {
	ZPL_EVENT_CAST(mode_event, data);

	// create tommy entity
	strcpy(data->player->model, "Tommy.i3d");

	// set default health
	data->player->health = 200.0f;

    add_weapons(data->player_ent);

	// set position of entity
	data->player_ent->position = mode_generate_spawn();

	// spawn our player
	gm->vtable.player_spawn(data->player_ent);

	// broadcast welcome message
	std::string welcome_msg = "Player " + std::string(data->player->name) + " has joined server";
	printf(welcome_msg.c_str());
	gm->vtable.broadcast_msg(welcome_msg.c_str());
}

ZPL_EVENT(freeride_player_disconnected) {
	ZPL_EVENT_CAST(mode_event, data);

	auto player = (mafia_player*)data->player_ent->user_data;

	// broadcast disconnect message
	std::string disconnect_msg = "Player " + std::string(player->name) + " has disconnected";
	printf(disconnect_msg.c_str());
	gm->vtable.broadcast_msg(disconnect_msg.c_str());
}

ZPL_EVENT(freeride_player_died) {
	ZPL_EVENT_CAST(mode_event, data);

	auto player = (mafia_player*)data->player_ent->user_data;

    // drop current weapon
    auto wep = get_weapon_by_id(player->current_weapon_id);

    if (wep) {
        gm->vtable.drop_spawn(data->player_ent->position, (char *)wep->model, wep->item);
    }

    // clear player inventory
    for (size_t i = 0; i < 8; i++)
        player->inventory.items[i] = { -1, 0, 0, 0 };
	
	// set default health
	player->health = 200.0f;

    add_weapons(data->player_ent);

	// set position of entity
	data->player_ent->position = mode_generate_spawn();
	
	gm->vtable.player_respawn(data->player_ent);


	// broadcast die message
	std::string die_msg = "Player " + std::string(player->name) + " has died";
	printf(die_msg.c_str());
	gm->vtable.broadcast_msg(die_msg.c_str());
}

ZPL_EVENT (freeride_server_tick) {

}

OAK_MOD_MAIN {
    gm = mod;

    gm->name = "freeride";
    gm->author = "Oak Devs";
    gm->version = "v1.0.0";
    
    gm->vtable.register_event(MODE_ON_PLAYER_CONNECTED, freeride_player_connected);
    gm->vtable.register_event(MODE_ON_PLAYER_DISCONNECTED, freeride_player_disconnected);
    gm->vtable.register_event(MODE_ON_PLAYER_DIED, freeride_player_died);
    gm->vtable.register_event(MODE_ON_SERVER_TICK, freeride_server_tick);
}