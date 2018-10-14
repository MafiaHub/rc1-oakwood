#pragma once

/* 
* Renders simple nicknames 
* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
*/

namespace nameplates {
    auto render() {
        for (u32 i = 0; i < network_context.max_entities; i++) {

            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (!entity || entity->id == local_player.entity.id) continue;

            if (entity->type == TYPE_PLAYER && entity->user_data) {
                auto player = reinterpret_cast<mafia_player*>(entity->user_data);

                if (player->ped) {
                    
                    auto player_pos = player->ped->GetInterface()->neckFrame->GetInterface()->mPosition;
                    auto player_pos_zpl = zpl_vec3f(player_pos.x, player_pos.y, player_pos.z);

                    zpl_vec3 dir;
                    zpl_vec3_sub(&dir, player_pos_zpl, local_player.entity.position);
                    zpl_vec3_norm0(&dir, dir);

                    auto fwd = player->ped->GetInterface()->neckFrame->GetInterface()->mRotation;
                    auto fwd_zpl = zpl_vec3f(fwd.x, fwd.y, fwd.z);

                    if (zpl_vec3_dot(dir, fwd_zpl) <= 0.f)
                        continue;

                    auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.4f, player_pos.z });
                    
                    if (screen.z > 0.0f) {
                        auto size = MafiaSDK::GetMission()->GetGame()->GetIndicators()->TextSize(player->name, 20.0f, 1, 0);
                        
                        //draw 8x more smoother ?
                        for(int i = 0; i < 8; i++)
                            MafiaSDK::GetMission()->GetGame()->GetIndicators()->OutText(player->name, screen.x - (size / 2), screen.y, size, 20.0f, 0xFFFFFFFF, 1, 1);
                    }
                }
            }
        }
    }
}