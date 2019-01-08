#pragma once
namespace gamemap
{
    D3DSURFACE_DESC back_buffer_desc;
    constexpr double convert_width_coef         = 1.3429f;
    constexpr double convert_height_coef        = 1.5903f;
    constexpr int blip_size                     = 15;

    zpl_vec2 translate_object_to_map(zpl_vec3 position) {
        auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();
        if (local_player) {
            auto player_frame = local_player->GetInterface()->humanObject.entity.frame;
            if (player_frame) {
                auto player_pos = player_frame->GetInterface()->mPosition;
                zpl_vec2 center_offset = { (player_pos.x - position.x) / convert_width_coef, (player_pos.z - position.z) / convert_height_coef };
                return { -center_offset.x - (back_buffer_desc.Width / 2), center_offset.y - (back_buffer_desc.Height / 2) };
            }
        }
        return {};
    }

    inline void init(IDirect3DDevice9* device) {
        back_buffer_desc = graphics::get_backbuffer_desc(device);
    }

    inline void render() {
        if (global_device != nullptr) {
            for (u32 i = 0; i < network_context.max_entities; i++) {
                librg_entity *entity = librg_entity_fetch(&network_context, i);
                if (entity == nullptr || entity->id == local_player.entity_id) continue;
                if (entity->type == TYPE_PLAYER) {
                    auto blip_pos = translate_object_to_map(entity->position);
                    graphics::draw_box(global_device, blip_pos.x + blip_size / 2, blip_pos.y + blip_size / 2, blip_size, blip_size, 0xFFFFFFFF);
                }
            }
        }
    }
}
