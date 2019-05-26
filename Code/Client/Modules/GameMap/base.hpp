#pragma once
namespace gamemap
{
    std::vector<gamemap_info> current_gamemap;
    zpl_vec2 center_player_minimap;
    constexpr double convert_width_coef         = 1.338827f;
    constexpr double convert_height_coef        = 1.63396f;
    constexpr int original_blip_size            = 15;
    constexpr int original_blip_car_size        = 8;
    constexpr int convert_map_pos_x             = 160;
    constexpr int convert_map_pos_y             = 117;

    #define MAFIA_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )
    struct mafia_vertex {
        float x, y, z;
        float rhw;
        DWORD color;
        DWORD spec;
        float tu, tv;
    };

    inline bool is_marker_inbounds(zpl_vec2 position, float blip_size) {

        //NOTE(DavoSK): Position is also gap from each size
        float screen_x = (float)MafiaSDK::GetIGraph()->Scrn_sx();
        float screen_y = (float)MafiaSDK::GetIGraph()->Scrn_sy();

        //NOTE(DavoSK): Gap size from 0 0 to map start position
        float pos_x = convert_map_pos_x * (1600.0f / screen_x) * 1.333f;
        float pos_y = convert_map_pos_y * (900.0f / screen_y);

        return (position.x + blip_size >= pos_x && position.y + blip_size >= pos_y) &&
               (position.x + blip_size <= (screen_x - pos_x) && position.y + blip_size <= (screen_y - pos_y));
    }

    inline zpl_vec2 translate_object_to_map(zpl_vec3 position) {
        auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();
        if (local_player) {
            auto player_frame = local_player->GetInterface()->humanObject.entity.frame;
            if (player_frame) {
                auto player_pos = player_frame->GetInterface()->position;
        
                float x_coef = convert_width_coef * (1600.0f / (float)MafiaSDK::GetIGraph()->Scrn_sx()) * 1.333f;
                float y_coef = convert_height_coef * (900.0f / (float)MafiaSDK::GetIGraph()->Scrn_sy());

                zpl_vec2 center_offset = { 
                    (player_pos.x - position.x) / x_coef,
                    (player_pos.z - position.z) / y_coef
                };

                return { 
                    -center_offset.x + center_player_minimap.x, 
                    center_offset.y + center_player_minimap.y 
                };
            }
        }
        return {};
    }

    f64 last_opened_map = 0.0f;
    inline void draw_player_cursor(void* vertex_buffer) {
        
        float blip_size = original_blip_size / (1600.0f / (float)MafiaSDK::GetIGraph()->Scrn_sx());
        float blip_size_car = original_blip_car_size / (1600.0f / (float)MafiaSDK::GetIGraph()->Scrn_sx());

        mafia_vertex* buffer = (mafia_vertex*)vertex_buffer;
        zpl_vec2 p1 = { buffer[0].x, buffer[0].y };
        zpl_vec2 p2 = { buffer[1].x, buffer[1].y };
        zpl_vec2 p3 = { buffer[2].x , buffer[2].y };

        zpl_vec2 ptemp;
        zpl_vec2_lerp(&ptemp, p1, p2, 0.5f);
        zpl_vec2_lerp(&center_player_minimap, ptemp, p3, 0.35f);
        center_player_minimap.x -= blip_size / 2;
        center_player_minimap.y -= blip_size / 2;

        bool force_update = (zpl_time_now() - last_opened_map) > 0.5f;

        constexpr float GAMEMAP_SYNC_FACTOR = 0.035f;
        for(auto &mapinfo : current_gamemap) {
            zpl_vec3 frame_pos;
            zpl_vec3_lerp(&frame_pos, mapinfo.last_pos, mapinfo.position, GAMEMAP_SYNC_FACTOR);
            mapinfo.last_pos = frame_pos;
          
            if (force_update) {
                frame_pos = mapinfo.position;
                mapinfo.last_pos = frame_pos;
            }

            if (mapinfo.entity_type == TYPE_PLAYER) {
                auto blip_pos = translate_object_to_map(frame_pos);

                if (is_marker_inbounds(blip_pos, blip_size)) {
                    //NOTE(DavoSK): Draw border
                    constexpr int border = 2;
                    const mafia_vertex rect_border[] = {
                        { blip_pos.x - border,			    blip_pos.y - border,	               0.0f, 1.0f,	0xFF000000,  0xFF000000, 0.0, 0.0 },
                        { blip_pos.x + blip_size + border,	blip_pos.y - border,			       0.0f, 1.0f,	0xFF000000,  0xFF000000, 1.0, 0.0 },
                        { blip_pos.x - border,			    blip_pos.y + blip_size + border,       0.0f, 1.0f,	0xFF000000,  0xFF000000, 0.0, 1.0 },
                        { blip_pos.x + blip_size + border,	blip_pos.y + blip_size + border,       0.0f, 1.0f,	0xFF000000,  0xFF000000, 1.0, 1.0 },
                    };

                    MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLESTRIP, 2, (void*)rect_border, MafiaSDK::LS3D_STREAM_TYPE::FILLED);

                    //NOTE(DavoSK): Draw player blip
                    const mafia_vertex rect_blip[] = {
                        { blip_pos.x,			    blip_pos.y,	               0.0f, 1.0f,	0xFF40AEF9,  0xFF40AEF9, 0.0, 0.0 },
                        { blip_pos.x + blip_size,	blip_pos.y,			       0.0f, 1.0f,	0xFF40AEF9,  0xFF40AEF9, 1.0, 0.0 },
                        { blip_pos.x,			    blip_pos.y + blip_size,    0.0f, 1.0f,	0xFF40AEF9,  0xFF40AEF9, 0.0, 1.0 },
                        { blip_pos.x + blip_size,	blip_pos.y + blip_size,    0.0f, 1.0f,	0xFF40AEF9,  0xFF40AEF9, 1.0, 1.0 },
                    };

                    MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLESTRIP, 2, (void*)rect_blip, MafiaSDK::LS3D_STREAM_TYPE::FILLED);
                }
            }

            if (mapinfo.entity_type == TYPE_VEHICLE) {
                auto blip_pos = translate_object_to_map(frame_pos);
                        
                if (is_marker_inbounds(blip_pos, blip_size_car)) {
                    //NOTE(DavoSK): Draw border
                    constexpr int border = 2;
                    const mafia_vertex rect_border[] = {
                        { blip_pos.x - border,			        blip_pos.y - border,	                   0.0f, 1.0f,	0xFF000000,  0xFF000000, 0.0, 0.0 },
                        { blip_pos.x + blip_size_car + border,	blip_pos.y - border,			           0.0f, 1.0f,	0xFF000000,  0xFF000000, 1.0, 0.0 },
                        { blip_pos.x - border,			        blip_pos.y + blip_size_car + border,       0.0f, 1.0f,	0xFF000000,  0xFF000000, 0.0, 1.0 },
                        { blip_pos.x + blip_size_car + border,	blip_pos.y + blip_size_car + border,       0.0f, 1.0f,	0xFF000000,  0xFF000000, 1.0, 1.0 },
                    };

                    MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLESTRIP, 2, (void*)rect_border, MafiaSDK::LS3D_STREAM_TYPE::FILLED);

                    //NOTE(DavoSK): Draw player blip
                    const mafia_vertex rect_blip[] = {
                        { blip_pos.x,			        blip_pos.y,	                   0.0f, 1.0f,	0xFF696969,  0xFF696969, 0.0, 0.0 },
                        { blip_pos.x + blip_size_car,	blip_pos.y,			           0.0f, 1.0f,	0xFF696969,  0xFF696969, 1.0, 0.0 },
                        { blip_pos.x,			        blip_pos.y + blip_size_car,    0.0f, 1.0f,	0xFF696969,  0xFF696969, 0.0, 1.0 },
                        { blip_pos.x + blip_size_car,	blip_pos.y + blip_size_car,    0.0f, 1.0f,	0xFF696969,  0xFF696969, 1.0, 1.0 },
                    };

                    MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLESTRIP, 2, (void*)rect_blip, MafiaSDK::LS3D_STREAM_TYPE::FILLED);
                }
            }
        }
        MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLELIST, 1, (void*)buffer, MafiaSDK::LS3D_STREAM_TYPE::FILLED);

        last_opened_map = zpl_time_now();
    }

    inline void add_messages() {
        librg_network_add(&network_context, NETWORK_PLAYER_UPDATE_GAMEMAP, [](librg_message * msg) {
            
            u32 info_count = librg_data_ru32(msg->data);
            std::vector<gamemap_info> new_snapshot;

            for (u32 i = 0; i < info_count; i++) {
                gamemap_info info;
                bool found = false;
                librg_data_rptr(msg->data, &info, sizeof(gamemap_info));      

                for (auto old_info : current_gamemap) {
                    if (old_info.entity_id == info.entity_id) {
                        info.last_pos = old_info.last_pos;
                        found = true;
                        break;
                    }
                }
         
                if (!found) {
                    info.last_pos = info.position;
                }

                new_snapshot.push_back(info);
            }

            current_gamemap = new_snapshot;
        });
    }
}
