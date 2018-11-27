#pragma once
auto player_despawn(MafiaSDK::C_Human* player) -> void {
    if (player) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player);
    }
}
