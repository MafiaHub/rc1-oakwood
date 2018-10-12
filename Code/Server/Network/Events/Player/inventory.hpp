inline auto player_inventory_debug(librg_entity* player_ent) -> void {
	auto player = (mafia_player*)player_ent->user_data;
	printf("-----------[INV]-----------\n");
	for (size_t i = 0; i < 8; i++) {
		auto daco = player->inventory.items[i];
		printf("%d %d %d\n", daco.weaponId, daco.ammoLoaded, daco.ammoHidden);
	}
	printf("Current wep: %d\n", player->current_weapon_id);
	printf("-----------[INV]-----------\n");
}

inline auto player_inventory_full(librg_entity* player_ent) -> bool {
	auto player = (mafia_player*)player_ent->user_data;
	for (size_t i = 0; i < 8; i++) {
		if (player->inventory.items[i].weaponId == -1)
			return false;
	}
	return true;
}

inline auto player_inventory_exists(librg_entity* player_ent, int id) -> bool {
	auto player = (mafia_player*)player_ent->user_data;
	for (size_t i = 0; i < 8; i++) {
		auto item = player->inventory.items[i];
		if (item.weaponId == id) {
			return true;
		}
	}
	return false;
}

inline auto player_inventory_add(
	librg_entity* player_ent, 
	inventory_item* item, 
	bool announce = false, 
	bool weapon_picked = false) -> void {

	auto player = (mafia_player*)player_ent->user_data;

	//inventory full dont add weapon
	if (player_inventory_full(player_ent)) {
		mod_debug("player_inventory_add inv full !");
		return;
	}

	//if weapon exists skip
	if (player_inventory_exists(player_ent, item->weaponId)) {
		mod_debug("player_inventory_add weapon exists !");
		return;
	}

	//serverside insert weapon into free slot
	for (size_t i = 0; i < 8; i++) {
		auto cur_item = player->inventory.items[i];
		if (cur_item.weaponId == -1) {
			player->inventory.items[i] = *item;
			break;
		}
	}

	//set new weapon as current
	player->current_weapon_id = item->weaponId;

	//non announce if we adding weapon while spawning 
	if (!announce) return;

	//check if weapon is picked case
	if (weapon_picked) {

		librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, player_ent->client_peer, data, {
			librg_data_went(&data, player_ent->id);
			librg_data_wptr(&data, item, sizeof(inventory_item));
		});

		mod_debug("sending weapon pickup");
		return;
	}

	//broadcast to others for giving a weapon
	librg_send(&network_context, NETWORK_PLAYER_WEAPON_ADD, data, {
		librg_data_went(&data, player_ent->id);
		librg_data_wptr(&data, item, sizeof(inventory_item));
	});
}

inline auto player_inventory_remove(
	librg_entity* player_ent, 
	int id, 
	bool announce = false,
	bool weapon_dropped = false) -> void {

	//do weapon exists in inventory ?
	if (!player_inventory_exists(player_ent, id)) return;
	
	//remove weapon from inventory
	auto player = (mafia_player*)player_ent->user_data;
	for (size_t i = 0; i < 8; i++) {
		auto item = player->inventory.items[i];
		if (item.weaponId == id) {

			//if removed weapon was current set hands
			if(item.weaponId == player->current_weapon_id)
				player->current_weapon_id = 0;

			player->inventory.items[i] = { -1, 0, 0, 0 };
		}
	}

	if (!announce) return;
	
	if (weapon_dropped) {
		librg_send_except(&network_context, NETWORK_PLAYER_WEAPON_DROP, player_ent->client_peer, data, {
			librg_data_went(&data, player_ent->id);
			librg_data_wu32(&data, id);
		});
		return;
	}

	//broadcast message
	librg_send(&network_context, NETWORK_PLAYER_WEAPON_REMOVE, data, {
		librg_data_went(&data, player_ent->id);
		librg_data_wu32(&data, id);
	});
}

auto player_inventory_send(librg_entity *player_ent) {
	auto player = (mafia_player *)player_ent->user_data;

	if (player) {
		librg_send_except(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, player_ent->client_peer, data, {
			librg_data_went(&data, player_ent->id);
			librg_data_wptr(&data, &player->inventory, sizeof(player_inventory));
		});
	}
}