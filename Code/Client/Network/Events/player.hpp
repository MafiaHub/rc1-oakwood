#pragma once
#define TICKRATE_SERVER 30

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

inline auto player_game_tick(mafia_player* ped, f64 delta) -> void {

	// TODO(DavoSK): Move it to MafiaSDK
	// fix shooting ( fixed ammo for now :) )
	*(BYTE*)((DWORD)ped->ped + 0x4A4) = 50;
	*(BYTE*)((DWORD)ped->ped + 0x4A8) = 50;

	auto player_int = ped->ped->GetInterface();
	if (player_int->playersCar || player_int->carLeavingOrEntering || ped->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)
		return;

	f64	currentTime = zpl_time_now();

	// Position interpolation
	{
		f32 alpha = zpl_unlerp(currentTime, ped->interp.pos.startTime, ped->interp.pos.finishTime);
		alpha = zpl_clamp(0.0f, alpha, 1.5f);

		f32 currentAlpha = alpha - ped->interp.pos.lastAlpha;
		ped->interp.pos.lastAlpha = alpha;

		zpl_vec3 compensation;
		zpl_vec3_lerp(&compensation, ped->interp.pos.start, ped->interp.pos.target, alpha);

		if (alpha == 1.5f) ped->interp.pos.finishTime = 0;

		ped->ped->GetInterface()->entity.position = EXPAND_VEC(compensation);
	}

	// Rotation interpolation
	{
		f32 alpha = zpl_unlerp(currentTime, ped->interp.rot.startTime, ped->interp.rot.finishTime);
		alpha = zpl_clamp(0.0f, alpha, 1.5f);

		f32 currentAlpha = alpha - ped->interp.rot.lastAlpha;
		ped->interp.rot.lastAlpha = alpha;

		zpl_vec3 compensation;
		zpl_vec3_lerp(&compensation, ped->interp.rot.start, ped->interp.rot.target, alpha);

		if (alpha == 1.5f) ped->interp.rot.finishTime = 0;

		ped->ped->GetInterface()->entity.rotation = EXPAND_VEC(compensation);
	}

	// Pose interpolation
	{
		f32 alpha = zpl_unlerp(currentTime, ped->interp.pose.startTime, ped->interp.pose.finishTime);
		alpha = zpl_clamp(0.0f, alpha, 1.5f);

		f32 currentAlpha = alpha - ped->interp.pose.lastAlpha;
		ped->interp.pose.lastAlpha = alpha;

		zpl_vec3 compensation;
		zpl_vec3_lerp(&compensation, ped->interp.pose.start, ped->interp.pose.target, alpha);

		if (alpha == 1.5f) ped->interp.pose.finishTime = 0;

		Vector3D mafia_pose = EXPAND_VEC(compensation);

		if (ped->is_aiming)
			ped->ped->PoseSetPoseAimed(mafia_pose);
		else
			ped->ped->PoseSetPoseNormal(mafia_pose);
	}
}

inline auto player_entityupdate(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = player->ped->GetInterface();

	zpl_vec3 recv_pose, recv_rotation;
	librg_data_rptr(evnt->data, &recv_rotation, sizeof(zpl_vec3));
	librg_data_rptr(evnt->data, &recv_pose, sizeof(zpl_vec3));
	player->animation_state		= librg_data_ru8(evnt->data);
	player->is_crouching		= librg_data_ru8(evnt->data);
	player->is_aiming			= librg_data_ru8(evnt->data);
	player->aiming_time			= librg_data_ru64(evnt->data);
	
	// Position interpolation
	player->interp.pos.startTime	= zpl_time_now();
	player->interp.pos.finishTime	= player->interp.pos.startTime + (1.0f / TICKRATE_SERVER);
	player->interp.pos.lastAlpha	= 0.0f;
	player->interp.pos.start		= player->interp.pos.target;
	player->interp.pos.target		= evnt->entity->position;

	// Rotation interpolation
	player->interp.rot.startTime	= zpl_time_now();
	player->interp.rot.finishTime	= player->interp.rot.startTime + (1.0f / TICKRATE_SERVER);
	player->interp.rot.lastAlpha	= 0.0f;
	player->interp.rot.start		= player->interp.rot.target;
	player->interp.rot.target		= recv_rotation;

	// Pose interpolation
	player->interp.pose.startTime	= zpl_time_now();
	player->interp.pose.finishTime	= player->interp.pose.startTime + (1.0f / TICKRATE_SERVER);
	player->interp.pose.lastAlpha	= 0.0f;
	player->interp.pose.start		= player->interp.pose.target;
	player->interp.pose.target		= recv_pose;


	if (!player_int->carLeavingOrEntering) {
		player_int->animState	= player->animation_state;
		player_int->isDucking	= player->is_crouching;
		player_int->isAiming	= player->is_aiming;
		*(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;
	}

	if (player->vehicle_id != -1 && (player->clientside_flags & CLIENTSIDE_PLAYER_WAITING_FOR_VEH)) {
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
}

inline auto player_entityremove(librg_event* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	if (player && player->ped) {
		evnt->entity->flags &= ~ENTITY_INTERPOLATED;
		
		player_despawn(reinterpret_cast<MafiaSDK::C_Player*>(player->ped));

		free(player);
		evnt->entity->user_data = nullptr; 
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