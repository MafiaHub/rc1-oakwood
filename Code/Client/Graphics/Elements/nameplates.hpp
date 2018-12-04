#pragma once
/* 
* Renders simple nicknames 
* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
*/
namespace nameplates {
   
    ID3DXFont* nameplate_font = nullptr;

    inline auto get_current_i3dcamera() -> MafiaSDK::I3D_Frame* {
        __asm {
            mov eax, dword ptr ds : [63788Ch]
            mov ecx, dword ptr ds : [eax + 10h]
            mov eax, dword ptr ds : [ecx + 17Ch]
        }
    }

    inline auto mod_vec3_to_color(zpl_vec3 rhs) {
        DWORD color = 0x0FF000000;
        color |= ((DWORD)rhs.r << 16);
        color |= ((DWORD)rhs.g << 8);
        color |= ((DWORD)rhs.b);
        return color;
    }

    inline auto iterate_players(std::function<void(mafia_player*)> fptr) {
        for (u32 i = 0; i < network_context.max_entities; i++) {

            librg_entity *entity = librg_entity_fetch(&network_context, i);
            if (entity == nullptr || entity->id == local_player.entity_id) continue;

            if (entity->type == TYPE_PLAYER && entity->user_data && fptr != nullptr) {
                fptr(reinterpret_cast<mafia_player*>(entity->user_data));
            }
        }
    }

    inline void init(IDirect3DDevice9* device) {
        nameplate_font = graphics::create_font(device, "tahoma-bold", 22, true);
    }

    /*
    * When device lost 
    * Free all needed elements
    */
    inline void device_lost() {
        if (nameplate_font) {
            nameplate_font->Release();
            nameplate_font = nullptr;
        }

        if (nameplate_font) {
            nameplate_font->Release();
            nameplate_font = nullptr;
        }
    }

    /* 
    * Recreated all freed elements after device is reseted
    */
    inline void device_reset(IDirect3DDevice9* device) {
        init(device);
    }

    inline void render(IDirect3DDevice9* device) {

        iterate_players([=](mafia_player* player) {
            if (player->ped && player->ped->GetInterface()->neckFrame) {
                auto player_pos = player->ped->GetInterface()->neckFrame->GetInterface()->mPosition;
                auto player_health = player->health;

                auto current_i3d_camera = get_current_i3dcamera();
                if (current_i3d_camera == nullptr) return;

                S_vector camera_pos = current_i3d_camera->GetInterface()->mPosition;
                auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.45f, player_pos.z });

                if (screen.z < 1.0f) {

                    zpl_vec3 vec = {};
                    zpl_vec3_sub(&vec, EXPAND_VEC(player_pos), EXPAND_VEC(camera_pos));
                    auto dist = zpl_vec3_mag(vec);
                    auto dist_sq = zpl_sqrt(dist);

                    constexpr auto orig_font_size = 30.0f;
                    auto font_size = orig_font_size / dist_sq;
                    if (font_size > orig_font_size)
                        font_size = orig_font_size;

                    auto size = graphics::get_text_width(nameplate_font, player->name) * (1 / dist_sq);
                    auto render_x = screen.x;
                    auto render_y = screen.y;
                    graphics::draw_text(nameplate_font, player->name, screen.x - (size / 2), screen.y, 1 / dist_sq, 0xFFFFFFFF, true);
                    
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

                    float width = (player_health * scaled_width) / 200.0f;
                    graphics::draw_box(device, render_x - 2.0f, screen.y - 2.0f, scaled_width + 2.0f, scaled_height + 2.0f, final_bg_color);
                    graphics::draw_box(device, render_x, screen.y, width, scaled_height, final_color);
                }
            }
        });
    }
}
