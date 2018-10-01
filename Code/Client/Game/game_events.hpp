#pragma once

//TODO, remove this hax for forcing ammo
void librg_entites_tick(librg_entity_t* ent) {
	*(int*)((DWORD)ent->user_data + 0x4A4) = 50;
}

//TODO, change menu_skip with mission string comparing !
int menu_skip = 0;
auto mod_bind_events() {

	MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {

		if (!menu_skip) {
			menu_skip = 1;
			return;
		}

		if (menu_skip == 1) {
			// disable traffic
			MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);

			// connecting camera look at the LockAt more cuz it's weird
 			auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
			Vector3D cam_pos = { -1788.927612f, -4.542368f,  -9.105090f };
			Vector3D cam_rot = { 0.982404f,0.520000f, 1.017291f };
			cam->LockAt(cam_pos, cam_rot);
			
			// welcome msg
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText("Welcome to Mafia Oakwood 0.1", 0xFF0000);
			std::string connect_string = "Connecting to " + GlobalConfig.server_address + " ...";
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText(connect_string.c_str(), 0xFFFFFF);
			mod_librg_connect();
			menu_skip = 0;
			return;
		}
		menu_skip++;
	});

	local_player_init();
	drop_init();

	MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {
		
		librg_tick(&network_context);
		for (u32 i = 0; i < network_context.max_entities; i++) {
			
			librg_entity_t *entity = librg_entity_fetch(&network_context, i);
			if (!entity || entity->id == local_player.entity.id) continue;

			switch (entity->type) {
				case TYPE_WEAPONDROP: {
					auto weapon_drop = (mafia_weapon_drop*)entity->user_data;
					if (weapon_drop && weapon_drop->weapon_drop_actor)
						drop_game_tick(weapon_drop);

				} break;

				case TYPE_PLAYER: {
					auto player = (mafia_player*)entity->user_data;
					if (player && player->ped) {
						player_game_tick(player);
					}
				} break;
			}
		}
	});	
}