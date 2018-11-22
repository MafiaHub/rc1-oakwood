librg_network_add(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, [](librg_message* msg) {

	u32 vehicle_id = librg_data_ru32(msg->data);
	u32 component_idx = librg_data_ru32(msg->data);
	Vector3D speed, unk;

	librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
	librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

	auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

	if (vehicle_ent && vehicle_ent->user_data) {
		auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
		if (vehicle->car) {
			vehicle->destroyed_components[component_idx] = 1;
			hooks::car_prepare_dropout_original(vehicle->car, component_idx, speed, &unk);
		}
	}
});

librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [](librg_message* msg) {

	u32 vehicle_id = librg_data_ru32(msg->data);
	u32 wheel_idx = librg_data_ru32(msg->data);
	Vector3D speed, unk;

	librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
	librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));
	
	auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

	if (vehicle_ent && vehicle_ent->user_data) {
		auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
		if (vehicle->car) {
			vehicle->tyres[wheel_idx].health = 0.0f;
			hooks::car_prepare_dropout_wheel_original(vehicle->car, wheel_idx, speed, &unk);
		}
	}
});

librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_UPDATE, [](librg_message* msg) {

	u32 vehicle_id = librg_data_ru32(msg->data);
	u32 tyre_idx = librg_data_ru32(msg->data);
	u32 tyre_flags = librg_data_ru32(msg->data);
	float tyre_health = librg_data_rf32(msg->data);

	auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

	if (vehicle_ent && vehicle_ent->user_data) {
		auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
		if (vehicle->car) {
			vehicle->tyres[tyre_idx].health	= tyre_health;
			vehicle->tyres[tyre_idx].flags		= tyre_flags;
			
			auto vehicle_tyre = vehicle->car->GetCarTyre(tyre_idx);
			if (vehicle_tyre) {
				*(float*)((DWORD)vehicle_tyre + 0x18C) = tyre_health;
				*(DWORD*)(vehicle_tyre + 0x120) = tyre_flags;
			}
		}
	}
});

librg_network_add(&network_context, NETWORK_VEHICLE_EXPLODE, [](librg_message* msg) {

	u32 vehicle_id = librg_data_ru32(msg->data);
	auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

	if (vehicle_ent && vehicle_ent->user_data) {

		auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
		vehicle->wants_explode = false;

		if (vehicle->car) {
			hooks::c_car_carexplosion_original(vehicle->car, 200);
		}
	}
});