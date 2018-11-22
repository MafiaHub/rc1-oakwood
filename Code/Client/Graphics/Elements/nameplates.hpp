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

	DWORD mod_vec3_to_color(zpl_vec3 rhs) {
		DWORD color = 0x0FF000000;
		color |= ((DWORD)rhs.r << 16);
		color |= ((DWORD)rhs.g << 8);
		color |= ((DWORD)rhs.b);

		return color;
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
					auto player_health = player->health;
					
					Vector3D camera_pos = get_current_i3dcamera()->GetInterface()->mPosition;

                    {
                        auto local_player_int = local_player.ped;

                        if (!local_player_int)
                            break;
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

						zpl_vec3 health_color_min = { 255.0f, 0.0f, 0.0f };
						zpl_vec3 health_color_max = { 0.0f, 255.0f, 0.0f };
						zpl_vec3 calculated_color, background_color;
						f32 health = zpl_clamp(player_health / 200.0f, 0.0f, 1.0f);
						zpl_vec3_lerp(&calculated_color, health_color_min, health_color_max, health);
						zpl_vec3_mul(&background_color, calculated_color, 0.2f);

						DWORD final_color = mod_vec3_to_color(calculated_color);
						DWORD final_bg_color = mod_vec3_to_color(background_color);


						MafiaSDK::GetMission()->GetGame()->GetIndicators()->FilledRectangle(render_x - 2.0f, screen.y - 2.0f, scaled_width + 2.0f, scaled_height + 2.0f, final_bg_color, 1.0f);
						MafiaSDK::GetMission()->GetGame()->GetIndicators()->FilledRectangle(render_x, screen.y, scaled_width, scaled_height, final_color, health);
                    }
                }
            }
        }
    }
}