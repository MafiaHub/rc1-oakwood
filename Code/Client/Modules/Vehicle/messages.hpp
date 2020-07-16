void add_messages() {
    librg_network_add(&network_context, NETWORK_VEHICLE_PLAYER_DISCONNECT, [](librg_message * msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            if (vehicle->car) {
                vehicle->car->SetEngineOn(false, false);
                vehicle->car->SetGear(0);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, [](librg_message* msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        u32 component_idx = librg_data_ru32(msg->data);
        S_vector speed, unk;

        librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
        librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            if (vehicle->car) {
                vehicle->destroyed_components[component_idx] = 1;
                car_prepare_dropout_original(vehicle->car, component_idx, speed, &unk);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [](librg_message* msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        u32 wheel_idx = librg_data_ru32(msg->data);
        S_vector speed, unk;

        librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
        librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            if (vehicle->car) {
                vehicle->tyres[wheel_idx].health = 0.0f;
                car_prepare_dropout_wheel_original(vehicle->car, wheel_idx, speed, &unk);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_UPDATE, [](librg_message* msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        u32 tyre_idx = librg_data_ru32(msg->data);
        u32 tyre_flags = librg_data_ru32(msg->data);
        float tyre_health = librg_data_rf32(msg->data);

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            if (vehicle->car) {
                vehicle->tyres[tyre_idx].health = tyre_health;
                vehicle->tyres[tyre_idx].flags = tyre_flags;

                auto vehicle_tyre = vehicle->car->GetCarTyre(tyre_idx);
                if (vehicle_tyre) {
                    *(float*)((DWORD)vehicle_tyre + 0x18C) = tyre_health;
                    *(DWORD*)(vehicle_tyre + 0x120) |= tyre_flags;
                }
            }
        }
    });

   librg_network_add(&network_context, NETWORK_VEHICLE_GAME_DESTROY, [](librg_message * msg) {
       u32 vehicle_id = librg_data_ru32(msg->data);
       printf("Game destroy vehicle: %d\n", vehicle_id);
#ifdef OAK_FEATURE_VEHICLE_CACHE
        auto cached_car = car_cache[vehicle_id];
        if (cached_car) {
            if (std::find(car_delte_queue.begin(), car_delte_queue.end(), cached_car) == car_delte_queue.end()) {
                car_delte_queue.push_back(cached_car);
                car_cache[vehicle_id] = nullptr;
            }
        }
#else
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            car_delte_queue.push_back(vehicle->car);
        }
#endif
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_EXPLODE, [](librg_message* msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
          
            if (vehicle->car) {
                vehicle->wants_explode = false;

                c_car_carexplosion_original(vehicle->car, 200);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_DEFORM_DELTA, [](librg_message* msg) {

        u32 vehicle_id = librg_data_ru32(msg->data);
        u32 deltas_count = librg_data_ru32(msg->data);
        std::vector<mafia_vehicle_deform> deltas_recv;

        for (u32 i = 0; i < deltas_count; i++) {
            mafia_vehicle_deform delta;
            librg_data_rptr(msg->data, &delta, sizeof(mafia_vehicle_deform));
            deltas_recv.push_back(delta);
        }

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);
        if (vehicle_ent && vehicle_ent->user_data) {

            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            if (vehicle->car) {

                for (auto delta : deltas_recv) {
                    auto mesh = vehicle->car->GetMeshByIndex(delta.mesh_index);
                    if (mesh) {
                        auto mesh_lod = mesh->GetLOD(0);
                        if (mesh_lod) {
                            MafiaSDK::I3D_stats_mesh stats;
                            auto vertices = mesh_lod->LockVertices(0);
                            if (vertices) {
                                mesh_lod->GetStats(stats);
                                if (delta.vertex_index < stats.vertex_count) {
                                    vertices[delta.vertex_index].n = EXPAND_VEC(delta.normal);
                                    vertices[delta.vertex_index].p = EXPAND_VEC(delta.position);
                                }
                                mesh_lod->UnlockVertices();
                            }
                        }
                    }
                }
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_RADAR_VISIBILITY, [](librg_message* msg) {
        u32 vehicle_id = librg_data_rent(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

            b32 state = librg_data_ru8(msg->data);

            if (state != vehicle->is_car_in_radar) {
                if (state)
                    MafiaSDK::GetIndicators()->RadarAddCar(vehicle->car, 0xFFFF0000);
                else
                    MafiaSDK::GetIndicators()->RadarRemoveCar(vehicle->car);

                vehicle->is_car_in_radar = state;
            }
        }
    });

   librg_network_add(&network_context, NETWORK_VEHICLE_MAP_VISIBILITY, [](librg_message* msg) {
        u32 vehicle_id = librg_data_rent(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {
            auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
            vehicle->is_visible_on_map = librg_data_ru8(msg->data);
        }
    });

   librg_network_add(&network_context, NETWORK_VEHICLE_SET_FUEL, [](librg_message* msg) {
       u32 vehicle_id = librg_data_rent(msg->data);
       auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

       if (vehicle_ent && vehicle_ent->user_data) {
           auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;
           vehicle->fuel = librg_data_rf32(msg->data);
           vehicle->car->GetInterface()->vehicle_interface.fuel = vehicle->fuel;
       }
   });

    librg_network_add(&network_context, NETWORK_VEHICLE_SET_POS, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity) {
            librg_data_rptr(msg->data, &entity->position, sizeof(entity->position));

            auto vehicle = (mafia_vehicle*)entity->user_data;

            if (vehicle) {
                lib_inter_reset(vehicle->interp.pos, EXPAND_VEC(entity->position));
                vehicle->car->GetInterface()->vehicle_interface.position = EXPAND_VEC(entity->position);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_SET_VEL, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity) {
            zpl_vec3 velocity;

            librg_data_rptr(msg->data, &velocity, sizeof(zpl_vec3));

            auto vehicle = (mafia_vehicle*)entity->user_data;

            if (vehicle) {
                vehicle->car->GetInterface()->vehicle_interface.speed = EXPAND_VEC(velocity);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_SET_DIR, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity && entity->user_data) {
            auto vehicle = (mafia_vehicle*)entity->user_data;

            librg_data_rptr(msg->data, &vehicle->rot_forward, sizeof(zpl_vec3));
            lib_inter_reset(vehicle->interp.rot, EXPAND_VEC(vehicle->rot_forward));
            vehicle->car->GetInterface()->vehicle_interface.rot_forward = EXPAND_VEC(vehicle->rot_forward);
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_SET_TRANSPARENCY, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity && entity->user_data) {
            auto vehicle = (mafia_vehicle*)entity->user_data;

            vehicle->transparency = librg_data_rf32(msg->data);
            vehicle->car->SetTransparency(vehicle->transparency);
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_SET_COLLISION_STATE, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity && entity->user_data) {
            auto vehicle = (mafia_vehicle*)entity->user_data;

            vehicle->collision_state = (b32)librg_data_ru8(msg->data);
            vehicle->car->SetColsOn(vehicle->collision_state);
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_REPAIR, [](librg_message* msg) {
        auto entity_id = librg_data_rent(msg->data);
        auto entity = librg_entity_fetch(&network_context, entity_id);
        if (entity && entity->user_data) {
            auto vehicle = (mafia_vehicle*)entity->user_data;

            vehicle->car->RepairPosition(true);
            vehicle->car->SetColsOn(vehicle->collision_state);

            auto vehicle_int = vehicle->car->GetInterface()->vehicle_interface;

            zpl_vec3 new_pos = EXPAND_VEC(vehicle_int.position);
            lib_inter_reset(vehicle->interp.pos, new_pos);

            zpl_vec3 new_rot = EXPAND_VEC(vehicle_int.rot_forward);
            lib_inter_reset(vehicle->interp.rot, new_rot);

            zpl_vec3 new_rot_up = EXPAND_VEC(vehicle_int.rot_up);
            lib_inter_reset(vehicle->interp.rot_up, new_rot_up);
        }
    });
}
