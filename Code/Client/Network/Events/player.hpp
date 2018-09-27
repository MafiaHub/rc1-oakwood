#pragma once

inline auto player_entitycreate(librg_event_t* evnt) -> void {

	auto player = new mafia_player();
	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3_t));
	librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3_t));
	librg_data_rptr(evnt->data, player->model, sizeof(char) * 32);
	librg_data_rptr(evnt->data, player->name, sizeof(char) * 32);
	player->is_crouching = librg_data_ru8(evnt->data);
	player->is_aiming = librg_data_ru8(evnt->data);
	librg_data_rptr(evnt->data, &player->inventory, sizeof(player_inventory));
	player->current_weapon_id = librg_data_ru32(evnt->data);
	player->health = librg_data_rf32(evnt->data);

	//feed interpolators
	player->inter_rot.init(player->rotation);
	player->inter_pos.init(evnt->entity->position);
	player->inter_pose.init(player->pose);

	player->ped = player_spawn(
		evnt->entity->position,
		player->rotation,
		player->inventory,
		player->model,
		player->current_weapon_id,
		player->health,
		false,
		-1);

	evnt->entity->user_data = player;
}

inline auto player_game_tick(mafia_player* player) -> void {

	//fix shooting ( fixed ammo for now :) )
	*(BYTE*)((DWORD)player->ped + 0x4A4) = 50;
	*(BYTE*)((DWORD)player->ped + 0x4A8) = 50;

	//update interpolated stuff :)
	auto player_int = player->ped->GetInterface();
	player_int->entity.position = EXPAND_VEC(player->inter_pos.interpolate());
	player_int->entity.rotation = EXPAND_VEC(player->inter_rot.interpolate());

	Vector3D mafia_pose = EXPAND_VEC(player->inter_pose.interpolate());
	if (player->is_aiming)
		player->ped->PoseSetPoseAimed(mafia_pose);
	else 
		player->ped->PoseSetPoseNormal(mafia_pose);
}

inline auto player_entityupdate(librg_event_t* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = player->ped->GetInterface();

	librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_vec3_t));
	librg_data_rptr(evnt->data, &player->pose, sizeof(zpl_vec3_t));
	player->animation_state = librg_data_ru8(evnt->data);
	player->is_crouching = librg_data_ru8(evnt->data);
	player->is_aiming = librg_data_ru8(evnt->data);
	player->aiming_time = librg_data_ru64(evnt->data);

	//feed interpolators
	player->inter_rot.set(player->rotation);
	player->inter_pos.set(evnt->entity->position);
	player->inter_pose.set(player->pose);

	player_int->animState = player->animation_state;
	player_int->isDucking = player->is_crouching;
	player_int->isAiming = player->is_aiming;
	*(DWORD*)((DWORD)player_int + 0xAD4) = player->aiming_time;
}

inline auto player_entityremove(librg_event_t* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	if (player->ped) {
		/*auto player_frame = *(DWORD*)((DWORD)player->ped + 0x68);
		
		if (player_frame) {
			__asm {
				mov eax, player_frame
				push eax
				mov ecx, [eax]
				call dword ptr ds : [ecx]
			}
		}

		MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player->ped);
		player->ped = nullptr;*/
		Vector3D haha = { 0.0, 0.0, 0.0 };
		player->ped->GetInterface()->entity.frame->SetOn(false);
		player->ped->GetInterface()->entity.position = haha;
		player->ped->GetInterface()->entity.frame->SetPos(haha);
	}
}

inline auto player_clientstreamer_update(librg_event_t* evnt) -> void {
	auto player = (mafia_player *)evnt->entity->user_data;
	auto player_int = local_player.ped->GetInterface();

	if (!player_int) {
		librg_event_reject(evnt);
		return;
	}

	evnt->entity->position	= EXPAND_VEC(player_int->humanObject.entity.position);
	player->rotation		= EXPAND_VEC(player_int->humanObject.entity.rotation);
	player->pose			= local_player.pose;
	player->animation_state = player_int->humanObject.animStateLocal;
	player->is_crouching	= player_int->humanObject.isDucking;
	player->is_aiming		= player_int->humanObject.isAiming;
	player->aiming_time		= *(DWORD*)((DWORD)player_int + 0xAD4);

	librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_vec3_t));
	librg_data_wptr(evnt->data, &player->pose, sizeof(zpl_vec3_t));
	librg_data_wu8(evnt->data, player->animation_state);
	librg_data_wu8(evnt->data, player->is_crouching);
	librg_data_wu8(evnt->data, player->is_aiming);
	librg_data_wu64(evnt->data, player->aiming_time);
}

auto mod_player_add_events() {
    librg_event_add(&network_context, LIBRG_CONNECTION_REQUEST, [](librg_event_t *evnt) {
        // TODO: password sending
        char nickname[32];
        strcpy(nickname, GlobalConfig.username.c_str());
        librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);
    });
}