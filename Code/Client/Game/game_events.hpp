#pragma once

//TODO, remove this hax for forcing ammo
void librg_entites_tick(librg_entity_t* ent) {
	*(int*)((DWORD)ent->user_data + 0x4A4) = 50;
}

u32 transition_idx	= 0;
f64 last_time		= 0.0f;
f64 passed_time		= 2.0f;

std::vector<std::pair<zpl_vec3, zpl_vec3>> camera_follow_points = {
	// Salieri bar
    { { -1742.34f, 0.316043f, -13.9444f  },{ -0.891282f, -0.00174832f, 0.45345f } },
 	{ { -1793.5f, -4.52254f, -6.82448f },{ 0.800482f, 0.280663f, 0.599357f } },
	// Near hospital
	{ { -947.722f,  11.1498f,  630.033f },{ 0.765135f,  0.0662749f,  0.64387f } },
	{ { -881.001f,  16.9289f,  686.176f },{ 0.762883f,  0.275639f, 0.646537f } },
	// Light house
	{ { 1001.26f, 33.8752f, -921.13f },{ -0.273181f, 0.114938f, 0.961963f } },
	{ { 856.086f, 77.5979f, -922.258f },{ 0.680891f, -0.392336f, 0.732385f } },
	// Theatre
	{ { -997.554f, 4.38823f, -176.89f },{ -0.676999f, -0.0104618f, 0.735984f } },
	{ { -1080.7f, -4.46511f, -178.232f },{ 0.563889f, 0.297369f, 0.825851f } },
	//China town 
	{ { -1558.17f, 17.2702f, 583.109f },{ -0.999885f, 0.069762f, -0.0151395f } },
	{ { -1806.01f, 19.1066f, 583.164f },{ -0.824788f, 0.258827f, 0.565441f } },
};

auto interpolate_cam() {
	auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
	auto from = camera_follow_points.at(transition_idx);
	auto to = camera_follow_points.at(transition_idx + 1);

	if (passed_time > 0.8f && passed_time < 0.82f) {
		MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(true, 1000, 0x000000);
	}

	if (passed_time > 1.0f) {		
		if (transition_idx + 2 > camera_follow_points.size() - 1)
			transition_idx = 0;
		else 
			transition_idx += 2;

		MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
		passed_time = 0.0f;
	}

	f64 delta_time = zpl_time_now() - last_time;

	zpl_vec3 dest_pos;
	zpl_vec3_lerp(&dest_pos, from.first, to.first, passed_time);

	zpl_vec3 dest_rot;
	zpl_vec3_lerp(&dest_rot, from.second, to.second, passed_time);

	Vector3D pos = EXPAND_VEC(dest_pos);
	Vector3D rot = EXPAND_VEC(dest_rot);
	cam->LockAt(pos, rot);

	passed_time += delta_time * 0.11f;
	last_time = zpl_time_now();
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
			
			MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);
			chat::chat_messages.push_back(std::make_pair(ImVec4(150.0, 0.0, 0.0, 1.0), "Welcome to Mafia Oakwood 0.1"));
			chat::chat_messages.push_back(std::make_pair(ImVec4(1.0, 1.0, 1.0, 1.0), "Connecting to " + GlobalConfig.server_address + " ..."));
			mod_librg_connect();
			menu_skip = 0;
			return;
		}
		menu_skip++;
	});

	local_player_init();
	drop_init();

	MafiaSDK::C_Indicators_Hooks::HookAfterDrawAll([&]() {
		
		if(!librg_is_connected(&network_context))
			interpolate_cam();

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