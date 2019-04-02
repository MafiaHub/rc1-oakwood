#ifdef OAKWOOD_DEBUG
namespace debug {
    
    input::KeyToggle debug_key(VK_F2);

    /* camera movement */
    S_vector debug_cam_pos;
    S_vector debug_cam_dir;
    int delta_x = 0;
    int delta_y = 0;

    /* debug rendering  */
    librg_entity* selected_entity = nullptr;
   
    inline void init() {}

    inline void render_librg_related() {
        
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "librg_entity*");
        ImGui::Indent();
        ImGui::Text("Position: %f %f %f", selected_entity->position.x, selected_entity->position.y, selected_entity->position.z);
        ImGui::Text("Is Streamed: %X", !(selected_entity->flags & ENTITY_INTERPOLATED));

        switch (selected_entity->type) {
        case TYPE_PLAYER: {
            auto player = (mafia_player*)selected_entity->user_data;
            ImGui::Text("Type: Player");
            ImGui::Unindent();

            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "mafia_player*");
            ImGui::Indent();
            ImGui::Text("Ping: %d", player->ping);
            ImGui::Text("Health: %f", player->health);
            ImGui::Text("Is aiming: %d", player->is_aiming);
            ImGui::Text("Is crouching: %d", player->is_crouching);
            ImGui::Text("Animation: %d", player->animation_state);
            ImGui::Text("Vehicle id: %d", player->vehicle_id);
            ImGui::Text("Current weapon: %d", player->current_weapon_id);
            ImGui::Text("Name: %s", player->name);
            ImGui::Text("Model: %s", player->model);
            ImGui::Text("Pose: %f %f %f", player->pose.x, player->pose.y, player->pose.z);
            ImGui::Text("Rot: %f %f %f", player->rotation.x, player->rotation.y, player->rotation.z);

            ImGui::NewLine();
            ImGui::Text("Inventory:");
                ImGui::Indent();
                for (int i = 0; i < 8; i++) {
                    ImGui::Text("Weapon[%d]: %d", i, player->inventory.items[i]);
                }
                ImGui::Unindent();

            ImGui::Unindent();
        }  break;

        case TYPE_VEHICLE: {
            auto vehicle = (mafia_vehicle*)selected_entity->user_data;
            ImGui::Text("Type: Vehicle");
            ImGui::Unindent();

            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "mafia_vehicle*");
            ImGui::Indent();
            ImGui::Text("Accelerating: %f", vehicle->accelerating);
            ImGui::Text("Break: %f", vehicle->break_val);
            ImGui::Text("Clutch: %f", vehicle->clutch);
            ImGui::Text("Engine health: %f", vehicle->engine_health);
            ImGui::Text("Engine On: %d", vehicle->engine_on);
            ImGui::Text("Engine RPM: %f", vehicle->engine_rpm);
            ImGui::Text("Fuel: %f", vehicle->fuel);
            ImGui::Text("Gear: %d", vehicle->gear);
            ImGui::Text("Hand break: %f", vehicle->hand_break);
            ImGui::Text("Health: %f", vehicle->health);
            ImGui::Text("Horn: %d", vehicle->horn);
            ImGui::Text("Siren: %d", vehicle->siren);
            ImGui::Text("Sound On: %d", vehicle->sound_enabled);
            ImGui::Text("Speed: %f %f %f", vehicle->speed.x, vehicle->speed.y, vehicle->speed.z);
            ImGui::Text("Rot Speed: %f %f %f", vehicle->rot_speed.x, vehicle->rot_speed.y, vehicle->rot_speed.z);
            ImGui::Text("Model: %s", vehicle->model);
            ImGui::Text("Seats: [%d, %d, %d, %d]", vehicle->seats[0], vehicle->seats[1], vehicle->seats[2], vehicle->seats[3]);
            ImGui::Unindent();

            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "mafia_vehicle* interpolation");
            ImGui::Indent();

            ImGui::Text("Pos");
            ImGui::Text("Error: %f %f %f", vehicle->interp.pos.error.x, vehicle->interp.pos.error.y, vehicle->interp.pos.error.z);
            ImGui::Text("Start: %f %f %f", vehicle->interp.pos.start.x, vehicle->interp.pos.start.y, vehicle->interp.pos.start.z);
            ImGui::Text("Target: %f %f %f", vehicle->interp.pos.target.x, vehicle->interp.pos.target.y, vehicle->interp.pos.target.z);
            ImGui::Text("Start time: %d", vehicle->interp.pos.start_time);
            ImGui::Text("Finish time: %d", vehicle->interp.pos.finish_time);
            ImGui::Text("Last alpha: %d", vehicle->interp.pos.last_alpha);
            ImGui::NewLine();

            ImGui::Text("Rot forward");
            ImGui::Text("Error: %f %f %f", vehicle->interp.rot_forward.error.x, vehicle->interp.rot_forward.error.y, vehicle->interp.rot_forward.error.z);
            ImGui::Text("Start: %f %f %f", vehicle->interp.rot_forward.start.x, vehicle->interp.rot_forward.start.y, vehicle->interp.rot_forward.start.z);
            ImGui::Text("Target: %f %f %f", vehicle->interp.rot_forward.target.x, vehicle->interp.rot_forward.target.y, vehicle->interp.rot_forward.target.z);
            ImGui::Text("Start time: %d", vehicle->interp.rot_forward.start_time);
            ImGui::Text("Finish time: %d", vehicle->interp.rot_forward.finish_time);
            ImGui::Text("Last alpha: %d", vehicle->interp.rot_forward.last_alpha);

            ImGui::NewLine();
            ImGui::Text("Rot up");
            ImGui::Text("Error: %f %f %f", vehicle->interp.rot_up.error.x, vehicle->interp.rot_up.error.y, vehicle->interp.rot_up.error.z);
            ImGui::Text("Start: %f %f %f", vehicle->interp.rot_up.start.x, vehicle->interp.rot_up.start.y, vehicle->interp.rot_up.start.z);
            ImGui::Text("Target: %f %f %f", vehicle->interp.rot_up.target.x, vehicle->interp.rot_up.target.y, vehicle->interp.rot_up.target.z);
            ImGui::Text("Start time: %d", vehicle->interp.rot_up.start_time);
            ImGui::Text("Finish time: %d", vehicle->interp.rot_up.finish_time);
            ImGui::Text("Last alpha: %d", vehicle->interp.rot_up.last_alpha);

            ImGui::Unindent();
        }   break;
        }
    }

    inline void render_mafia_entity() {

        MafiaSDK::C_Entity_Interface* mafia_entity = nullptr;
        switch (selected_entity->type) {
            case TYPE_PLAYER: {
                auto player = (mafia_player*)selected_entity->user_data;
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "C_Human*");
                ImGui::Indent();

                if (player && player->ped) {
                    auto ped_int = player->ped->GetInterface();
                    mafia_entity = &ped_int->entity;
                    ImGui::Text("Anim state: %d", ped_int->animState);
                    ImGui::Text("Car leaving/entering: %X", ped_int->carLeavingOrEntering);
                    ImGui::Text("Current vehicle: %X", ped_int->playersCar);
                    ImGui::Text("Health: %X", ped_int->health);
                    ImGui::Text("Is aiming: %d", ped_int->isAiming);
                    ImGui::Text("Is ducking: %d", ped_int->isDucking);
                    ImGui::Text("Is reloading: %d", ped_int->isReloading);

                    ImGui::NewLine();
                    ImGui::Text("Inventory:");
                    ImGui::Indent();
                    
                    for (int i = 0; i < 8; i++)
                        ImGui::Text("Weapon[%d]: %d", i, ped_int->inventory.items[i]);
                    
                    ImGui::Unindent();
                }

                ImGui::Unindent();
                ImGui::NewLine();
            } break;

            case TYPE_VEHICLE: {
                auto vehicle = (mafia_vehicle*)selected_entity->user_data;
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "C_Car*/C_Vehicle*");
                ImGui::Indent();

                if (vehicle && vehicle->car) {
                    auto vehicle_int = &vehicle->car->GetInterface()->vehicle_interface;
                    mafia_entity = &vehicle->car->GetInterface()->entity;
                   
                    ImGui::Text("Accelerating: %f", vehicle_int->accelerating);
                    ImGui::Text("Break: %f", vehicle_int->break_val);
                    ImGui::Text("Clutch: %f", vehicle_int->clutch);
                    ImGui::Text("Engine health: %f", vehicle_int->engine_health);
                    ImGui::Text("Engine On: %d", vehicle_int->engine_on);
                    ImGui::Text("Engine RPM: %f", vehicle_int->engine_rpm);
                    ImGui::Text("Fuel: %f", vehicle_int->fuel);
                    ImGui::Text("Gear: %d", vehicle_int->gear);
                    ImGui::Text("Hand break: %f", vehicle_int->hand_break);

                    ImGui::SliderFloat("Health", &vehicle_int->health, 0.0, 100.0f);
                    ImGui::Checkbox("Horn", (bool*)&vehicle_int->horn);
                    ImGui::Checkbox("Siren", (bool*)&vehicle_int->siren);
                    ImGui::Checkbox("Sound ON", (bool*)&vehicle_int->sound_enabled);
                    
                    ImGui::SliderFloat("SpeedX", &vehicle_int->speed.x, -200.0f, 200.0f, "%.3f");
                    ImGui::SliderFloat("SpeedY", &vehicle_int->speed.y, -200.0f, 200.0f, "%.3f");
                    ImGui::SliderFloat("SpeedZ", &vehicle_int->speed.z, -200.0f, 200.0f, "%.3f");

                    ImGui::SliderFloat("RotSpeedX", &vehicle_int->rot_speed.x, -200.0f, 200.0f, "%.3f");
                    ImGui::SliderFloat("RotSpeedY", &vehicle_int->rot_speed.y, -200.0f, 200.0f, "%.3f");
                    ImGui::SliderFloat("RotSpeedZ", &vehicle_int->rot_speed.z, -200.0f, 200.0f, "%.3f");
                }

                ImGui::Unindent();
                ImGui::NewLine();
            } break;
        }

        if (mafia_entity) {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "C_actor*");
            ImGui::Indent();
            ImGui::Text("C_actor Type: %d", mafia_entity->objectType);
            ImGui::InputFloat("PosX", &mafia_entity->position.x, -10000.0f, 10000.0f, "%.3f");
            ImGui::InputFloat("PosY", &mafia_entity->position.y, -10000.0f, 10000.0f, "%.3f");
            ImGui::InputFloat("PosZ", &mafia_entity->position.z, -10000.0f, 10000.0f, "%.3f");
            ImGui::NewLine();
            ImGui::InputFloat("DirX", &mafia_entity->rotation.x, -1.0f, 1.0f, "%.3f");
            ImGui::InputFloat("DirY", &mafia_entity->rotation.y, -1.0f, 1.0f, "%.3f");
            ImGui::InputFloat("DirZ", &mafia_entity->rotation.z, -1.0f, 1.0f, "%.3f");
            ImGui::Unindent();
            ImGui::NewLine();
        }
    }


    inline void render_engine_frame(MafiaSDK::I3D_Frame* selected) {

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "I3D_frame*");
        ImGui::Indent();
        auto frame_int = selected->GetInterface();
        ImGui::Text("Frame name: %s", frame_int->name);
        ImGui::NewLine();
        ImGui::InputFloat("PosX", &frame_int->position.x, -10000.0f, 10000.0f, "%.3f");
        ImGui::InputFloat("PosX", &frame_int->position.y, -10000.0f, 10000.0f, "%.3f");
        ImGui::InputFloat("PosX", &frame_int->position.z, -10000.0f, 10000.0f, "%.3f");
        ImGui::NewLine();
        ImGui::InputFloat("DirX", &frame_int->rotation.x, -1.0f, 1.0f, "%.3f");
        ImGui::InputFloat("DirY", &frame_int->rotation.y, -1.0f, 1.0f, "%.3f");
        ImGui::InputFloat("DirZ", &frame_int->rotation.z, -1.0f, 1.0f, "%.3f");
        ImGui::NewLine();

        S_vector scale = selected->GetScale();
        ImGui::SliderFloat("ScaleX", &scale.x, 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("ScaleY", &scale.y, 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("ScaleZ", &scale.z, 0.0f, 10.0f, "%.3f");
        selected->SetScale(scale);

        ImGui::Unindent();
        ImGui::NewLine();
    }

    inline void render_selected_entity() {
         
        if (selected_entity && selected_entity->user_data) {
          
            if (ImGui::TreeNode("Remote data")) {
                render_librg_related();
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Local data")) {
                render_mafia_entity();
                ImGui::TreePop();
            }

            MafiaSDK::I3D_Frame* selected_frame = nullptr;
            switch (selected_entity->type) {
                case TYPE_PLAYER: {
                    auto player = (mafia_player*)selected_entity->user_data;
                    if (player && player->ped) {
                        selected_frame = player->ped->GetFrame();
                    }
                } break;
                case TYPE_VEHICLE: {
                    auto vehicle = (mafia_vehicle*)selected_entity->user_data;
                    if (vehicle && vehicle->car) {
                        selected_frame = vehicle->car->GetFrame();
                    }
                } break;
            }
            
            
            if (selected_frame) {

                if (ImGui::TreeNode("LS3D frame")) {
                    render_engine_frame(selected_frame);
                    ImGui::TreePop();
                }

                S_matrix frame_matrix = selected_frame->GetMatrix();
                MafiaSDK::I3D_bsphere debug_sphere  = { 0.0f, 0.0f, 0.0f, 0.0f };
                S_vector debug_sphere_color         = { 1.0f, 0.0, 0.0f };
                
                debug_sphere.radius     = frame_matrix.GetUScale();
                debug_sphere.offsetY    = debug_sphere.radius;
                MafiaSDK::I3DGetDriver()->DrawSphere(frame_matrix, debug_sphere, debug_sphere_color, 0);
            }
        }
    }

    inline bool check_input() {
        bool state = !!debug_key;
        if (state) {
            input::block_input(state);

            // After we enable debug mode, camera should be near local player
            auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
            auto local_ped = player::get_local_ped();
            if (cam && local_ped) {
                if (menuActiveState != Menu_DebugMode) {
                    debug_cam_pos = local_ped->GetFrame()->GetInterface()->position;
                }
                else {
                    cam->Unlock();

                    if (local_ped->GetInterface()->humanObject.playersCar != nullptr)
                        cam->SetCar(local_ped->GetInterface()->humanObject.playersCar);
                    else
                        cam->SetPlayer(local_ped);
                }
            }

            if (menuActiveState == Menu_DebugMode) {
                menuActiveState = Menu_Chat;
                modules::chat::is_focused = false;
                input::block_input(false);
                return false;
            }
        }

        return state;
    }

    inline void render() {
        if (librg_is_connected(&network_context)) {
            ImGui::Begin("Debug Menu", nullptr);

            //ImGui::SetWindowSize(ImVec2(500, 600));
            
            if (selected_entity)
                render_selected_entity();
            
            ImGui::End();
        }
    }

    float speed = 5.0f;
    #define RADIAN (180.0f /  ZPL_PI)
    #define ASIN(x) (asin(x) * RADIAN)

    inline POINT get_cursorpos() {
        POINT current_pos;
        GetCursorPos(&current_pos);
        ScreenToClient((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), &current_pos);
        return current_pos;
    }

    inline librg_entity* get_nearest_entity(zpl_vec3 point, float* distance) {
        librg_entity* nearest_entity = nullptr;
        for (int i = 0; i < network_context.max_entities; i++) {
            auto entity = librg_entity_fetch(&network_context, i);
            if (entity) {
                
                zpl_vec3 ent_distance;
                zpl_vec3_sub(&ent_distance, point, entity->position);

                float dist = zpl_vec3_mag(ent_distance);
                if (dist < *distance) {
                    *distance = dist;
                    nearest_entity = entity;
                }
            }
        }
        return nearest_entity;
    }

    inline void raycast_picking_entity() {

        auto cursor_pos = get_cursorpos();
        D3DXMATRIX matProj, matView, matWorld;
        global_device->GetTransform(D3DTS_PROJECTION, &matProj);
        global_device->GetTransform(D3DTS_VIEW, &matView);
        D3DXMatrixIdentity(&matWorld);

        D3DXVECTOR3 v;
        v.x = (((2.0f *  (float)cursor_pos.x) / (float)MafiaSDK::GetIGraph()->Scrn_sx()) - 1) / matProj._11;
        v.y = -(((2.0f * (float)cursor_pos.y) / (float)MafiaSDK::GetIGraph()->Scrn_sy()) - 1) / matProj._22;
        v.z = 1.0f;

        D3DXMATRIX m;
        D3DXVECTOR3 rayOrigin, rayDir;
        D3DXMatrixInverse(&m, NULL, &matView);

        rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
        rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
        rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
        rayOrigin.x = m._41;
        rayOrigin.y = m._42;
        rayOrigin.z = m._43;

        D3DXMATRIX matInverse;
        D3DXMatrixInverse(&matInverse, NULL, &matWorld);

        D3DXVECTOR3 rayObjOrigin, rayObjDirection;

        D3DXVec3TransformCoord(&rayObjOrigin, &rayOrigin, &matInverse);
        D3DXVec3TransformNormal(&rayObjDirection, &rayDir, &matInverse);
        D3DXVec3Normalize(&rayObjDirection, &rayObjDirection);

        zpl_vec3 ray_origin     = EXPAND_VEC(debug_cam_pos);
        zpl_vec3 ray_dir        = EXPAND_VEC(rayObjDirection);

        constexpr float to_add  = 0.15;
        float distance          = 0.0f;
        float min_distance      = 10.0f;
        float distance_geted    = 10.0f;
        float last_distance     = 10000.0f;

        librg_entity* best_entity = nullptr;
        for (int i = 0; i < 1000; i++) {
            zpl_vec3 current_point = ray_origin;
            zpl_vec3 move;

            zpl_vec3_mul(&move, ray_dir, distance);
            zpl_vec3_add(&current_point, current_point, move);

            auto ent = get_nearest_entity(current_point, &distance_geted);
            if (ent && distance_geted < last_distance) {
                last_distance = distance_geted;
                best_entity = ent;
            }

            distance += to_add;
        }
        selected_entity = best_entity;
    }

    inline void game_update(f64 delta_time) {
        if (menuActiveState == Menu_DebugMode) {
            if (input::InputState.rmb_down) {               
               
                auto current_pos = get_cursorpos();
                RECT client_rect;
                GetClientRect((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), &client_rect);

                auto width = client_rect.right - client_rect.left;
                auto height = client_rect.bottom - client_rect.top;

                delta_x = current_pos.x - width / 2.0f;
                delta_y = current_pos.y - height / 2.0f;

                POINT mpos = {width / 2.0f, height / 2.0f};
                ClientToScreen((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), &mpos);
                SetCursorPos(mpos.x, mpos.y);    
                input::InputState.mouse_move_delta = { 0, 0 };
            } else {            
                
                delta_x = 0;
                delta_y = 0;
            }

            if (input::InputState.lmb_down && !ImGui::IsMouseHoveringAnyWindow() 
                && !ImGui::GetIO().WantCaptureMouse) {
                raycast_picking_entity();
                input::InputState.lmb_down = false;
            }

            if (abs(delta_x) > 1) {
                float angle = DirToRotation360(EXPAND_VEC(debug_cam_dir));
                angle += delta_x * 0.1f;
                float old_y = debug_cam_dir.y;
                debug_cam_dir = EXPAND_VEC(ComputeDirVector(angle));
                debug_cam_dir.y = old_y;
            }

            if (abs(delta_y) > 1) {    
                debug_cam_dir.y = sin((ASIN(debug_cam_dir.y) - (float)delta_y * 0.1f) / RADIAN);
            }

            if (input::is_key_down(VK_SHIFT))
                speed = 10.0f;
            else 
                speed = 5.0f;

            if (input::is_key_down(0x57)) { // W
                debug_cam_pos.x += debug_cam_dir.x * 0.2 * speed;
                debug_cam_pos.y += debug_cam_dir.y * 0.2 * speed;
                debug_cam_pos.z += debug_cam_dir.z * 0.2 * speed;
            }

            if (input::is_key_down(0x53)) { // S
                debug_cam_pos.x -= debug_cam_dir.x * 0.2 * speed;
                debug_cam_pos.y -= debug_cam_dir.y * 0.2 * speed;
                debug_cam_pos.z -= debug_cam_dir.z * 0.2 * speed;
            }

            if (input::is_key_down(0x41)) { // A
                debug_cam_pos.x -= debug_cam_dir.z * 0.2 * speed;
                debug_cam_pos.z += debug_cam_dir.x * 0.2 * speed;
            }

            if (input::is_key_down(0x44)) { // D
                debug_cam_pos.x += debug_cam_dir.z * 0.2 * speed;
                debug_cam_pos.z -= debug_cam_dir.x * 0.2 * speed;
            }

            auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
            if (cam) {
                cam->LockAt(debug_cam_pos, debug_cam_dir);
            }
        }
    }
};
#endif // OAKWOOD_DEBUG