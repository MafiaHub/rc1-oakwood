#pragma once
#define EXPAND_VEC(VEC) {VEC.x, VEC.y, VEC.z}
#define EXPLODE_VEC(VEC) VEC.x, VEC.y, VEC.z

inline auto mod_log(const char* msg) -> void {
    printf("[Oakwood MP] %s\n", msg);
}

inline auto mod_debug(const char* msg) -> void {
    printf("[DEBUG] %s\n", msg);
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
        if (dist < smallest_distance) {
            smallest_distance = dist;
            current_entity = entity;
        }
    }

    return current_entity;
}

inline auto mod_file_exist(std::string file_name) {
    std::ifstream infile(file_name);
    return infile.good();
}
