#pragma once
namespace gamemap
{
    zpl_vec2 center_player_minimap;

    constexpr double convert_width_coef         = 1.338827f;
    constexpr double convert_height_coef        = 1.63396f;
    constexpr int blip_size                     = 20;

    #define MAFIA_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )
    struct mafia_vertex {
        float x, y, z;
        float rhw;
        DWORD color;
        DWORD spec;
        float tu, tv;
    };

    inline zpl_vec2 translate_object_to_map(zpl_vec3 position) {
        auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();
        if (local_player) {
            auto player_frame = local_player->GetInterface()->humanObject.entity.frame;
            if (player_frame) {
                auto player_pos = player_frame->GetInterface()->mPosition;
                zpl_vec2 center_offset = { (player_pos.x - position.x) / convert_width_coef, (player_pos.z - position.z) / convert_height_coef };
                return { -center_offset.x + center_player_minimap.x, center_offset.y + center_player_minimap.y };
            }
        }
        return {};
    }

    inline void draw_player_cursor(void* vertex_buffer) {
        
        mafia_vertex* buffer = (mafia_vertex*)vertex_buffer;
        zpl_vec2 p1 = { buffer[0].x, buffer[0].y };
        zpl_vec2 p2 = { buffer[1].x, buffer[1].y };
        zpl_vec2 p3 = { buffer[2].x , buffer[2].y };

        zpl_vec2 ptemp;
        zpl_vec2_lerp(&ptemp, p1, p2, 0.5f);
        zpl_vec2_lerp(&center_player_minimap, ptemp, p3, 0.35f);
        center_player_minimap.x -= blip_size / 2;
        center_player_minimap.y -= blip_size / 2;

        for (u32 i = 0; i < network_context.max_entities; i++) {
            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (entity == nullptr || entity->id == local_player.entity_id) continue;
            if (entity->type == TYPE_PLAYER && entity->user_data) {
                mafia_player* player = (mafia_player*)entity->user_data;
                if (player->ped) {
                    auto frame = player->ped->GetInterface()->entity.frame;
                    if (frame) {
                        zpl_vec3 frame_pos = EXPAND_VEC(frame->GetInterface()->mPosition);
                        auto blip_pos = translate_object_to_map(frame_pos);

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
            }
        }
        MafiaSDK::GetIGraph()->DrawPrimitiveList(MafiaSDK::PRIMITIVE_TYPE::TRIANGLELIST, 1, (void*)buffer, MafiaSDK::LS3D_STREAM_TYPE::FILLED);
    }
}
