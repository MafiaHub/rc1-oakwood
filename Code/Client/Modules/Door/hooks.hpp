//----------------------------------------------
//C_Door::SetState
//----------------------------------------------
typedef void(__thiscall *C_Door__SetState_t)(MafiaSDK::C_Door *_this, MafiaSDK::C_Door_Enum::States state, MafiaSDK::C_Actor *actor, BOOL unk1, BOOL unk2);
C_Door__SetState_t door_setstate_orignal = nullptr;
void __fastcall Door__SetState(MafiaSDK::C_Door* _this, DWORD edx, MafiaSDK::C_Door_Enum::States state, MafiaSDK::C_Actor *actor, BOOL unk1, BOOL unk2) {
	
	if (actor == player::get_local_ped()) {
		player::use_door(_this, state);
	}
}    

void init() {
	door_setstate_orignal = reinterpret_cast<C_Door__SetState_t>(
        DetourFunction((PBYTE)0x00439610, (PBYTE)&Door__SetState)
    );
}
