void add_messages() {
    librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [](librg_message* msg) {

        zpl_vec3 speed, unk;
        auto vehicle_id = librg_data_ru32(msg->data);
        auto wheel_idx = librg_data_ru32(msg->data);
        librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
        librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {

            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);
            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

                auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                sender_vehicle->tyres[wheel_idx].health = 0.0f;

                mod_message_send(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, [&](librg_data *data) {
                    librg_data_wu32(data, vehicle_ent->id);
                    librg_data_wu32(data, wheel_idx);
                    librg_data_wptr(data, (void*)&speed, sizeof(zpl_vec3));
                    librg_data_wptr(data, (void*)&unk, sizeof(zpl_vec3));
                });
            }
        }	
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_WHEEL_UPDATE, [](librg_message* msg) {

        auto vehicle_id		= librg_data_ru32(msg->data);
        auto tyre_idx		= librg_data_ru32(msg->data);
        auto tyre_flags		= librg_data_ru32(msg->data);
        auto tyre_health	= librg_data_rf32(msg->data);

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {

            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);

            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

                auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                
                sender_vehicle->tyres[tyre_idx].flags	= tyre_flags;
                sender_vehicle->tyres[tyre_idx].health	= tyre_health;

                mod_message_send(&network_context, NETWORK_VEHICLE_WHEEL_UPDATE, [&](librg_data *data) {
                    librg_data_wu32(data, vehicle_ent->id);
                    librg_data_wu32(data, tyre_idx);
                    librg_data_wu32(data, tyre_flags);
                    librg_data_wf32(data, tyre_health);
                });
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, [](librg_message* msg) {

        zpl_vec3 speed, unk;
        auto vehicle_id = librg_data_ru32(msg->data);
        auto component_idx = librg_data_ru32(msg->data);
        librg_data_rptr(msg->data, &speed, sizeof(zpl_vec3));
        librg_data_rptr(msg->data, &unk, sizeof(zpl_vec3));

        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {

            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);
            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

                auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;
                sender_vehicle->destroyed_components[component_idx] = 1;

                mod_message_send(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, [&](librg_data *data) {
                    librg_data_wu32(data, vehicle_ent->id);
                    librg_data_wu32(data, component_idx);
                    librg_data_wptr(data, (void*)&speed, sizeof(zpl_vec3));
                    librg_data_wptr(data, (void*)&unk, sizeof(zpl_vec3));
                });
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_EXPLODE, [](librg_message* msg) {

        auto vehicle_id = librg_data_ru32(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        if (vehicle_ent && vehicle_ent->user_data) {

            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);
            
            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

                auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;	
                mod_message_send(&network_context, NETWORK_VEHICLE_EXPLODE, [&](librg_data *data) {
                    librg_data_wu32(data, vehicle_ent->id);
                });

                destroy_vehicle(vehicle_ent);
            }
        }
    });

    librg_network_add(&network_context, NETWORK_VEHICLE_DEFORM_DELTA, [](librg_message* msg) {

        auto vehicle_id = librg_data_ru32(msg->data);
        auto vehicle_ent = librg_entity_fetch(&network_context, vehicle_id);

        auto deltas_count = librg_data_ru32(msg->data);
        std::vector<mafia_vehicle_deform> deltas_recv;

        for (u32 i = 0; i < deltas_count; i++) {
            mafia_vehicle_deform delta;
            librg_data_rptr(msg->data, &delta, sizeof(mafia_vehicle_deform));
            deltas_recv.push_back(delta);
        }

        if (vehicle_ent && vehicle_ent->user_data) {

            auto sender_ent = librg_entity_find(&network_context, msg->peer);
            auto control_peer = librg_entity_control_get(&network_context, vehicle_ent->id);

            if (sender_ent && sender_ent->client_peer == control_peer && control_peer != nullptr) {

                auto sender_vehicle = (mafia_vehicle*)vehicle_ent->user_data;

                for (mafia_vehicle_deform recv_delta : deltas_recv) {

                    bool updated = false;
                    for (mafia_vehicle_deform& current_delta : sender_vehicle->deform_deltas) {    
                        
                        if (recv_delta.mesh_index == current_delta.mesh_index &&
                            recv_delta.vertex_index == current_delta.vertex_index) {

                            current_delta.normal = recv_delta.normal;
                            current_delta.position = recv_delta.position;
                            updated = true;
                            break;
                        }
                    }

                    if(!updated)
                        sender_vehicle->deform_deltas.push_back(recv_delta);
                }

                auto deltas_count_srv = sender_vehicle->deform_deltas.size();
                if (deltas_count_srv) {
                    mod_message_send(&network_context, NETWORK_VEHICLE_DEFORM_DELTA, [&](librg_data *data) {
                        librg_data_wu32(data, vehicle_ent->id);
                        librg_data_wu32(data, deltas_count_srv);
                        librg_data_wptr(data, sender_vehicle->deform_deltas.data(), deltas_count_srv * sizeof(mafia_vehicle_deform));
                    });
                }
            }
        }
    });

}
