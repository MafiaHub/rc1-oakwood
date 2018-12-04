#pragma once
auto player_despawn(MafiaSDK::C_Human* player) -> void {
    if (player) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player);
        DWORD frame = *(DWORD*)(player + 0x68);
        if (frame) {
            __asm {
                mov eax, frame
                push eax
                mov ecx, [eax]
                call dword ptr ds : [ecx]
            }
        }
    }
}
