#pragma once

auto player_despawn(MafiaSDK::C_Human* player) -> void {
    
    if(player) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player);
        MafiaSDK::I3D_Frame* player_frame = *(MafiaSDK::I3D_Frame**)((DWORD)player + 0x68);
        __asm {
            mov eax, player_frame
            push eax
            mov ecx, [eax]
            call dword ptr ds : [ecx]
        }
    }
}
