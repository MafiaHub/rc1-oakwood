#pragma once

void switchClientState(int state) {
    clientActiveState = state;

    if (state == ClientState_Browser) {
        MafiaSDK::GetMission()->GetGame()->SetTrafficVisible(true);
        modules::mainmenu::init();
    }
}
