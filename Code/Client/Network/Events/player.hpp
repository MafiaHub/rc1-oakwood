#pragma once

inline auto player_entitycreate(librg_event* evnt) -> void {

	auto player					= new mafia_player();
	player->voice_channel		= voip::create_remote();
	player->vehicle_id			= librg_data_ri32(evnt->data);
	player->streamer_entity_id	= librg_data_ri32(evnt->data);
	
	if (player->vehicle_id != -1) {
		player->clientside_flags |= CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
	}

	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, player->model, sizeof(char) * 32);
	librg_data_rptr(evnt->data, player->name, sizeof(char) * 32);

	player->is_crouching = librg_data_ru8(evnt->data);
	player->is_aiming = librg_data_ru8(evnt->data);
	librg_data_rptr(evnt->data, &player->inventory, sizeof(player_inventory));
	player->current_weapon_id = librg_data_ru32(evnt->data);
	player->health = librg_data_rf32(evnt->data);

	player->target_pos = evnt->entity->position;
	player->target_rot = player->rotation;
	player->target_pose = player->pose;

	player->ped = player_spawn(
		evnt->entity->position,
		player->rotation,
		player->inventory,
		player->model,
		player->current_weapon_id,
		player->health,
		false,
		-1,
		player->vehicle_id != -1);

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
	f32 alpha = (f32)player->inter_delta / network_context.timesync.server_delay;
    player->inter_delta += delta;

	auto player_int = player->ped->GetInterface();

	if (player_int->playersCar || player_int->carLeavingOrEntering || player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)
		return;

	zpl_vec3 inter_pos, inter_rot;
	zpl_vec3_lerp(&inter_pos, player->last_pos, player->target_pos, alpha);
	zpl_vec3_lerp(&inter_rot, player->last_rot, player->target_rot, alpha);
	zpl_vec3_lerp(&player->pose, player->last_pose, player->target_pose, alpha);

	player_int->entity.position = EXPAND_VEC(inter_pos);
	player_int->entity.rotation = EXPAND_VEC(inter_rot);
	Vector3D mafia_pose			= EXPAND_VEC(player->pose);

	if (player->is_aiming)
		player->ped->PoseSetPoseAimed(mafia_pose);
	else 
		player->ped->PoseSetPoseNormal(mafia_pose);
}

inline auto player_entityupdate(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = player->ped->GetInterface();
	zpl_vec3 target_pose;
	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &target_pose, sizeof(zpl_vec3));
	player->animation_state		= librg_data_ru8(evnt->data);
	player->is_crouching		= librg_data_ru8(evnt->data);
	player->is_aiming			= librg_data_ru8(evnt->data);
	player->aiming_time			= librg_data_ru64(evnt->data);
	
	player->target_pos			= EXPAND_VEC(evnt->entity->position);
	player->target_rot			= EXPAND_VEC(player->rotation);
	player->target_pose			= target_pose;

	player->last_pos			= EXPAND_VEC(player_int->entity.position);
	player->last_rot			= EXPAND_VEC(player_int->entity.rotation);
	player->last_pose			= EXPAND_VEC(player->pose);

	player_int->animState		= player->animation_state;
	player_int->isDucking		= player->is_crouching;
	player_int->isAiming		= player->is_aiming;
	*(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;

	if (player->vehicle_id != -1 && player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH) {
		auto vehicle_ent = librg_entity_fetch(&network_context, player->vehicle_id);
		if (vehicle_ent && vehicle_ent->user_data) {
			auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
			player->clientside_flags &= ~CLIENTSIDE_PLAYER_WAITING_FOR_VEH;
			
			for (int i = 0; i < 4; i++) {
				if (vehicle->seats[i] == evnt->entity->id) {
					player->ped->Intern_UseCar(vehicle->car, i);
					break;
				}
			}
		}
	}

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