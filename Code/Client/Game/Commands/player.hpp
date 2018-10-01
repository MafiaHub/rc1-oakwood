#pragma once

auto player_despawn(MafiaSDK::C_Player* player) -> void {
    
    if(player) {
        MafiaSDK::GetMission()->GetGame()->RemoveTemporaryActor(player);
		auto player_frame = *(DWORD*)((DWORD)player + 0x68);
		
		if (player_frame) {
			__asm {
				mov eax, player_frame
				push eax
				mov ecx, [eax]
				call dword ptr ds : [ecx]
			}
		}
    }
}