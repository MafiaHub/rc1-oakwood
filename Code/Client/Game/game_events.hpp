#pragma once
u32 transition_idx	= 0;
f64 last_time		= 0.0f;
f64 passed_time		= 2.0f;

std::vector<std::string> forbidden_bridges = {
    "LLsklap01",
    "sklapx01"
};

#include "connecting_cam.hpp"

f64 delta_time = 0.0f;

auto mod_bind_events() {
    modules::misc::init();
    modules::player::init();
    modules::vehicle::init();
    modules::door::init();
    modules::weapondrop::init();

#ifdef OAKWOOD_DEBUG
    modules::debug::init();
#endif
  
    MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {
        
        //TODO(DavoSK): Move it to sdk
        //NOTE: wtf is this?
        *(BOOL*)(0x006C406C) = true;
      
        auto mission_name = MafiaSDK::GetCurrentMissionName();

        //NOTE(DavoSK): Tutorial for us is main menu / server browser
        if (strcmp(mission_name, "tutorial") == 0) {
            modules::mainmenu::init();
            return;
        }

        //NOTE(DavoSK): We dont want to init game in menu !
        if (strcmp(mission_name, "00menu") == 0) {
            return;
        }

        MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);
        for (auto bridge_name : forbidden_bridges) {
            auto bridge = (MafiaSDK::C_Bridge*)MafiaSDK::GetMission()->FindActorByName(bridge_name.c_str());
            if (bridge) {
                bridge->Shutdown(TRUE);
            }
        }

        mod_librg_connect();
    });

    MafiaSDK::C_Game_Hooks::HookLocalPlayerFallDown([&]() {
        modules::player::died();
        local_player.dead = true;
    });

    MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {

        delta_time = zpl_time_now() - last_time;
    
        if (!librg_is_connected(&network_context) && !modules::mainmenu::is_active)
            conncam::interpolate_cam(delta_time);

#ifdef OAKWOOD_DEBUG
        modules::debug::game_update(delta_time);
#endif

        librg_tick(&network_context);

        librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | ENTITY_INTERPOLATED), [](librg_ctx *ctx, librg_entity *entity) {
            switch (entity->type) {
            case TYPE_WEAPONDROP: {
            } break;

            case TYPE_PLAYER: {
                auto player = (mafia_player*)entity->user_data;
                if (player && player->ped && player->streamer_entity_id != local_player.entity_id) {
                    modules::player::game_tick(player, delta_time);
                }
            } break;

            case TYPE_VEHICLE: {
                auto vehicle = (mafia_vehicle*)entity->user_data;
                if (vehicle && vehicle->car) {
                    modules::vehicle::game_tick(vehicle, delta_time);
                }
            } break;
            }
        });

        modules::mainmenu::tick();
        last_time = zpl_time_now();
    });
}
