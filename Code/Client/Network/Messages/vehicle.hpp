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