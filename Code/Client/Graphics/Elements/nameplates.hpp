#pragma once

/* 
* Renders simple nicknames 
* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
*/

namespace nameplates {
    MafiaSDK::ITexture* voip_texture = nullptr;

    void render() {
        
        if(!voip_texture) {
            voip_texture = MafiaSDK::GetIGraph()->CreateITexture();
            voip_texture->Create("gramafon.tga", "", 8, 100, 100, 0);
        }

        for (u32 i = 0; i < network_context.max_entities; i++) {

            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (!entity || entity->id == local_player.entity.id) continue;

            if (entity->type == TYPE_PLAYER && entity->user_data) {
                auto player = reinterpret_cast<mafia_player*>(entity->user_data);
                if (player->ped) {
                    auto player_pos = player->ped->GetInterface()->neckFrame->GetInterface()->mPosition;
                    Vector3D local_player_pos;
                    {
                        auto local_player_int = local_player.ped;

                        if (!local_player_int)
                            break;

                        local_player_pos = local_player_int->GetInterface()->humanObject.neckFrame->GetInterface()->mPosition;
                    }
                    auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.4f, player_pos.z });
                    
                    if(screen.z < 1.0f) {
                        if(zpl_time_now() - player->last_talked < 5.0f && voip_texture)
                            voip_texture->Draw2D(screen.x - (100 / 2),  screen.y - 150);

                        zpl_vec3 vec = {};
                        zpl_vec3_sub(&vec, EXPAND_VEC(player_pos), EXPAND_VEC(local_player_pos));
                        auto dist = zpl_vec3_mag(vec);
                        printf("Distance: %f\n", dist);
                        auto orig_font_size = 20.0f;
                        auto min_safe_dist = 3.0f;
                        auto font_size = orig_font_size / zpl_sqrt(dist);

                        auto size = MafiaSDK::GetMission()->GetGame()->GetIndicators()->TextSize(player->name, font_size, 1, 0);
                        auto render_x = screen.x - (size / 2);
                        auto render_y = screen.y;
                        
                        //draw 8x more smoother ?
                        for(int i = 0; i < 8; i++) 
                            MafiaSDK::GetMission()->GetGame()->GetIndicators()->OutText(player->name, screen.x - (size / 2), screen.y, size, font_size, 0xFFFFFFFF, 1, 1);
                        
                    }
                }
            }
        }
    }
}