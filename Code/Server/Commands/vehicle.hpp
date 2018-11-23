#pragma once

librg_entity *spawn_vehicle(zpl_vec3 position, zpl_vec3 rotation, char *model) {

    mafia_vehicle *vehicle = new mafia_vehicle();

    strncpy(vehicle->model, model, strlen(model));
    vehicle->rotation = rotation;
    vehicle->health             = 100.0f;
    vehicle->engine_health      = 100.0f;
    vehicle->fuel               = 60.0f;
    vehicle->sound_enabled      = 1;
    vehicle->rotation_second	= { 0.0f, 1.0f, 0.0f };

    auto new_vehicle_entity		    = librg_entity_create(&network_context, TYPE_VEHICLE);
    new_vehicle_entity->position	= position;
    new_vehicle_entity->user_data	= vehicle;

    auto streamer = mod_get_nearest_player(&network_context, new_vehicle_entity->position);
    if (streamer != nullptr) {
        librg_entity_control_set(&network_context, new_vehicle_entity->id, streamer->client_peer);
    }

    return new_vehicle_entity;
}