#ifndef OAK_SERVER_H
#define OAK_SERVER_H

#include <stdint.h>

/* GENERAL */

#define OAK_API
#define OAK_API_TYPE(a, b)

#define oak_array(type) type *
#define oak_ref(type) type *

OAK_API_TYPE(float, float)
OAK_API_TYPE(oak_string, str)
OAK_API_TYPE(oak_vec3, vec3)
OAK_API_TYPE(oak_vec2, vec2)
OAK_API_TYPE(oak_ref(int), oak_ref(int))
OAK_API_TYPE(oak_array(oak_player), oak_array(int))
OAK_API_TYPE(oak_array(oak_dialog), oak_array(int))
OAK_API_TYPE(oak_array(oak_vehicle), oak_array(int))
OAK_API_TYPE(oak_array(oak_door), oak_array(int))

typedef union {
    struct { float x, y, z; };
    struct { int r, g, b; };
    float e[3];
} oak_vec3;

typedef uintptr oak_player;
typedef uintptr oak_dialog;
typedef uintptr oak_vehicle;
typedef uintptr oak_door;
typedef uintptr oak_npc;

typedef const char * oak_string;

typedef int oak_visiblity_type;
enum {
    OAK_VISIBILITY_NAME,
    OAK_VISIBILITY_ICON,
    OAK_VISIBILITY_RADAR,
    OAK_VISIBILITY_MODEL,
    OAK_VISIBILITY_COLLISION,
};

typedef int oak_seat_id;
enum {
    OAK_SEAT_FRONT_LEFT = 0,
    OAK_SEAT_FRONT_RIGHT,
    OAK_SEAT_REAR_LEFT,
    OAK_SEAT_REAR_RIGHT,
    OAK_MAX_SEATS,
};

OAK_API int oak_logn(oak_string, int);

/* MISC */

OAK_API int   oak_killbox_set(float);
OAK_API float oak_killbox_get();

/* ENTITY LISTS */

OAK_API oak_array(oak_player) oak_player_list(oak_ref(int));
OAK_API oak_array(oak_vehicle) oak_vehicle_list(oak_ref(int));
OAK_API oak_array(oak_door) oak_door_list(oak_ref(int));

/* PLAYERS */

OAK_API int oak_player_spawn(oak_player, oak_vec3, float);
OAK_API int oak_player_despawn(oak_player);

OAK_API int oak_player_invalid(oak_player);
OAK_API int oak_player_kick(oak_player, oak_string, int);
OAK_API int oak_player_kill(oak_player);
OAK_API int oak_player_playanim(oak_player, oak_string, int);

OAK_API int oak_player_model_set(oak_player, oak_string, int);
OAK_API int oak_player_health_set(oak_player, float);
OAK_API int oak_player_position_set(oak_player, oak_vec3);
OAK_API int oak_player_direction_set(oak_player, oak_vec3);
OAK_API int oak_player_heading_set(oak_player, float);

OAK_API int oak_player_give_weapon(oak_player, int, int, int);
OAK_API int oak_player_remove_weapon(oak_player, int);

OAK_API oak_string oak_player_name_get(oak_player);
OAK_API oak_string oak_player_model_get(oak_player);
OAK_API float oak_player_health_get(oak_player);
OAK_API float oak_player_heading_get(oak_player);
OAK_API oak_vec3 oak_player_position_get(oak_player);
OAK_API oak_vec3 oak_player_direction_get(oak_player);

OAK_API oak_string oak_player_ip_get(oak_player id);
OAK_API oak_string oak_player_statecode_get(oak_player id);
OAK_API oak_string oak_player_statename_get(oak_player id);

/* PLAYER VISIBILITY */

OAK_API int oak_player_visibility_set(oak_player, oak_visiblity_type, int);
OAK_API int oak_player_visibility_get(oak_player, oak_visiblity_type);

/* CAMERA */

OAK_API int oak_camera_set(oak_player, oak_vec3, oak_vec3);
OAK_API int oak_camera_unlock(oak_player);
OAK_API int oak_camera_target_player(oak_player, oak_player);
OAK_API int oak_camera_target_vehicle(oak_player, oak_vehicle);
OAK_API int oak_camera_target_unset(oak_player);

/* HUD */
OAK_API int oak_hud_fadeout(oak_player, int, int, int);
OAK_API int oak_hud_countdown(oak_player, int);
OAK_API int oak_hud_announce(oak_player, oak_string, int, float);
OAK_API int oak_hud_message(oak_player, oak_string, int, int);
OAK_API int oak_dialog_show(oak_player, oak_string, int, oak_string, int, oak_string, int, oak_string, int, oak_dialog, int);

/* CHAT */

OAK_API int oak_chat_send(oak_player, oak_string, int);
OAK_API int oak_chat_broadcast(oak_string, int);

/* VEHICLES */

OAK_API oak_vehicle oak_vehicle_spawn(oak_string, int, oak_vec3, float);
OAK_API int oak_vehicle_despawn(oak_vehicle);

OAK_API int oak_vehicle_invalid(oak_vehicle);
OAK_API int oak_vehicle_repair(oak_vehicle);

OAK_API int oak_vehicle_position_set(oak_vehicle, oak_vec3);
OAK_API int oak_vehicle_direction_set(oak_vehicle, oak_vec3);
OAK_API int oak_vehicle_heading_set(oak_vehicle, float);
OAK_API int oak_vehicle_fuel_set(oak_vehicle, float);
OAK_API int oak_vehicle_transparency_set(oak_vehicle, float);
OAK_API int oak_vehicle_lock_set(oak_vehicle, int);

OAK_API oak_vec3 oak_vehicle_position_get(oak_vehicle);
OAK_API oak_vec3 oak_vehicle_direction_get(oak_vehicle);
OAK_API float oak_vehicle_heading_get(oak_vehicle);
OAK_API float oak_vehicle_fuel_get(oak_vehicle);
OAK_API float oak_vehicle_transparency_get(oak_vehicle);
OAK_API int oak_vehicle_lock_get(oak_vehicle id);

/* VEHICLE VISIBILITY */

OAK_API int oak_vehicle_visibility_set(oak_vehicle, oak_visiblity_type, int);
OAK_API int oak_vehicle_visibility_get(oak_vehicle, oak_visiblity_type);

/* VEHICLE/PLAYER INTERACTION */

OAK_API int oak_vehicle_player_put(oak_vehicle, oak_player, oak_seat_id);
OAK_API int oak_vehicle_player_remove(oak_vehicle, oak_player);
OAK_API oak_vehicle oak_vehicle_player_inside(oak_player);
OAK_API oak_seat_id oak_vehicle_player_seat_get(oak_vehicle, oak_player);
OAK_API oak_player oak_vehicle_player_at_seat(oak_vehicle, oak_seat_id);
OAK_API oak_array(oak_player) oak_vehicle_player_list(oak_vehicle, oak_ref(int));

/* DOORS */

OAK_API oak_door oak_door_create(oak_string, int);
OAK_API int oak_door_destroy(oak_door);
OAK_API int oak_door_invalid(oak_door);
OAK_API oak_string oak_door_name_get(oak_door);

#endif // OAK_SERVER_H
