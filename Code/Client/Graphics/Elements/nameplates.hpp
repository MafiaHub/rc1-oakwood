#pragma once

/* 
* Renders simple nicknames 
* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
*/

namespace nameplates {
    MafiaSDK::ITexture* voip_texture = nullptr;

    auto render() {
        
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
                    auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.4f, player_pos.z });
                    
                    if(zpl_time_now() - player->last_talked < 5.0f && voip_texture) {
                        printf("Draw gramofon !\n");
                        voip_texture->Draw2D(screen.x - (100 / 2),  screen.y - 150);
                    }

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