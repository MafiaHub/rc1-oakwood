# Oakwood Modding API

The low-level modding API acts as a bridge between the server core and community-created plugins.

A plugin, in terms of Mafia Oakwood, is a DLL containing a code responsible for defining the gameplay logic.
The modding API is mostly suited for developers, who are capable of adding extensions to the server core and extend its functionality.

Such an example is Oakwood Framework, which uses the modding API and extends the core's feature list by providing easy-to-use wrappers and helpers.
Feel free to explore the API and do something creative!

## Basic types used within this guide

|      name      |              description              |
| -------------- | ------------------------------------- |
| u32            | unsigned 32-bit integer               |
| char*          | pointer to null-terminated char array |
| librg_entity   | server entity                         |
| bool           | boolean                               |
| inventory_item | item in an inventory (weapon, ...)    |
| zpl_vec3       | 3D vector                             |
| float          | 32-bit floating-point real number     |

## Natives

This is a list of all natives exposed to modders:

### General

#### oak_broadcast_msg_color(char* text, u32 color)
Prints a native Mafia message with a specified 32-bit packed color (RGBA format).

#### oak_broadcast_msg(char* text)
Prints a native Mafia message with a default color.

#### oak_chat_print(char* text)
Prints a message to all players to the chat.

#### oak_send_msg(char* text, librg_entity *receiver)
Prints a message for a specific player.

### Player

#### oak_player_fadeout(librg_entity* entity, bool fadeout, u32 duration, u32 color)
Player's screen will fade in or out (based on fadeout argument) with a specific duration and color. Method has to be called twice to perform a blink effect.

#### oak_player_inventory_add(librg_entity* entity, inventory_item* item)
Adds an item to the player's inventory.

#### oak_player_spawn(librg_entity* entity, zpl_vec3 pos)
Spawns a player on the map at a specific position. (Use ONLY on connection event!)

#### oak_player_respawn(librg_entity* entity, zpl_vec3 pos)
Respawns a player on the map at a specific position. (Used when player dies.)

#### oak_player_set_model(librg_entity* entity, char* modelName)
Sets player a character model.

#### oak_player_set_position(librg_entity* entity, zpl_vec3 pos)
Teleports player at a specific position.

#### oak_player_set_health(librg_entity* entity, float health)
Sets player a health (Note: HP is represented as a (base)x2, so to set 100 HP, you need to pass value of 200.0).

#### oak_player_rotation(librg_entity* entity, zpl_vec3 rotation)
Sets player's heading vector (yaw axis).

#### oak_player_set_camera(librg_entity* entity, zpl_vec3 pos, zpl_vec3 rot)
Locks player's camera at a specific position with specified heading vector (yaw axis).

#### oak_player_unlock_camera(librg_entity* entity)
Unlocks player's camera.

#### oak_player_play_animation(librg_entity* entity, char* name)
Plays an animation.

#### librg_entity* oak_player_get_vehicle(librg_entity* entity)
Retrieves vehicle player is sitting in or nullptr when not seated.

#### bool oak_player_put_to_vehicle(librg_entity* entity, librg_entity* vehicle_ent, int seat_id)
Puts player to a specific seat of a vehicle.

#### bool oak_player_die(librg_entity* entity)
Kills the player forcefully.

### Weapon drop

#### librg_entity* oak_drop_spawn(zpl_vec3 pos, char* model, inventory_item item)
Spawns a weapon drop.

### Vehicle

#### librg_entity* oak_vehicle_spawn(zpl_vec3 pos, zpl_vec3 rot, char* model, b32 show_in_radar)
Spawns a vehicle with a specified position, model and a heading vector. show_in_radar specifies whether the car should be visible on the radar.

#### oak_vehicle_show_on_radar(librg_entity* entity, bool state)
Toggles radar visibility on/off.

#### int oak_vehicle_get_player_seat_id(librg_entity* entity, librg_entity* player)
Returns a seated player's seat ID or -1 if not seated.

#### oak_vehicle_set_position(librg_entity* entity, zpl_vec3 pos)
Sets vehicle's position.

#### oak_vehicle_set_direction(librg_entity* entity, zpl_vec3 dir)
Sets vehicle's heading direction vector (yaw axis).


