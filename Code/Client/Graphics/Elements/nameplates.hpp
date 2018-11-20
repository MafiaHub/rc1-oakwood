#pragma once

/* 
* Renders simple nicknames 
* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
*/
namespace nameplates {
    MafiaSDK::ITexture* voip_texture = nullptr;

	MafiaSDK::I3D_Frame* get_current_i3dcamera() {
		__asm {
			mov eax, dword ptr ds : [63788Ch]
			mov ecx, dword ptr ds : [eax + 10h]
			mov eax, dword ptr ds : [ecx + 17Ch]
		}
	}

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
					auto player_health = player->ped->GetInterface()->health;
					
					bool in_same_car = false;
					Vector3D camera_pos = get_current_i3dcamera()->GetInterface()->mPosition;

                    {
                        auto local_player_int = local_player.ped;

                        if (!local_player_int)
                            break;

                        //local_player_pos = local_player_int->GetInterface()->humanObject.neckFrame->GetInterface()->mPosition;
						in_same_car = (local_player_int->GetInterface()->humanObject.playersCar == player->ped->GetInterface()->playersCar) && player->ped->GetInterface()->playersCar != nullptr;
					}

                    auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.45f, player_pos.z });
                    
                    if(screen.z < 1.0f) {
                        if(zpl_time_now() - player->last_talked < 5.0f && voip_texture)
                            voip_texture->Draw2D(screen.x - (100 / 2),  screen.y - 150);

                        zpl_vec3 vec = {};
                        zpl_vec3_sub(&vec, EXPAND_VEC(player_pos), EXPAND_VEC(camera_pos));
                        auto dist = zpl_vec3_mag(vec);
						auto dist_sq = zpl_sqrt(dist);


                        constexpr auto orig_font_size = 20.0f;
						auto font_size = orig_font_size / dist_sq;
						if (font_size > orig_font_size)
							font_size = orig_font_size;

                        auto size = MafiaSDK::GetMission()->GetGame()->GetIndicators()->TextSize(player->name, font_size, 1, 0);
                        auto render_x = screen.x - (size / 2);
                        auto render_y = screen.y;
                        
                        MafiaSDK::GetMission()->GetGame()->GetIndicators()->OutText(player->name, screen.x - (size / 2), screen.y, size, font_size, 0xFFFFFFFF, 0, 1);
                        
						constexpr auto health_width = 135.0f;
						constexpr auto health_height = 15.0f;

						auto scaled_width = health_width / dist_sq;
						auto scaled_height = health_height / dist_sq;

						if (scaled_width > health_width)
							scaled_width = health_width;
						
						if (scaled_height > health_height)
							scaled_height = health_height;

						screen.y += scaled_height + 5.0f;
						render_x = screen.x - (scaled_width / 2);
						MafiaSDK::GetMission()->GetGame()->GetIndicators()->FilledRectangle(render_x, screen.y, scaled_width, scaled_height, 0x0FFFF0000, player_health / 200.0f);
						MafiaSDK::GetMission()->GetGame()->GetIndicators()->Rectangle(render_x - 1.0f, screen.y - 1.0f, scaled_width + 1.0f, scaled_height + 1.0f, 0xFFFFFFFF);
                    }
                }
            }
        }
    }
}