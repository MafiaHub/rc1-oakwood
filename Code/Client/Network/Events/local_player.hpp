#pragma once
struct local_player_data {
    local_player_data() : 
        dead(false)  {
    }

    u32 entity_id;
    zpl_vec3 pose;
    bool dead;
} local_player;

inline auto get_local_entity() {
    return librg_entity_fetch(&network_context, local_player.entity_id);
}

inline auto get_local_player() -> mafia_player* {
    auto local_ent = get_local_entity();
    if (local_ent) {
        return (mafia_player*)local_ent->user_data;
    }

    return nullptr;
}

inline auto get_local_ped() -> MafiaSDK::C_Player* {
    auto player = get_local_player();
    if (player) {
        return (MafiaSDK::C_Player*)player->ped;
    }
    return nullptr;
}

inline auto get_player_from_base(void* base) -> librg_entity* {

    for (u32 i = 0; i < network_context.max_entities; i++) {
        librg_entity *entity = librg_entity_fetch(&network_context, i);
        if (!entity || entity->type != TYPE_PLAYER || !entity->user_data) continue;	
        auto pl = (mafia_player*)(entity->user_data);
        if (base == pl->ped) return entity;
    }
    return nullptr;
}

inline auto get_vehicle_from_base(void* base) -> librg_entity* {

    for (u32 i = 0; i < network_context.max_entities; i++) {
        librg_entity *entity = librg_entity_fetch(&network_context, i);
        if (!entity || entity->type != TYPE_VEHICLE || !entity->user_data) continue;	
        auto pl = (mafia_vehicle*)(entity->user_data);
        if (base == pl->car) return entity;
    }
    return nullptr;
}

auto player_inventory_send() {
    player_inventory inv = {0};

    auto local_ped = get_local_ped();
    if (!local_ped) {
        mod_log("[INV SEND] Local player doesn't exist!");
        return;
    }

    memcpy(&inv, &((MafiaSDK::C_Human *)local_ped)->GetInterface()->inventory, sizeof(player_inventory));

    librg_send(&network_context, NETWORK_PLAYER_INVENTORY_SYNC, data, {
        librg_data_wptr(&data, &inv, sizeof(player_inventory));
    });
}


/* 
* todo add reason killer and so one ...
*/
inline auto local_player_died() {

    if (local_player.dead) return;

    if (!local_player.dead) 
        local_player.dead = true;
    
    auto player = get_local_player();
    if (player) {

        if (player->ped) {
            auto veh = player->ped->GetInterface()->playersCar;
            if (veh) {
                MafiaSDK::C_Player* current_player = (MafiaSDK::C_Player*)player->ped;
                current_player->LockControls(TRUE);
                player->ped->Intern_FromCar();
            }
        }
        //player->ped = nullptr;
    }

    librg_send(&network_context, NETWORK_PLAYER_DIE, data, {});
}


inline auto local_player_hit(
    MafiaSDK::C_Human* victim,
    DWORD hit_type, 
    const S_vector* unk1, 
    const S_vector* unk2, 
    const S_vector* unk3,
    float damage,
    MafiaSDK::C_Actor* attacker, 
    unsigned int player_part) -> void {

    auto attacker_ent = get_player_from_base(attacker);
    
    if (!victim || !attacker_ent) return;
    
    librg_send(&network_context, NETWORK_PLAYER_HIT, data, {
        librg_data_went(&data, attacker_ent->id);
        librg_data_wu32(&data, hit_type);
        librg_data_wptr(&data, (void*)unk1, sizeof(zpl_vec3));
        librg_data_wptr(&data, (void*)unk2, sizeof(zpl_vec3));
        librg_data_wptr(&data, (void*)unk3, sizeof(zpl_vec3));
        librg_data_wf32(&data, damage);
        librg_data_wf32(&data, victim->GetInterface()->health);
        librg_data_wu32(&data, player_part);
    });
}

inline auto local_player_shoot(S_vector pos) -> void {

    librg_send(&network_context, NETWORK_PLAYER_SHOOT, data, {
        librg_data_wptr(&data, &pos, sizeof(S_vector));
    });
}

inline auto local_player_weapondrop(inventory_item* item, char* model) -> void {

    char wep_model[32];
    strcpy(wep_model, model);

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_DROP, data, {
        librg_data_wptr(&data, item, sizeof(inventory_item));
        librg_data_wptr(&data, wep_model, sizeof(char) * 32);
    });

    player_inventory_send();
}

inline auto local_player_weaponchange(u32 index) -> void {

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, data, {
        librg_data_wu32(&data, index);
    });

    player_inventory_send();
}

inline auto local_player_fromcar() -> void {
    librg_send(&network_context, NETWORK_PLAYER_FROM_CAR, data, {});
}

//TODO send inventory on each message related with weapons !
inline auto local_player_reload() -> void {
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, data, {});

    player_inventory_send();	
}

inline auto local_player_holster() -> void {
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, data, {});

    player_inventory_send();
}

inline auto local_player_weaponpickup(librg_entity* item_entity) -> void {

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, data, {
        librg_data_went(&data, item_entity->id);
    });

    //force weapon change for inventory sync (weird rules wen wep is picked up, some weapons have priority and sheet)
    auto local_ped = get_local_ped();
    if (local_ped) {
        auto mafia_drop = (mafia_weapon_drop*)item_entity->user_data;
        local_ped->G_Inventory_SelectByID(mafia_drop->weapon.weaponId);
        local_ped->Do_ChangeWeapon(0, 0);
        player_inventory_send();
    }
}

inline auto local_player_throwgrenade(const S_vector & pos) {
    S_vector vec_copy = pos;
    librg_send(&network_context, NETWORK_PLAYER_THROW_GRENADE, data, {
        librg_data_wptr(&data, &vec_copy, sizeof(S_vector));
    });

    player_inventory_send();
}

inline auto local_player_useactor(DWORD actor, int action, int seat_id, int unk3) {

    auto vehicle_ent = get_vehicle_from_base((void*)actor);
    if (!vehicle_ent) return;

    librg_send(&network_context, NETWORK_PLAYER_USE_ACTOR, data, {
        librg_data_wu32(&data, vehicle_ent->id);
        librg_data_wi32(&data, action);
        librg_data_wi32(&data, seat_id);
        librg_data_wi32(&data, unk3);
    });
}

inline auto local_player_use_door(MafiaSDK::C_Door* door, MafiaSDK::C_Door_Enum::States state) {
    
    if(!door || !librg_is_connected(&network_context)) return;
    
    auto door_int = door->GetInterface();
    if (door_int && door_int->entity.frame) {

        auto door_frame_name = door_int->entity.frame->GetInterface()->name;
        auto door_name_len = strlen(door_frame_name);
        if (door_name_len) {
            librg_send(&network_context, NETWORK_PLAYER_USE_DOORS, data, {
                librg_data_wu32(&data, door_name_len);
                librg_data_wptr(&data, door_frame_name, door_name_len);
                librg_data_wu32(&data, state);
            });
        }
    }
}

inline auto local_player_hijack(DWORD car, int seat) {

    auto vehicle_ent = get_vehicle_from_base((void*)car);
    if (!vehicle_ent) return;

    librg_send(&network_context, NETWORK_PLAYER_HIJACK, data, {
        librg_data_wu32(&data, vehicle_ent->id);
        librg_data_wi32(&data, seat);
    });
}

inline auto local_player_remove_temporary_actor(void* base) {

    auto player_ent = get_player_from_base(base);
    if (player_ent) {
        auto player = (mafia_player*)player_ent->user_data;
        if (player && player->ped) {
            printf("Deallocate player '%s'\n", player->name);
            delete player;
            player_ent->user_data = nullptr;
        }
    }

    auto vehicle_ent = get_vehicle_from_base(base);
    if (vehicle_ent) {
        auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
        if (vehicle) {
            if (!(vehicle->clientside_flags & CLIENTSIDE_VEHICLE_STREAMER_REMOVED)) {
                librg_send(&network_context, NETWORK_VEHICLE_GAME_DESTROY, data, {
                    librg_data_wu32(&data, vehicle_ent->id);
                });
            }
            printf("Deallocate vehicle '%d'\n", vehicle_ent->id);
            delete vehicle;
            vehicle_ent->user_data = nullptr;
        }
    }
}

inline auto local_player_car_destruct(void* base) {
}

#include "Game/Hooks/local_player.hpp"
