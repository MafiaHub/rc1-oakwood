#pragma once
inventory_item thompson = { 10, 50, 50, 0 };
inventory_item colt		= { 9, 50, 50, 0 };

inline auto mode_generate_spawn() -> zpl_vec3 {
	return { -1984.884277f, -5.032383f, 23.144674f };
}

inline auto mode_broadcast_msg(const char* text, u32 color = 0xFFFFFF) {
	librg_send(&network_context, NETWORK_SEND_CONSOLE_MSG, data, {
		librg_data_wu32(&data, strlen(text));
		librg_data_wu32(&data, color);
		librg_data_wptr(&data, (void*)text, strlen(text));
	});
}

ZPL_EVENT(freeride_player_connected) {
	ZPL_EVENT_CAST(mode_event, data);

	//create tommy entity
	strcpy(data->player->model, "Tommy.i3d");

	//set default health
	data->player->health = 200.0f;

	//add weapons
	player_inventory_add(data->evnt->entity, &thompson);
	player_inventory_add(data->evnt->entity, &colt);

	//set position of entity
	data->evnt->entity->position = mode_generate_spawn();

	//sent player for making player spawned !
	player_send_spawn(data->evnt->entity);

	//broadcast welcome message
	std::string welcome_msg = "Player " + std::string(data->player->name) + " has joined server";
	mod_log(welcome_msg.c_str());
	mode_broadcast_msg(welcome_msg.c_str());
}

ZPL_EVENT(freeride_player_disconnected) {
	ZPL_EVENT_CAST(mode_event, data);

	auto player = (mafia_player*)data->player_ent->user_data;

	//broadcast disconnect message
	std::string disconnect_msg = "Player " + std::string(data->player->name) + " has disconnected";
	mod_log(disconnect_msg.c_str());
	mode_broadcast_msg(disconnect_msg.c_str());
}

ZPL_EVENT(freeride_player_died) {
	ZPL_EVENT_CAST(mode_event, data);

	auto player = (mafia_player*)data->player_ent->user_data;
	
	//set default health
	data->player->health = 200.0f;

	//add weapons
	player_inventory_add(data->player_ent, &thompson);
	player_inventory_add(data->player_ent, &colt);

	//set position of entity
	data->player_ent->position = mode_generate_spawn();
	
	player_send_respawn(data->player_ent);

	//broadcast die message
	std::string die_msg = "Player " + std::string(player->name) + " has died";
	mod_log(die_msg.c_str());
	mode_broadcast_msg(die_msg.c_str());
}

auto freeride_events_add() -> void {
	zpl_event_add(&gamemode_events, MODE_ON_PLAYER_CONNECTED, freeride_player_connected);
	zpl_event_add(&gamemode_events, MODE_ON_PLAYER_DISCONNECTED, freeride_player_disconnected);
	zpl_event_add(&gamemode_events, MODE_ON_PLAYER_DIED, freeride_player_died);
}