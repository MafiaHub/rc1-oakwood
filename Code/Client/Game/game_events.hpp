#pragma once
u32 transition_idx = 0;
f64 last_time = 0.0f;
f64 passed_time = 2.0f;

std::vector<std::string> forbidden_bridges = {
    "LLsklap01",
    "sklapx01"};

#include "connecting_cam.hpp"
#include "resource.h"

f64 delta_time = 0.0f;
input::KeyToggle key_conn_escape(VK_ESCAPE);

auto mod_bind_events()
{
    modules::misc::init();
    modules::player::init();
    modules::vehicle::init();
    modules::door::init();
    modules::weapondrop::init();

#ifdef OAKWOOD_DEBUG
    modules::debug::init();
#endif

    MafiaSDK::C_Game_Hooks::HookOnGameInit([&]() {
        //TODO(DavoSK): Move it to sdk, force enable game map
        *(BOOL *)(0x006C406C) = true;

        auto mission_name = MafiaSDK::GetCurrentMissionName();

        SendMessageW((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), WM_SETTEXT, 0, (LPARAM)"Mafia: Oakwood");

        HICON hIcon = (HICON)LoadImageA(NULL, "files\\Oakwood.ico", 1, NULL, NULL, LR_LOADFROMFILE | LR_LOADTRANSPARENT);
        if (hIcon)
        {
            SendMessage((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessage((HWND)MafiaSDK::GetIGraph()->GetMainHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }
        else
        {
            printf("Failed to set icon.\n");
        }

        // Note: tutorial map is reserved for main browser state
        if (strcmp(mission_name, "tutorial") == 0 &&
            ((clientActiveState != ClientState_Infobox)
                && (clientActiveState != ClientState_PasswordPrompt)
                && (clientActiveState != ClientState_Downloading)))
        {
            switchClientState(ClientState_Browser);
            return;
        }

        //NOTE(DavoSK): We dont want to init game in menu !
        if (strcmp(mission_name, "00menu") == 0)
        {
            return;
        }

        MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(false);
        for (auto bridge_name : forbidden_bridges)
        {
            auto bridge = (MafiaSDK::C_Bridge *)MafiaSDK::GetMission()->FindActorByName(bridge_name.c_str());
            if (bridge)
            {
                bridge->Shutdown(TRUE);
            }
        }
    });

    MafiaSDK::C_Game_Hooks::HookLocalPlayerFallDown([&]() {
        modules::player::died(modules::player::get_local_ped(), 1, 0, 0);
        local_player.dead = true;
    });

    MafiaSDK::C_Game_Hooks::HookOnGameTick([&]() {
        delta_time = zpl_time_now() - last_time;

        switch (clientActiveState)
        {
        case ClientState_Infobox:
        case ClientState_PasswordPrompt:
        case ClientState_Downloading:
        case ClientState_Browser:
        {
            auto game = MafiaSDK::GetMission()->GetGame();
            if (game)
            {
                auto cam = game->GetCamera();
                if (cam)
                {
                    cam->LockAt(modules::mainmenu::camera_pos, modules::mainmenu::camera_dir);
                }
            }
            return;
        }

        case ClientState_Connecting:
            conncam::interpolate_cam(delta_time);
            librg_tick(&network_context);
            if (librg_is_connected(&network_context))
            {
                switchClientState(ClientState_Connected);
            }
            last_time = zpl_time_now();

            if (key_conn_escape)
            {
                MafiaSDK::GetMission()->MapLoad("tutorial");
                switchClientState(ClientState_Browser);
            }

            return;

        case ClientState_Connected:
            librg_tick(&network_context);
            break;
        }

#ifdef OAKWOOD_DEBUG
        modules::debug::game_update(delta_time);
#endif

        librg_entity_iterate(&network_context, (LIBRG_ENTITY_ALIVE | ENTITY_INTERPOLATED), [](librg_ctx *ctx, librg_entity *entity) {
            switch (entity->type)
            {
            case TYPE_WEAPONDROP:
            {
            }
            break;

            case TYPE_PLAYER:
            {
                auto player = (mafia_player *)entity->user_data;
                if (player && player->ped && player->streamer_entity_id != local_player.entity_id)
                {
                    modules::player::game_tick(player, delta_time);
                }
            }
            break;

            case TYPE_VEHICLE:
            {
                auto vehicle = (mafia_vehicle *)entity->user_data;
                if (vehicle && vehicle->car)
                {
                    modules::vehicle::game_tick(vehicle, delta_time);
                }
            }
            break;
            }
        });

        //NOTE(DavoSK): safe vehicle removing, dont delete them when someone is entering / leaving
        for (auto car_to_remove : car_delte_queue)
        {
            if (car_to_remove != nullptr)
            {
                bool do_remove = true;
                for (int i = 0; i < 4; i++)
                {
                    auto car_actor_seat = (MafiaSDK::C_Human *)car_to_remove->GetOwner(i);
                    if (car_actor_seat != nullptr)
                    {
                        if (car_actor_seat->GetInterface()->carLeavingOrEntering != nullptr)
                        {
                            do_remove = false;
                            break;
                        }
                    }
                }

                if (do_remove)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        auto car_actor_seat = (MafiaSDK::C_Human *)car_to_remove->GetOwner(i);
                        if (car_actor_seat != nullptr)
                        {
                            car_actor_seat->Intern_FromCar();
                            auto mafia_ent = modules::player::get_player_from_base((void *)car_actor_seat);
                            if (mafia_ent && mafia_ent->user_data)
                            {
                                auto player = (mafia_player *)mafia_ent->user_data;
                                player->vehicle_id = -1;
                            }
                        }
                    }

                    MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(car_to_remove);
                }
            }
        }

        // Spectator camera
        if (local_player.spec_id != -1)
        {
            zpl_printf("spec_id: %d \n", local_player.spec_id);

            librg_entity *ent = librg_entity_fetch(&network_context, local_player.spec_id);

            if (ent || local_player.spec_id != local_player.last_spec_id)
                cam_set_target(ent);

            if (!ent)
            {
                local_player.last_spec_id = local_player.spec_id;
            }
        }

        last_time = zpl_time_now();
    });
}
