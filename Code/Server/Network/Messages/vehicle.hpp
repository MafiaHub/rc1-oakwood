librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [](librg_message* msg) {

	zpl_vec3 speed, unk;
	auto vehicle_id = librg_data_ru32(msg->data);
	auto wheel_idx = librg_data_ru32(msg->data);
	librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
	librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

	auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

	if (vehicle_ent && vehicle_ent->user_data) {

		auto sender_ent = librg_entity_find(&network_context, msg->peer);
		auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);
		if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

			auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;
			sender_vehicle->tyres[wheel_idx].health = 0.0f;

			mod_message_send(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [&](librg_data *data) {
				librg_data_wu32(data, vehicle_ent->id);
				librg_data_wu32(data, wheel_idx);
				librg_data_wptr(data, (void*)&speed, sizeof(zpl_vec3));
				librg_data_wptr(data, (void*)&unk, sizeof(zpl_vec3));
			});
		}
	}	
});