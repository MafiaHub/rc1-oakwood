#define OAK_BROADCAST_MSG_COLOR(name) void name(const char* text, u32 color)
typedef OAK_BROADCAST_MSG_COLOR(oak_broadcast_msg_color_ptr);

#define OAK_BROADCAST_MSG(name) void name(const char* text)
typedef OAK_BROADCAST_MSG(oak_broadcast_msg_ptr);

#define OAK_CHAT_PRINT(name) void name(const char* text)
typedef OAK_CHAT_PRINT(oak_chat_print_ptr);

#define OAK_SEND_MSG(name) void name(const char* text, librg_entity *receiver)
typedef OAK_SEND_MSG(oak_send_msg_ptr);

#define OAK_PLAYER_FADEOUT(name) void name(librg_entity *entity, bool fadeout, u32 duration, u32 color)
typedef OAK_PLAYER_FADEOUT(oak_player_fadeout_ptr);

#define OAK_PLAYER_INVENTORY_ADD(name) void name(librg_entity *entity, inventory_item *item)
typedef OAK_PLAYER_INVENTORY_ADD(oak_player_inventory_add_ptr);

#define OAK_PLAYER_SPAWN(name) void name(librg_entity *entity, zpl_vec3 pos)
typedef OAK_PLAYER_SPAWN(oak_player_spawn_ptr);

#define OAK_PLAYER_SET_MODEL(name) void name(librg_entity *entity, char *modelName)
typedef OAK_PLAYER_SET_MODEL(oak_player_set_model_ptr);

#define OAK_PLAYER_SET_POSITION(name) void name(librg_entity *entity, zpl_vec3 position)
typedef OAK_PLAYER_SET_POSITION(oak_player_set_position_ptr);

#define OAK_PLAYER_SET_HEALTH(name) void name(librg_entity *entity, float health)
typedef OAK_PLAYER_SET_HEALTH(oak_player_set_health_ptr);

#define OAK_PLAYER_SET_ROTATION(name) void name(librg_entity *entity, zpl_vec3 rotation)
typedef OAK_PLAYER_SET_ROTATION(oak_player_set_rotation_ptr);

#define OAK_PLAYER_SET_CAMERA(name) void name(librg_entity *entity, zpl_vec3 pos, zpl_vec3 rot)
typedef OAK_PLAYER_SET_CAMERA(oak_player_set_camera_ptr);

#define OAK_PLAYER_UNLOCK_CAMERA(name) void name(librg_entity *entity)
typedef OAK_PLAYER_UNLOCK_CAMERA(oak_player_unlock_camera_ptr);

#define OAK_PLAYER_PLAY_ANIMATION(name) void name(librg_entity *entity, const char* text)
typedef OAK_PLAYER_PLAY_ANIMATION(oak_player_play_animation_ptr);

#define OAK_PLAYER_GET_VEHICLE(name) librg_entity* name(librg_entity *entity)
typedef OAK_PLAYER_GET_VEHICLE(oak_player_get_vehicle_ptr);

#define OAK_PLAYER_PUT_TO_VEHICLE(name) b32 name(librg_entity *entity, librg_entity *vehicle_ent, int seat_id)
typedef OAK_PLAYER_PUT_TO_VEHICLE(oak_player_put_to_vehicle_ptr);

#define OAK_PLAYER_DIE(name) b32 name(librg_entity *entity)
typedef OAK_PLAYER_DIE(oak_player_die_ptr);

#define OAK_DROP_SPAWN(name) librg_entity* name(zpl_vec3 position, char *model, inventory_item item)
typedef OAK_DROP_SPAWN(oak_drop_spawn_ptr);

#define OAK_VEHICLE_SPAWN(name) librg_entity* name(zpl_vec3 position, zpl_vec3 rotation, char* model, b32 show_in_radar)
typedef OAK_VEHICLE_SPAWN(oak_vehicle_spawn_ptr);

#define OAK_VEHICLE_SHOW_ON_RADAR(name) void name(librg_entity *entity, b32 state)
typedef OAK_VEHICLE_SHOW_ON_RADAR(oak_vehicle_show_on_radar_ptr);

#define OAK_VEHICLE_GET_PLAYER_SEAT_ID(name) int name(librg_entity *entity, librg_entity *player)
typedef OAK_VEHICLE_GET_PLAYER_SEAT_ID(oak_vehicle_get_player_seat_id_ptr);

#define OAK_VEHICLE_SET_POSITION(name) void name(librg_entity *entity, zpl_vec3 position)
typedef OAK_VEHICLE_SET_POSITION(oak_vehicle_set_position_ptr);

#define OAK_VEHICLE_SET_DIRECTION(name) void name(librg_entity *entity, zpl_vec3 dir)
typedef OAK_VEHICLE_SET_DIRECTION(oak_vehicle_set_direction_ptr);

