#pragma once

//TODO, remove this hax for forcing ammo
void librg_entites_tick(librg_entity_t* ent) {
	*(int*)((DWORD)ent->user_data + 0x4A4) = 50;
}

u32 transition_idx	= 0;
f64 last_time		= 0.0f;
f64 passed_time		= 2.0f;

std::vector<std::pair<zpl_vec3, zpl_vec3>> camera_follow_points = {
	/*// Salieri bar
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
	{ { -1806.01f, 19.1066f, 583.164f },{ -0.824788f, 0.258827f, 0.565441f } },*/
	{ { 377.887f, 45.0663f, -312.636f },{ -0.192431f, -0.104528f, -0.981311f } },
	{ { 374.35f, 44.1696f, -317.256f },{ -0.194983f, -0.0732375f, -0.980807f } },

	{ { 208.902f, 32.1993f, -414.388f },{ 0.999219f, 0.158159f, 0.0395246f } },
	{ { 211.301f, 32.4001f, -414.306f },{ 0.99935f, 0.299042f, 0.0360365f } },

	{ { -272.304f, 17.6089f, -352.958f },{ -0.0996391f, 0.575007f, 0.995024f } },
	{ { -273.665f, 17.4891f, -351.581f },{ -0.159734f, 0.20108f, 0.98716f } },

	{ { -337.101f, 25.7366f, -345.136f },{ -0.619948f, -0.0436174f, -0.784643f } },
	{ { -338.079f, 23.6143f, -346.402f },{ -0.631818f, 0.128798f, -0.775117f } },

	{ { -570.787f, 4.23206f, -414.67f },{ -0.998873f, -0.0958434f, -0.0474665f } },
	{ { -573.983f, 3.92536f, -414.822f },{ -0.998873f, -0.0941059f, -0.0474665f } },

	{ { -661.216f, -2.19623f, -174.317f },{ -0.640014f, 0.0749803f, 0.768363f } },
	{ { -659.372f, -2.19623f, -172.781f },{ -0.640014f, 0.0749803f, 0.768363f } },

	{ { -840.896f, -5.7629f, -103.007f },{ -0.853791f, 0.393943f, -0.520617f } },
	{ { -840.882f, -5.70702f, -103.811f },{ -0.344526f, 0.25882f, -0.938777f } },

	{ { -948.201f, -5.75411f, -168.035f },{ -0.999912f, 0.859852f, 0.0133015f } },
	{ { -976.991f, -5.71983f, -168.637f },{ -0.999391f, 0.980268f, 0.0348869f } },

	{ { -1029.13f, -6.18722f, -162.653f },{ -0.47533f, 0.372977f, 0.879808f } },
	{ { -1032.5f, -6.43452f, -162.733f },{ -0.448725f, 0.382673f, 0.89367f } },

	{ { -1417.8f, 0.37499f, -225.593f },{ 0.937219f, 0.300729f, 0.348741f } },
	{ { -1416.3f, 0.856156f, -225.035f },{ 0.937219f, 0.300729f, 0.348741f } },

	{ { -1177.6f, -7.05525f, -420.047f },{ -0.353346f, 0.302394f, -0.935493f } },
	{ { -1178.38f, -7.05525f, -419.84f },{ -0.382129f, 0.290727f, -0.924109f } },
};

auto interpolate_cam(f64 delta_time) {
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

	zpl_vec3 dest_pos;
	zpl_vec3_lerp(&dest_pos, from.first, to.first, passed_time);

	zpl_vec3 dest_rot;
	zpl_vec3_lerp(&dest_rot, from.second, to.second, passed_time);

	Vector3D pos = EXPAND_VEC(dest_pos);
	Vector3D rot = EXPAND_VEC(dest_rot);
	cam->LockAt(pos, rot);
	passed_time += delta_time * 0.11f;
}

auto mod_bind_events() {

	local_player_init();
	drop_init();

	MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {

		auto mission_id = MafiaSDK::GetCurrentMissionID();
		if (mission_id == MafiaSDK::C_Mission_Enum::MissionID::FREERIDE || 
			mission_id == MafiaSDK::C_Mission_Enum::MissionID::FREERIDE) {

			MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);
			chat::chat_messages.push_back(std::make_pair(ImVec4(150.0, 0.0, 0.0, 1.0), "Welcome to Mafia Oakwood 0.1"));
			chat::chat_messages.push_back(std::make_pair(ImVec4(1.0, 1.0, 1.0, 1.0), "Connecting to " + GlobalConfig.server_address + " ..."));
			mod_librg_connect();
		}
	});

	MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {

		f64 delta_time = zpl_time_now() - last_time;

		if(!librg_is_connected(&network_context))
			interpolate_cam(delta_time);

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
					if (player && player->ped && player->streamer_entity_id != local_player.entity.id) {
						player_game_tick(player, delta_time);
					}
				} break;
			}
		}

		last_time = zpl_time_now();
	});

	MafiaSDK::C_Indicators_Hooks::HookAfterDrawAll([&]() {
		nameplates::render();
	});	
}