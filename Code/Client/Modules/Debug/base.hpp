namespace debug {
    
    input::KeyToggle debug_key(VK_F2);
    bool is_debug_enabled = false;
    S_vector debug_cam_pos;
    S_vector debug_cam_dir;
    int delta_x = 0;
    int delta_y = 0;

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
#define RADIAN (180.0f /  ZPL_PI)
#define ASIN(x) (asin(x) * RADIAN)

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
                auto width = client_rect.right - client_rect.left;
                auto height = client_rect.bottom - client_rect.top;

                delta_x = current_pos.x - width / 2.0f;
                delta_y = current_pos.y - height / 2.0f;

                POINT mpos = {width / 2.0f, height / 2.0f};
                ClientToScreen((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), &mpos);
                SetCursorPos(mpos.x, mpos.y);
                
                input::InputState.mouse_move_delta = { 0, 0 };
            }
            else {            
                delta_x = 0;
                delta_y = 0;
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
