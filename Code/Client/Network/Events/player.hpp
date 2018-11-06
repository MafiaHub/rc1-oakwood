#pragma once

inline auto player_entitycreate(librg_event* evnt) -> void {

	auto player = new mafia_player();
	player->voice_channel = voip::create_remote();
	player->streamer_entity_id = librg_data_ri32(evnt->data);

	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, player->model, sizeof(char) * 32);
	librg_data_rptr(evnt->data, player->name, sizeof(char) * 32);
	player->is_crouching = librg_data_ru8(evnt->data);
	player->is_aiming = librg_data_ru8(evnt->data);
	librg_data_rptr(evnt->data, &player->inventory, sizeof(player_inventory));
	player->current_weapon_id = librg_data_ru32(evnt->data);
	player->health = librg_data_rf32(evnt->data);

	player->ped = player_spawn(
		evnt->entity->position,
		player->rotation,
		player->inventory,
		player->model,
		player->current_weapon_id,
		player->health,
		false,
		-1);

	if(player->streamer_entity_id == local_player.entity.id) {
		auto me = local_player.ped;
		auto action = player->ped->GetActionManager()->NewFollow(me, 3.0f, 13, 2, 0, 0);
        player->ped->GetActionManager()->NewTurnTo(me, action->action_id);
        player->ped->GetActionManager()->AddJob(action);
        MafiaSDK::GetFollowManager()->AddFollower(player->ped, me);
        player->ped->ForceAI(0, 1, 0, 0);
	}

	evnt->entity->user_data = player;
	evnt->entity->flags |= ENTITY_INTERPOLATED;
}

inline auto player_game_tick(mafia_player* player, f64 delta) -> void {

	//fix shooting ( fixed ammo for now :) )
	*(BYTE*)((DWORD)player->ped + 0x4A4) = 50;
	*(BYTE*)((DWORD)player->ped + 0x4A8) = 50;

	//update interpolated stuff :)
	f32 alpha = player->inter_delta / network_context.timesync.server_delay;
    player->inter_delta += (f32)delta;

	auto player_int = player->ped->GetInterface();
	if(player_int->playersCar == nullptr) {
		player_int->entity.position = EXPAND_VEC(cubic_hermite_v3_interpolate(&player->inter_pos, alpha));
		player_int->entity.rotation = EXPAND_VEC(cubic_hermite_v3_interpolate(&player->inter_rot, alpha));
	}

	Vector3D mafia_pose = EXPAND_VEC(cubic_hermite_v3_interpolate(&player->inter_pose, alpha));
	if (player->is_aiming)
		player->ped->PoseSetPoseAimed(mafia_pose);
	else 
		player->ped->PoseSetPoseNormal(mafia_pose);
}

inline auto player_entityupdate(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = player->ped->GetInterface();

	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
	player->animation_state = librg_data_ru8(evnt->data);
	player->is_crouching = librg_data_ru8(evnt->data);
	player->is_aiming = librg_data_ru8(evnt->data);
	player->aiming_time = librg_data_ru64(evnt->data);
	
	/* update interpolation tables */
	cubic_hermite_v3_value(&player->inter_pos, evnt->entity->position);
	cubic_hermite_v3_value(&player->inter_rot, player->rotation);
	cubic_hermite_v3_value(&player->inter_pose, player->pose);

	player_int->animState = player->animation_state;
	player_int->isDucking = player->is_crouching;
	player_int->isAiming = player->is_aiming;
	*(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;

	player->inter_delta = 0.0f;
}

inline auto player_entityremove(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	if (player->ped) {
		evnt->entity->flags &= ~ENTITY_INTERPOLATED;
		
		player_despawn(reinterpret_cast<MafiaSDK::C_Player*>(player->ped));
		player->ped = nullptr;

		free(evnt->user_data);
		evnt->user_data = nullptr; 
	}
}

inline auto player_clientstreamer_update(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = reinterpret_cast<MafiaSDK::C_Player*>(player->ped)->GetInterface();

	if (!player_int) {
		librg_event_reject(evnt);
		return;
	}

	auto frame_position = player_int->humanObject.entity.frame->GetInterface()->mPosition;

	evnt->entity->position	= EXPAND_VEC(frame_position);
	player->rotation		= EXPAND_VEC(player_int->humanObject.entity.rotation);
	player->pose			= local_player.pose;
	player->animation_state = player_int->humanObject.animStateLocal;
	player->is_crouching	= player_int->humanObject.isDucking;
	player->is_aiming		= player_int->humanObject.isAiming;
	player->aiming_time		= *(DWORD*)((DWORD)player_int + 0xAD4);

	librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
	librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3));
	librg_data_wu8(evnt->data, player->animation_state);
	librg_data_wu8(evnt->data, player->is_crouching);
	librg_data_wu8(evnt->data, player->is_aiming);
	librg_data_wu64(evnt->data, player->aiming_time);
}

auto mod_player_add_events() {
    librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, [](librg_event *evnt) {
        // TODO: password sending
        char nickname[32];
        strcpy(nickname, GlobalConfig.username.c_str());
        librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);
    });
}