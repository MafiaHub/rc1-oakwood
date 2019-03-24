#pragma once

extern auto get_local_player() -> mafia_player*;

auto spawn(zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId,
                  bool is_in_car) -> MafiaSDK::C_Player *{
    
    S_vector default_scale = { 1.0f, 1.0f, 1.0f };
    S_vector default_pos = EXPAND_VEC(position);

    auto player_model = (MafiaSDK::I3D_Model*)MafiaSDK::I3DGetDriver()->CreateFrame(MafiaSDK::I3D_Driver_Enum::FrameType::MODEL);
    while(MafiaSDK::GetModelCache()->Open(player_model, model, NULL, NULL, NULL, NULL))  {
        printf("Error: Unable to create player model <%s> !\n", model);
    }

    player_model->SetName("testing_player");
    player_model->SetScale(default_scale);
    player_model->SetWorldPos(default_pos);
    player_model->Update();

    MafiaSDK::C_Player *new_ped = nullptr;

    if (is_local_player)
        new_ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Player));
    else
        new_ped = reinterpret_cast<MafiaSDK::C_Player*>(MafiaSDK::GetMission()->CreateActor(MafiaSDK::C_Mission_Enum::ObjectTypes::Enemy));

    new_ped->Init(player_model);

    if (!is_local_player)
        new_ped->SetBehavior(MafiaSDK::C_Human_Enum::BehaviorStates::DoesntReactOnWeapon);

    new_ped->SetShooting(1.0f);
    new_ped->SetActive(1);
    MafiaSDK::GetMission()->GetGame()->AddTemporaryActor(new_ped);

    if (is_local_player) { 
        auto game = MafiaSDK::GetMission()->GetGame();
        if (game) {
            game->GetCamera()->SetCar(NULL);
            game->GetCamera()->SetMode(true, 1);
            game->GetCamera()->SetPlayer(new_ped);
            game->SetLocalPlayer(new_ped);
        }

        MafiaSDK::GetIndicators()->PlayerSetWingmanLives(100);

        auto player = get_local_player();
        if (player) {
            local_player.dead = false;
            if (player->ped) {
                despawn(player->ped);
            }

            player->ped = new_ped;
            strcpy(player->name, GlobalConfig.username);
        }
    }

    new_ped->GetInterface()->humanObject.health = health;

    if (!is_in_car) {
        new_ped->GetInterface()->humanObject.entity.position = default_pos;
        new_ped->GetInterface()->humanObject.entity.rotation = EXPAND_VEC(rotation);
    }

    //Foreach every weapon in inventory and give it to the player
    for (size_t i = 0; i < 8; i++) {
        S_GameItem* item = (S_GameItem*)&inventory.items[i];
        if (item->weaponId != expectedWeaponId) {
            new_ped->G_Inventory_AddItem(*item);
        }
    }
    
    //TODO(DavoSK): Make it more fancy !
    //Select right weapon
    modules::player::select_by_id_original((void *)new_ped->GetInventory(), current_wep, nullptr);
    new_ped->Do_ChangeWeapon(0, 0);
    new_ped->ChangeWeaponModel();

    //If player have hands do holster
    if (current_wep == 0)
        new_ped->Do_Holster();

    return new_ped;
}

auto despawn(MafiaSDK::C_Human* player) -> void {
    if (player) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player);
        /*DWORD frame = *(DWORD*)(player + 0x68);
        if (frame) {
            __asm {
                mov eax, frame
                push eax
                mov ecx, [eax]
                call dword ptr ds : [ecx]
            }
        }*/
    }
}

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

auto inventory_send() -> void {
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
inline auto died() -> void {

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


inline auto hit(
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

inline auto shoot(shoot_data_t data_shot) -> void {

    librg_send(&network_context, NETWORK_PLAYER_SHOOT, data, {
        librg_data_wptr(&data, &data_shot.pos, sizeof(S_vector));
        librg_data_wptr(&data, &data_shot.dir, sizeof(S_vector));
        librg_data_wptr(&data, &data_shot.screen_coord, sizeof(S_vector));
    });
}

inline auto weapondrop(inventory_item* item, char* model) -> void {

    char wep_model[32];
    strcpy(wep_model, model);

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_DROP, data, {
        librg_data_wptr(&data, item, sizeof(inventory_item));
        librg_data_wptr(&data, wep_model, sizeof(char) * 32);
    });

    inventory_send();
}

inline auto weaponchange(u32 index) -> void {

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_CHANGE, data, {
        librg_data_wu32(&data, index);
    });

    inventory_send();
}

inline auto fromcar() -> void {
    librg_send(&network_context, NETWORK_PLAYER_FROM_CAR, data, {});
}

//TODO send inventory on each message related with weapons !
inline auto reload() -> void {
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_RELOAD, data, {});

    inventory_send();	
}

inline auto holster() -> void {
    librg_send(&network_context, NETWORK_PLAYER_WEAPON_HOLSTER, data, {});

    inventory_send();
}

inline auto weaponpickup(librg_entity* item_entity) -> void {

    librg_send(&network_context, NETWORK_PLAYER_WEAPON_PICKUP, data, {
        librg_data_went(&data, item_entity->id);
    });

    //force weapon change for inventory sync (weird rules wen wep is picked up, some weapons have priority and sheet)
    auto local_ped = get_local_ped();
    if (local_ped) {
        auto mafia_drop = (mafia_weapon_drop*)item_entity->user_data;
        local_ped->G_Inventory_SelectByID(mafia_drop->weapon.weaponId);
        local_ped->Do_ChangeWeapon(0, 0);
        inventory_send();
    }
}

inline auto throwgrenade(const S_vector & pos) -> void {
    S_vector vec_copy = pos;
    librg_send(&network_context, NETWORK_PLAYER_THROW_GRENADE, data, {
        librg_data_wptr(&data, &vec_copy, sizeof(S_vector));
    });

    inventory_send();
}

inline auto useactor(DWORD actor, int action, int seat_id, int unk3) -> void {

    auto vehicle_ent = get_vehicle_from_base((void*)actor);
    if (!vehicle_ent) return;

    librg_send(&network_context, NETWORK_PLAYER_USE_ACTOR, data, {
        librg_data_wu32(&data, vehicle_ent->id);
        librg_data_wi32(&data, action);
        librg_data_wi32(&data, seat_id);
        librg_data_wi32(&data, unk3);
    });
}

inline void use_door(MafiaSDK::C_Door* door, MafiaSDK::C_Door_Enum::States state) {
    
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

inline auto hijack(DWORD car, int seat) -> void {

    auto vehicle_ent = get_vehicle_from_base((void*)car);
    if (!vehicle_ent) return;

    librg_send(&network_context, NETWORK_PLAYER_HIJACK, data, {
        librg_data_wu32(&data, vehicle_ent->id);
        librg_data_wi32(&data, seat);
    });
}

inline auto remove_temporary_actor(void* base) -> void {

    auto player_ent = get_player_from_base(base);
    if (player_ent) {
        auto player = (mafia_player*)player_ent->user_data;
        if (player && player->ped) {
            player->ped = nullptr;
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
            vehicle->car = nullptr;
        }
    }
}

inline auto car_destruct(void* base) -> void {
}
