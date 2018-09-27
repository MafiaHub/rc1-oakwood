#pragma once
inventory_item thompson = { 10, 50, 50, 0 };
inventory_item colt		= { 9, 50, 50, 0 };

inline auto mode_generate_spawn() -> zpl_vec3 {
	return { -1984.884277f, -5.032383f, 23.144674f };
}

inline auto mode_broadcast_msg(const char* text, u32 color = 0xFFFFFF) {
	librg_send(&ctx, NETWORK_SEND_CONSOLE_MSG, data, {
		librg_data_wu32(&data, strlen(text));
		librg_data_wu32(&data, color);
		librg_data_wptr(&data, (void*)text, strlen(text));
	});
}

inline auto mode_player_connected(librg_event_t* evnt, mafia_player* player) -> void {

	//create tommy entity
	strcpy(player->model, "Tommy.i3d");

	//set default health
	player->health = 200.0f;

	//add weapons
	player_inventory_add(evnt->entity, &thompson);
	player_inventory_add(evnt->entity, &colt);

	//set position of entity
	evnt->entity->position = mode_generate_spawn();

	//sent player for making player spawned !
	player_send_spawn(evnt->entity);

	//broadcast welcome message
	std::string welcome_msg = "Player " + std::string(player->name) + " has joined server";
	mod_log(welcome_msg.c_str());
	mode_broadcast_msg(welcome_msg.c_str());
}

inline auto mode_player_disconnected(librg_entity_t* player_ent) -> void {

	auto player = (mafia_player*)player_ent->user_data;

	//broadcast disconnect message
	std::string disconnect_msg = "Player " + std::string(player->name) + " has disconnected";
	mod_log(disconnect_msg.c_str());
	mode_broadcast_msg(disconnect_msg.c_str());
}

inline auto mode_player_died(librg_entity_t* player_ent) -> void {

	auto player = (mafia_player*)player_ent->user_data;
	
	//set default health
	player->health = 200.0f;

	//add weapons
	player_inventory_add(player_ent, &thompson);
	player_inventory_add(player_ent, &colt);

	//set position of entity
	player_ent->position = mode_generate_spawn();
	
	player_send_respawn(player_ent);

	//broadcast die message
	std::string die_msg = "Player " + std::string(player->name) + " has died";
	mod_log(die_msg.c_str());
	mode_broadcast_msg(die_msg.c_str());
}