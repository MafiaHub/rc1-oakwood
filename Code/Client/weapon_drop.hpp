#pragma once
Vector4D drop_rotation;
float drop_angle = 0.0f;

//--------------------------[ LIBRG EVENTS ]--------------------------
inline auto drop_entitycreate(librg_event_t* evnt) -> void {
	auto drop = new mafia_weapon_drop;
	librg_data_rptr(evnt->data, drop->model, sizeof(char) * 32);
	librg_data_rptr(evnt->data, &drop->weapon, sizeof(inventory_item));

	auto player_frame = new MafiaSDK::I3D_Frame();
	Vector3D default_scale = { 1.5f, 1.5f, 1.5f };
	Vector3D default_pos = EXPAND_VEC(evnt->entity->position);
	player_frame->SetName("testing_drop");
	player_frame->LoadModel(drop->model);
	player_frame->SetScale(default_scale);
	player_frame->SetPos(default_pos);

	auto weapon_drop = new MafiaSDK::C_Drop_In_Weapon;
	weapon_drop->Init(player_frame);
	weapon_drop->GetInterface()->DropItem = { drop->weapon.weaponId, drop->weapon.ammoLoaded, drop->weapon.ammoHidden, NULL };
	drop->weapon_drop_actor = weapon_drop;
	MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(weapon_drop);
	evnt->entity->user_data = drop;
}

inline auto drop_entityremove(librg_event_t* evnt) -> void {
	auto drop = (mafia_weapon_drop *)evnt->entity->user_data;
	if (drop) {
		MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(drop->weapon_drop_actor);
	}
}
 
inline auto drop_frame_init(MafiaSDK::C_Human* _this, MafiaSDK::C_Drop_In_Weapon* drop, char* model_name) -> void {
	if (local_player.ped == _this) {
		auto item = (inventory_item*)&drop->GetInterface()->DropItem;
		local_player_weapondrop(item, model_name);
		MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(drop);
	}
}

inline auto drop_game_tick(mafia_weapon_drop* drop) -> void {
	drop_angle		+= 0.04f;
	drop_rotation	= { cos(drop_angle*0.1744f), 0.0, sin(drop_angle*0.1744f), 0.0 };
	MafiaSDK::I3D_Frame* drop_frame = *(MafiaSDK::I3D_Frame**)((DWORD)drop->weapon_drop_actor + 0x68);
	if(drop_frame) drop_frame->SetRot(drop_rotation);
}

inline auto drop_on_pickup(MafiaSDK::C_Drop_In_Weapon* drop) {

	for (u32 i = 0; i < ctx.max_entities; i++) {

		// skip local player 
		if (i == local_player.entity.id) continue;
		librg_entity_t *entity = librg_entity_fetch(&ctx, i);
		if (!entity || entity->type != TYPE_WEAPONDROP) continue;

		auto mafia_drop = (mafia_weapon_drop*)entity->user_data;
		if (mafia_drop->weapon_drop_actor == drop) {
			local_player_weaponpickup(entity);
		}
	}
}

namespace hooks
{
	DWORD jump_back = 0x00580180;
	DWORD other_jmp = 0x0057FB78;
	char model[32];
	DWORD jmp_back_capture = 0x0057FBA8;
	
	void copy_model(char* model_name) {
		sprintf(model, "%s", model_name);
	}

	__declspec(naked) void drop_capture_model() {
		__asm {
			lea     ebp, [eax + 24h]
			pushad
				push ebp
				call copy_model
				add esp, 0x4
			popad

			XOR eax, eax
			jmp jmp_back_capture
		}
	}

	__declspec(naked) void drop_in_weapon_init() {
		__asm
		{
			pushad
			    lea eax, model
				push eax
				push edi
				push ebp
				call drop_frame_init
				add esp, 0xC
			popad

			mov dword ptr ss : [esp + 0x20], eax 
			jmp jump_back
		}
	}

	__declspec(naked) void no_phys() { 
		__asm retn 4 
	}

	DWORD daco = 0x005A51A8;
	DWORD pickup_back = 0x594948;
	DWORD adddrcactor = 0x5A79A0;
	__declspec(naked) void weapon_pickup() {
		__asm {
			pushad
				push eax
				call drop_on_pickup
				add esp, 0x4
			popad

			push eax 
			mov eax, dword ptr ds : [0x63788C] 
			mov ecx, dword ptr ds : [eax + 0x24] 
			call adddrcactor
			jmp pickup_back
		}
	}
};

inline auto drop_init() -> void {
	MemoryPatcher::InstallJmpHook(0x0057FBA3, (DWORD)&hooks::drop_capture_model);
	MemoryPatcher::InstallJmpHook(0x00580176, (DWORD)&hooks::drop_in_weapon_init);
	MemoryPatcher::InstallJmpHook(0x00443650, (DWORD)&hooks::no_phys);
	MemoryPatcher::InstallJmpHook(0x00594923, (DWORD)&hooks::weapon_pickup);
}