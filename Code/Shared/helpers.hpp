#pragma once
#define EXPAND_VEC(VEC) {VEC.x, VEC.y, VEC.z}
#define EXPLODE_VEC(VEC) VEC.x, VEC.y, VEC.z

#ifdef _WIN32
std::string get_platform_path() {
    char temp_path_raw[MAX_PATH] = { '\0' };
    GetModuleFileName(GetModuleHandle(NULL), temp_path_raw, MAX_PATH);
    auto temp_path = std::string(temp_path_raw);
    auto temp_pos = temp_path.rfind("\\");
    return temp_path.erase(temp_pos, std::string::npos);
}
#endif

inline auto mod_log(const char* msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[Oakwood MP] %s\n", msg);
#else
    printf("[Oakwood MP] %s\n", msg);
#endif
}

inline auto mod_log(std::string msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[Oakwood MP] %s\n", msg.c_str());
#else
    printf("[Oakwood MP] %s\n", msg.c_str());
#endif
}

inline auto mod_debug(const char* msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[DEBUG] %s\n", msg);
#else
    printf("[DEBUG] %s\n", msg);
#endif
}

inline auto mod_get_file_content(std::string file_name) {
    std::ifstream ifs(file_name);
    std::string content((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));

    return content;
}

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

inline auto mod_file_exist(std::string file_name) {
    std::ifstream infile(file_name);
    return infile.good();
}
