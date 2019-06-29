#pragma once

#define NATIVE_CHECK_ENTITY_TYPE(entity, ent_type) if (!entity || !entity->user_data || entity->type != ent_type)
#define NATIVE_CHECK_ENTITY(entity) if (!entity || !entity->user_data)

extern "C" {
    
    //
    // General
    //

    OAKGEN_NATIVE();
    void oak_broadcast_msg_color(const char* text, u32 color) {
        modules::misc::broadcast_msg_color(text, color);
    }

    OAKGEN_NATIVE();
    void oak_broadcast_msg(const char* text) {
        oak_broadcast_msg_color(text, 0xFFFFFF);
    }

    OAKGEN_NATIVE();
    void oak_chat_print(const char* text) {
        modules::misc::chat_print(text);
    }

    OAKGEN_NATIVE();
    void oak_send_msg(const char* text, librg_entity *receiver) {
        modules::misc::send_msg(text, receiver);
    }


    //
    // Player
    //

    OAKGEN_NATIVE();
    void oak_player_fadeout(librg_entity *entity, bool fadeout, u32 duration, u32 color) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::fadeout(entity, fadeout, duration, color);
    }

    OAKGEN_NATIVE();
    void oak_player_inventory_add(librg_entity *entity, inventory_item *item) {
        modules::player::inventory_add(entity, item);
    }

    OAKGEN_NATIVE();
    void oak_player_spawn(librg_entity *entity, zpl_vec3 pos) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER){};

        entity->position = EXPAND_VEC(pos);
        modules::player::send_spawn(entity);
    }

    OAKGEN_NATIVE();
    void oak_player_set_model(librg_entity *entity, char *modelName) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        auto player = (mafia_player*)entity->user_data;

        if (player && modelName) {
            modules::player::set_model(entity, modelName);
        }
    }

    OAKGEN_NATIVE();
    void oak_player_set_position(librg_entity *entity, zpl_vec3 position) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};
        
        modules::player::set_pos(entity, position);
    }

    OAKGEN_NATIVE();
    void oak_player_set_health(librg_entity *entity, float health) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::set_health(entity, health);
    }

    OAKGEN_NATIVE();
    void oak_player_set_rotation(librg_entity *entity, zpl_vec3 rotation) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::set_rot(entity, rotation);
    }

    OAKGEN_NATIVE();
    void oak_player_set_camera(librg_entity *entity, zpl_vec3 pos, zpl_vec3 rot) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::set_camera(entity, pos, rot);
    }

    OAKGEN_NATIVE();
    void oak_player_unlock_camera(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::unlock_camera(entity);
    }

    OAKGEN_NATIVE();
    void oak_player_play_animation(librg_entity *entity, const char* text) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};

        modules::player::play_anim(entity, text);
    }

    OAKGEN_NATIVE();
    librg_entity* oak_player_get_vehicle(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return nullptr; };

        return modules::player::get_vehicle(entity);
    }

    OAKGEN_NATIVE();
    b32 oak_player_put_to_vehicle(librg_entity *entity, librg_entity *vehicle_ent, int seat_id) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return false; };
        NATIVE_CHECK_ENTITY_TYPE(vehicle_ent, TYPE_VEHICLE) { return false; };

        return modules::player::put_to_vehicle(entity, vehicle_ent, seat_id);
    }

    OAKGEN_NATIVE();
    b32 oak_player_die(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) { return false; };
        modules::player::die(entity, true);

        return true;
    }

    OAKGEN_NATIVE();
    void oak_player_show_on_map(librg_entity *entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};
        modules::player::set_map_vis(entity, state);
    }

    OAKGEN_NATIVE();
    void oak_player_show_nameplate(librg_entity* entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_PLAYER) {};
        modules::player::set_nameplate_vis(entity, state);
    }

    //
    // Weapon drop
    //

    OAKGEN_NATIVE();
    librg_entity* oak_drop_spawn(zpl_vec3 position, char *model, inventory_item item) {
        return modules::player::spawn_weapon_drop(position, model, item);
    }

    //
    // Vehicle 
    // 

    OAKGEN_NATIVE();
    librg_entity* oak_vehicle_spawn(zpl_vec3 position, zpl_vec3 rotation, char* model, b32 show_in_radar) {
        return modules::vehicle::spawn_vehicle(position, rotation, model, show_in_radar);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_destroy(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::destroy_vehicle(entity);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_show_on_radar(librg_entity *entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_radar_vis(entity, state);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_show_on_map(librg_entity *entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_map_vis(entity, state);
    }

    OAKGEN_NATIVE();
    int oak_vehicle_get_player_seat_id(librg_entity *entity, librg_entity *player) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) { return -1; };
        NATIVE_CHECK_ENTITY_TYPE(player, TYPE_PLAYER) { return -1; };

        return modules::vehicle::get_player_seat_id(entity, player);
    }
    
    OAKGEN_NATIVE();
    void oak_vehicle_set_position(librg_entity *entity, zpl_vec3 position) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_pos(entity, position);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_set_direction(librg_entity *entity, zpl_vec3 dir) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_dir(entity, dir);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_set_transparency(librg_entity *entity, f32 transparency) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_transparency(entity, transparency);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_set_collision_state(librg_entity *entity, b32 state) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_collision_state(entity, state);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_repair(librg_entity *entity) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::repair(entity);
    }

    OAKGEN_NATIVE();
    void oak_vehicle_set_fuel(librg_entity *entity, f32 fuel) {
        NATIVE_CHECK_ENTITY_TYPE(entity, TYPE_VEHICLE) {};
        modules::vehicle::set_fuel(entity, fuel);
    }
}

auto set_up_natives() -> void {
    auto vt = &gm.vtable;
    #include "natives.generated.hpp"
}
