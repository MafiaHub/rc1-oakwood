#pragma once

#define VEHICLE_SELECTION_TIME 2.0f

void vehicles_streamer_update() {
	zpl_local_persist f64 last_streamers_selection = 0.0f;

	if (zpl_time_now() - last_streamers_selection > VEHICLE_SELECTION_TIME) {
		last_streamers_selection = zpl_time_now();

		librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | TYPE_VEHICLE), [](librg_ctx *ctx, librg_entity *entity) {
			if (entity->user_data && entity->type & TYPE_VEHICLE) {
				auto vehicle = (mafia_vehicle*)entity->user_data;

				if (vehicle->seats[0] == -1) {
					auto streamer = mod_get_nearest_player(&network_context, entity->position);
					if (streamer != nullptr) {
						librg_entity_control_set(&network_context, entity->id, streamer->client_peer);
					}
					else {
						librg_entity_control_remove(&network_context, entity->id);
					}
				}
			}
		});
	}
}