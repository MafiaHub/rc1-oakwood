#pragma once

#include "Game/Hooks/scene.hpp"

namespace effects {
    extern bool is_enabled;
    inline void load(std::string effect_file);
}

u32 transition_idx	= 0;
f64 last_time		= 0.0f;
f64 passed_time		= 2.0f;

std::vector<std::string> forbidden_bridges = {
    "LLsklap01",
    "sklapx01"
};

std::vector<std::pair<zpl_vec3, zpl_vec3>> camera_follow_points = {
    { { -272.304f, 17.6089f, -352.958f },{ -0.0996391f, 0.575007f, 0.995024f } },
    { { -273.665f, 17.4891f, -351.581f },{ -0.159734f, 0.20108f, 0.98716f } },

    { { -840.896f, -5.7629f, -103.007f },{ -0.853791f, 0.393943f, -0.520617f } },
    { { -840.882f, -5.70702f, -103.811f },{ -0.544526f, 0.25882f, -0.638777f } },

    { { -948.201f, -5.75411f, -168.035f },{ -0.999912f, 0.859852f, 0.0133015f } },
    { { -976.991f, -5.71983f, -168.637f },{ -0.999391f, 0.980268f, 0.0348869f } },

    { { -1061.22f, -6.53558f, -163.883f },{ 0.312284f, 0.270617f, 0.949989f } },
    { { -1055.73f, -6.53558f, -164.939f },{ 0.159829f, 0.297392f, 0.987145f } },

    { { -1255.98f, -6.59641f, -318.882f },{ 0.880575f, 0.0627944f, -0.473907f } },
    { { -1255.24f, -6.9736f, -319.205f },{ 0.850002f, 0.384299f, -0.52678f } },

    { { -1417.8f, 0.37499f, -225.593f },{ 0.937219f, 0.300729f, 0.348741f } },
    { { -1416.3f, 0.856156f, -225.035f },{ 0.937219f, 0.300729f, 0.348741f } },

    { { -1177.6f, -7.05525f, -420.047f },{ -0.353346f, 0.302394f, -0.935493f } },
    { { -1178.38f, -7.05525f, -419.84f },{ -0.382129f, 0.290727f, -0.924109f } },

    { { -1784.17f, -4.60527f, -1.51114f },{ 0.945835f, 0.638778f, 0.324649f } },
    { { -1784.93f, -5.0755f, -1.78271f },{ 0.849487f, 0.289045f, 0.52761f } },

    { { -2095.03f, -5.45904f, -515.071f  },{ -0.966289f, 0.195958f, -0.257461f } },
    { { -2102.34f, -4.91622f, -518.325f },{ -0.994021f, 0.405153f, -0.109188f } },

    { { -352.357f, 13.1112f, -361.674f },{ -0.564254f, 0.252069f, -0.825601f } },
    { { -354.06f, 12.8654f, -362.564f },{ -0.627766f, 0.544639f, -0.778402f } },
};

auto interpolate_cam(f64 delta_time) {
    auto cam = MafiaSDK::GetMission()->GetGame()->GetCamera();

    cam->SetCar(nullptr);
    cam->SetPlayer(nullptr);
    cam->Unlock();

    auto from = camera_follow_points.at(transition_idx);
    auto to = camera_follow_points.at(transition_idx + 1);

    if (passed_time > 0.8f && passed_time < 0.82f) {
        MafiaSDK::GetIndicators()->FadeInOutScreen(true, 1000, 0x000000);
    }

    if (passed_time > 1.0f) {		
        if (transition_idx + 2 > camera_follow_points.size() - 1)
            transition_idx = 0;
        else 
            transition_idx += 2;

        MafiaSDK::GetIndicators()->FadeInOutScreen(false, 1000, 0x000000);
        passed_time = 0.0f;
    }

    zpl_vec3 dest_pos;
    zpl_vec3_lerp(&dest_pos, from.first, to.first, passed_time);

    zpl_vec3 dest_rot;
    zpl_vec3_lerp(&dest_rot, from.second, to.second, passed_time);

    S_vector pos = EXPAND_VEC(dest_pos);
    S_vector rot = EXPAND_VEC(dest_rot);
    cam->LockAt(pos, rot);
    passed_time += delta_time * 0.11f;
}

f64 delta_time = 0.0f;
auto mod_bind_events() {

    scene_init();
    local_player_init();
    vehicle_init();
    drop_init();

    MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {

        auto mission_id = MafiaSDK::GetCurrentMissionID();
        if (mission_id == MafiaSDK::C_Mission_Enum::MissionID::FREERIDE || 
            mission_id == MafiaSDK::C_Mission_Enum::MissionID::FREERIDE_NOC) {

            MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);

            for (auto bridge_name : forbidden_bridges) {
                auto bridge = (MafiaSDK::C_Bridge*)MafiaSDK::GetMission()->FindActorByName(bridge_name.c_str());
                if (bridge) {
                    bridge->Shutdown(TRUE);
                }
            }

            chat::add_message("Welcome to Mafia Oakwood 0.1");
            chat::add_message("Connecting to " + GlobalConfig.server_address + " ...");
            mod_librg_connect();

            effects::load(GlobalConfig.localpath + "files/Cinematic.fx");
            effects::is_enabled = true;
        }
    });

    MafiaSDK::C_Game_Hooks::HookLocalPlayerFallDown([&]() {
        local_player_died();
        local_player.dead = true;
    });

    MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {

        delta_time = zpl_time_now() - last_time;

        if (!librg_is_connected(&network_context))
            interpolate_cam(delta_time);

        librg_tick(&network_context);
        //voip::network_tick();

        librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | ENTITY_INTERPOLATED), [](librg_ctx *ctx, librg_entity *entity) {
            switch (entity->type) {
            case TYPE_WEAPONDROP: {
                auto weapon_drop = (mafia_weapon_drop*)entity->user_data;
                if (weapon_drop && weapon_drop->weapon_drop_actor) {
                    drop_game_tick(weapon_drop);
                }
            } break;

            case TYPE_PLAYER: {
                auto player = (mafia_player*)entity->user_data;
                if (player && player->ped && player->streamer_entity_id != local_player.entity_id) {
                    player_game_tick(player, delta_time);
                }
            } break;

            case TYPE_VEHICLE: {
                auto vehicle = (mafia_vehicle*)entity->user_data;
                if (vehicle && vehicle->car) {
                    vehicle_game_tick(vehicle, delta_time);
                }
            } break;
            }
        });

        last_time = zpl_time_now();
    });
}
