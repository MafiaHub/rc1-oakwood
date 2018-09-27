#pragma once
/*
* Librg events
*/
void on_librg_connect(librg_event_t* evnt) {
	
	MafiaSDK::GetMission()->GetGame()->GetCamera()->Unlock();

	auto local_player_data = new mafia_player;
	evnt->entity->user_data = local_player_data;
	local_player.entity = *evnt->entity;
}

void on_librg_entity_create(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entitycreate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			drop_entitycreate(evnt);
		} break;
	}
}

void on_librg_entity_update(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entityupdate(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			
		} break;
	}
}

void on_librg_entity_remove(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_entityremove(evnt);
		} break;
		case TYPE_WEAPONDROP: {
			drop_entityremove(evnt);
		} break;
	}
}

void on_librg_clientstreamer_update(librg_event_t* evnt) {
	switch (evnt->entity->type) {
		case TYPE_PLAYER: {
			player_clientstreamer_update(evnt);
		} break;
		case TYPE_WEAPONDROP: {
		} break;
	}
}

/* 
* Librg messages
*/
inline auto mod_librg_register_messages() -> void {

	librg_network_add(&ctx, NETWORK_PLAYER_RESPAWN, [](librg_message_t* msg) {

		//read data
		zpl_vec3 position, rotation;
		player_inventory inventory;
		char model[32];

		librg_entity_id player_id = librg_data_rent(msg->data);
		librg_data_rptr(msg->data, &position, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, &rotation, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, model, sizeof(char) * 32);
		librg_data_rptr(msg->data, &inventory, sizeof(player_inventory));
		u32 current_wep = librg_data_ru32(msg->data);
		f32 health = librg_data_rf32(msg->data);

		bool is_local_player = player_id == local_player.entity.id;

		//-- create new frame
		Vector3D default_scale = { 1.0f, 1.0f, 1.0f };
		Vector3D default_pos = EXPAND_VEC(position);

		auto player_frame = new MafiaSDK::I3D_Frame();
		player_frame->SetName("testing_player");
		player_frame->LoadModel(model);
		player_frame->SetScale(default_scale);
		player_frame->SetPos(default_pos);

		if (!is_local_player) {
			auto player_ent = librg_entity_fetch(&ctx, player_id);
			if (player_ent && player_ent->user_data) {
				auto player = (mafia_player*)player_ent->user_data;
				auto new_ped = reinterpret_cast<MafiaSDK::C_Human*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Enemy));
				new_ped->Init(player_frame);
				new_ped->SetShooting(1.0f);
				new_ped->SetBehavior(MafiaSDK::C_Human_Enum::BehaviorStates::DoesntReactOnWeapon);
				MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_ped);

				new_ped->GetInterface()->health = health;
				new_ped->GetInterface()->entity.position = default_pos;
				new_ped->GetInterface()->entity.rotation = EXPAND_VEC(rotation);

				//-- INV 
				for (size_t i = 0; i < 8; i++) {
					S_GameItem* item = (S_GameItem*)&inventory.items[i];
					if (item->weaponId != 0) {
						new_ped->G_Inventory_AddItem(*item);
					}
				}

				new_ped->G_Inventory_SelectByID(current_wep);
				new_ped->ChangeWeaponModel();

				if (current_wep == 0)
					new_ped->Do_Holster();
				//---

				printf("respawn remote !\n");

				//remove old ped
				//find new way how to properly remove players :)
				if (player->ped) {
					Vector3D haha = { 0.0, 0.0, 0.0 };
					player->ped->GetInterface()->entity.frame->SetOn(false);
					player->ped->GetInterface()->entity.position = haha;
					player->ped->GetInterface()->entity.frame->SetPos(haha);
					player->ped = new_ped;
				}

				//interpolator init
				player->inter_pos.init(position);
				player->inter_rot.init(rotation);
			}
		} else {
			auto new_ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Player));
			new_ped->Init(player_frame);
			new_ped->SetShooting(1.0f);
			MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_ped);
			MafiaSDK::GetMission()->GetGame()->SetLocalPlayer(new_ped);
			new_ped->GetInterface()->humanObject.health = health;
			new_ped->GetInterface()->humanObject.entity.position = default_pos;
			new_ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);

			//-- INV 
			for (size_t i = 0; i < 8; i++) {
				S_GameItem* item = (S_GameItem*)&inventory.items[i];
				if (item->weaponId != 0) {
					new_ped->G_Inventory_AddItem(*item);
				}
			}

			new_ped->G_Inventory_SelectByID(current_wep);
			new_ped->ChangeWeaponModel();

			if (current_wep == 0)
				new_ped->Do_Holster();
			//---

			MafiaSDK::GetMission()->GetGame()->GetCamera()->SetMode(true, 1);
			MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(new_ped);
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->PlayerSetWingmanLives(100);

			if (local_player.ped) {
				Vector3D haha = { 0.0, 0.0, 0.0 };
				local_player.ped->GetInterface()->humanObject.entity.position = { 0.0, 0.0, 0.0 };
				local_player.ped->GetInterface()->humanObject.entity.frame->SetOn(false);
				local_player.ped->GetInterface()->humanObject.entity.frame->SetPos(haha);
			}

			local_player.dead = false;
			local_player.ped = new_ped;
			MafiaSDK::GetMission()->GetGame()->GetIndicators()->FadeInOutScreen(false, 500, 0xFFFFFF);
		}
	});

	librg_network_add(&ctx, NETWORK_PLAYER_SPAWN, [](librg_message_t* msg) {
		
		zpl_vec3 position, rotation;
		player_inventory inventory;
		char model[32];

		librg_data_rptr(msg->data, &position, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, &rotation, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, model, sizeof(char) * 32);
		librg_data_rptr(msg->data, &inventory, sizeof(player_inventory));
		u32 current_wep = librg_data_ru32(msg->data);
		f32 health = librg_data_rf32(msg->data);

		Vector3D default_scale = { 1.0f, 1.0f, 1.0f };
		Vector3D default_pos = EXPAND_VEC(position);

		auto player_frame = new MafiaSDK::I3D_Frame();
		player_frame->SetName("testing_player");
		player_frame->LoadModel(model);
		player_frame->SetScale(default_scale);
		player_frame->SetPos(default_pos);

		local_player.ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Player));
		local_player.ped->Init(player_frame);
		local_player.ped->SetShooting(1.0f);

		MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(local_player.ped);
		MafiaSDK::GetMission()->GetGame()->SetLocalPlayer(local_player.ped);

		local_player.ped->GetInterface()->humanObject.health = health;
		local_player.ped->GetInterface()->humanObject.entity.position = default_pos;
		local_player.ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);

		for (size_t i = 0; i < 8; i++) {
			S_GameItem* item = (S_GameItem*)&inventory.items[i];
			if (item->weaponId != 0) {
				((MafiaSDK::C_Human*)local_player.ped)->G_Inventory_AddItem(*item);
			}
		}

		((MafiaSDK::C_Human*)local_player.ped)->G_Inventory_SelectByID(current_wep);
		((MafiaSDK::C_Human*)local_player.ped)->ChangeWeaponModel();

		if (current_wep == 0)
			((MafiaSDK::C_Human*)local_player.ped)->Do_Holster();

		MafiaSDK::GetMission()->GetGame()->GetCamera()->SetPlayer(local_player.ped);
	});
}

/*
* Librg init
*/
inline auto mod_librg_init_stuff() -> void {
	
	ctx.mode = LIBRG_MODE_CLIENT;
	ctx.max_entities = 16;
	ctx.tick_delay = 32;
	ctx.world_size = { 5000.0f, 5000.0f, 0.0f };

	librg_init(&ctx);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_librg_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_librg_entity_update);
	librg_event_add(&ctx, LIBRG_ENTITY_REMOVE, on_librg_entity_remove);
	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
	mod_librg_register_messages();


	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, [](librg_event_t *evnt) {
		// TODO: password sending
		char nickname[32];
		strcpy(nickname, GlobalConfig.username.c_str());
		librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);
	});


	librg_network_add(&ctx, NETWORK_PLAYER_SHOOT, [](librg_message_t* msg) {
		zpl_vec3 pos;
		librg_entity_id id = librg_data_rent(msg->data);
		librg_data_rptr(msg->data, &pos, sizeof(zpl_vec3));
		Vector3D target = EXPAND_VEC(pos);

		auto entity = librg_entity_fetch(&ctx, id);
		auto player = (mafia_player*)entity->user_data;

		*(BYTE*)((DWORD)player->ped + 0x4A4) = 50;
		player->ped->Do_Shoot(true, target);
		player->ped->Do_Shoot(false, target);
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_CHANGE, [](librg_message_t* msg) {
		librg_entity_id id = librg_data_rent(msg->data);
		u32 index = librg_data_ru32(msg->data);
		auto entity = librg_entity_fetch(&ctx, id);
		if (entity) {
			auto player = (mafia_player*)entity->user_data;
			player->current_weapon_id = index;
			player->ped->G_Inventory_SelectByID(index);
			player->ped->Do_ChangeWeapon(0, 0);
		}
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_DROP, [](librg_message_t* msg) {

		librg_entity_id id = librg_data_rent(msg->data);
		u32 remove_id = librg_data_ru32(msg->data);

		auto entity = librg_entity_fetch(&ctx, id); 
		if (entity) {
			auto player = (mafia_player*)entity->user_data;
			player->ped->G_Inventory_RemoveWeapon(remove_id);

			if (remove_id == player->current_weapon_id)
				player->current_weapon_id = 0;

			player->ped->G_Inventory_SelectByID(0);
			player->ped->Do_ChangeWeapon(0, 0);
		}
	});

	librg_network_add(&ctx, NETWORK_PLAYER_WEAPON_PICKUP, [](librg_message_t* msg) {
		librg_entity_id id = librg_data_rent(msg->data);
		auto entity = librg_entity_fetch(&ctx, id);
		auto player = (mafia_player*)entity->user_data;
		S_GameItem weapon_item;
		librg_data_rptr(msg->data, &weapon_item, sizeof(inventory_item));

		//give player weapon
		((MafiaSDK::C_Human*)player->ped)->G_Inventory_AddItem(weapon_item);
		if (player->current_weapon_id == weapon_item.weaponId) return;

		//set current weapon to new picked one
		player->current_weapon_id = weapon_item.weaponId;

		//apply model
		player->ped->G_Inventory_SelectByID(weapon_item.weaponId);
		player->ped->Do_ChangeWeapon(0, 0);
		player->ped->ChangeWeaponModel();

		mod_debug("weapon pickup");
	});

	librg_network_add(&ctx, NETWORK_PLAYER_HIT, [](librg_message_t* msg) {

		librg_entity_id sender_id = librg_data_rent(msg->data);
		auto sender_ent = librg_entity_fetch(&ctx, sender_id);

		u32 hit_type = librg_data_ru32(msg->data);
		Vector3D unk1, unk2, unk3;
		librg_data_rptr(msg->data, (void*)&unk1, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, (void*)&unk2, sizeof(zpl_vec3));
		librg_data_rptr(msg->data, (void*)&unk3, sizeof(zpl_vec3));

		f32 damage = librg_data_rf32(msg->data);
		u32 player_part = librg_data_ru32(msg->data);

		if (sender_ent && sender_ent->user_data && local_player.ped) {
			auto player = (mafia_player*)(sender_ent->user_data);
			hit_hook_skip = false;
			local_player.ped->Hit(hit_type, unk1, unk2, unk3, damage, player->ped, player_part, NULL);
			hit_hook_skip = true;
		}
	});

	librg_network_add(&ctx, NETWORK_PLAYER_DIE, [](librg_message_t* msg) {
		auto entity_id = librg_data_rent(msg->data);
		auto entity = librg_entity_fetch(&ctx, entity_id);
		if (entity->user_data) {
			auto player = (mafia_player*)(entity->user_data);
			if (player) {
				//player->ped->Intern_ForceDeath();
			}
		}
	});

	librg_network_add(&ctx, NETWORK_SEND_CONSOLE_MSG, [](librg_message_t* msg) {
		u32 msg_size	= librg_data_ru32(msg->data);
		u32 msg_color	= librg_data_ru32(msg->data);
		char* text = reinterpret_cast<char*>(malloc(msg_size));
		librg_data_rptr(msg->data, text, msg_size);
		text[msg_size]  = '\0';
		MafiaSDK::GetMission()->GetGame()->GetIndicators()->ConsoleAddText(reinterpret_cast<const char*>(text), msg_color);
	});
}

/*
*  Librg connect to server
*/
inline auto mod_librg_connect() -> void {
	librg_address_t addr = { 27010, (char*)GlobalConfig.server_address.c_str()};
	librg_network_start(&ctx, addr);
}