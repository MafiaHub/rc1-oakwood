#pragma once

#ifdef _WIN32
std::string get_platform_path() {
    char temp_path_raw[MAX_PATH] = { '\0' };
    GetModuleFileName(GetModuleHandle(NULL), temp_path_raw, MAX_PATH);
    auto temp_path = std::string(temp_path_raw);
    auto temp_pos = temp_path.rfind("\\");
    return temp_path.erase(temp_pos, std::string::npos);
}
#endif

inline auto mod_get_nearest_player(librg_ctx* ctx, zpl_vec3 pos, i32 exception = -1) {
    f32 smallest_distance = 1000.0f;
    librg_entity* current_entity = nullptr;
    
    for (u32 i = 0; i < ctx->max_entities; i++) {   
        librg_entity *entity = librg_entity_fetch(ctx, i);
        if (!entity) continue;
        if (entity->type != TYPE_PLAYER || exception == entity->id) continue;

        zpl_vec3 final_vec;
        zpl_vec3_sub(&final_vec, entity->position, pos);
        float dist = zpl_vec3_mag(final_vec);
        if (dist < smallest_distance && dist < entity->stream_range) {
            smallest_distance = dist;
            current_entity = entity;
        }
    }

    return current_entity;
}

inline auto mod_vehicle_assign_nearest_player(librg_ctx *ctx, librg_entity *vehicle, i32 exception = -1) {
    auto streamer = mod_get_nearest_player(ctx, vehicle->position, exception);
    if (streamer != nullptr) {
        librg_entity_control_set(ctx, vehicle->id, streamer->client_peer);
    }
    else {
        librg_entity_control_remove(ctx, vehicle->id);
    }
}
