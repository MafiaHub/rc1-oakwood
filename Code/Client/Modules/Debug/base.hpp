namespace debug {
    
    input::KeyToggle debug_key(VK_F2);
    bool is_debug_enabled = false;
    S_vector debug_cam_pos;
    S_vector debug_cam_dir;

    inline void init() {
        
    }

    inline void render() {
        if (debug_key) {
            is_debug_enabled = !is_debug_enabled;
            input::block_input(is_debug_enabled);

            // After we enable debug mode, camera should be near local player
            auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();
            auto local_ped = player::get_local_ped();
            if (cam && local_ped) {
                if (is_debug_enabled) {
                    debug_cam_pos = player::get_local_ped()->GetInterface()->humanObject.entity.position;
                    cam->Unlock();
                }
                else {
                    if (local_ped->GetInterface()->humanObject.playersCar != nullptr)
                        cam->SetCar(local_ped->GetInterface()->humanObject.playersCar);
                    else
                        cam->SetPlayer(local_ped);
                }
            }
        }

        // Draw debug menu here 
        if (is_debug_enabled) {
        
        }
    }

    constexpr float speed = 5.0f;
#define RADIAN (180.0f /  3.14159f)
#define ASIN(x) (asin(x) * RADIAN)

    int delta_x = 0;
    int delta_y = 0;

    inline void game_update(f64 delta_time) {
        
        // Update free camera here
        if (is_debug_enabled) {

       

            if (input::InputState.rmb_down) {
               
                POINT current_pos;
                GetCursorPos(&current_pos);

                ScreenToClient((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), &current_pos);

                RECT client_rect;
                GetClientRect(
                    (HWND)MafiaSDK::GetIGraph()->GetMainHWND(),
                    &client_rect
                );

                delta_x = current_pos.x - client_rect.left + MafiaSDK::GetIGraph()->Scrn_sx() / 2;
                delta_y = current_pos.y - client_rect.top + MafiaSDK::GetIGraph()->Scrn_sy() / 2;

               

                SetCursorPos(client_rect.left + MafiaSDK::GetIGraph()->Scrn_sx() / 2, 
                    client_rect.top + MafiaSDK::GetIGraph()->Scrn_sy() / 2);
                
                input::InputState.mouse_move_delta = { 0, 0 };
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

            if (input::is_key_down(VK_UP)) {
                debug_cam_pos.x += debug_cam_dir.x * 0.2 * speed;
                debug_cam_pos.y += debug_cam_dir.y * 0.2 * speed;
                debug_cam_pos.z += debug_cam_dir.z * 0.2 * speed;
            }

            if (input::is_key_down(VK_DOWN)) {
                debug_cam_pos.x -= debug_cam_dir.x * 0.2 * speed;
                debug_cam_pos.y -= debug_cam_dir.y * 0.2 * speed;
                debug_cam_pos.z -= debug_cam_dir.z * 0.2 * speed;
            }

            if (input::is_key_down(VK_LEFT)) {
                debug_cam_pos.x -= debug_cam_dir.z * 0.2 * speed;
                debug_cam_pos.z += debug_cam_dir.x * 0.2 * speed;
            }

            if (input::is_key_down(VK_RIGHT)) {
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
