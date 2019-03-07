/* 
* Simple filtration of forbidden object types 
*/
using ObjTypes = MafiaSDK::C_Mission_Enum::ObjectTypes;
std::vector<ObjTypes> forbidden_objects = {
	ObjTypes::Car,
	ObjTypes::Dog,
	ObjTypes::Enemy,
	ObjTypes::Pumpar,
	ObjTypes::Trolley
};

MafiaSDK::C_Actor* Scene_CreateActor_Filter(MafiaSDK::C_Mission_Enum::ObjectTypes type, DWORD frame) {
	
	for(auto forbidden_type : forbidden_objects) {
		if(type == forbidden_type && frame != NULL) {
			//NOTE(DavoSK): We dont delete frame just set off
			MafiaSDK::I3D_Frame* frame_ex = (MafiaSDK::I3D_Frame*)frame;
			if(frame_ex) 
				frame_ex->SetOn(false);
			
			return nullptr;
		}
	}
	
	return MafiaSDK::GetMission()->CreateActor(type);;
}

DWORD filter_create_actor_back = 0x00544B07;
__declspec(naked) void Scene_CreateActor() {
	__asm {
		push edi
		push eax
		call Scene_CreateActor_Filter
		add esp, 0x8
		jmp filter_create_actor_back
	}
}

/*
* Draw distance hook
*/
float vd_min = 20.0f;
float vd_max = 450.0f;
float vd_value = GlobalConfig.view_distance > 0.0f ? GlobalConfig.view_distance : vd_max;

void __declspec(naked)VD_Hook_1_0_ENG() {
	_asm {
		ADD[ESP], 0x2;
		FLD DWORD PTR DS : [ESP + 0x0B8];	
		FLD DWORD PTR DS : vd_min;			
		FCOMiP ST, ST(1);						
		JAE finaly;								
		FLD DWORD PTR DS : vd_max;				
		FCOMiP ST, ST(1);						
		JBE finaly;								
		FFREE ST;				
		FLD DWORD PTR DS : vd_value;							
	finaly:
		FSTP DWORD PTR DS : [ESP + 0x0B8];
		MOV ECX, DWORD PTR SS : [ESP + 0x0B8];
		RETN;
	}
}

inline auto init() {
	// Scene
    MemoryPatcher::InstallJmpHook(0x00544AFF, (DWORD)&Scene_CreateActor);
    auto vd_value1 = vd_value - 50.0f;
    auto vd_value2 = vd_value - 20.0f;
    MemoryPatcher::PatchAddress(0x00402201 + 0x4, (BYTE*)&vd_value1, 4);
    MemoryPatcher::PatchAddress(0x00402209 + 0x4, (BYTE*)&vd_value2, 4);
    MemoryPatcher::InstallCallHook(0x0054192E, (DWORD)VD_Hook_1_0_ENG);
    MemoryPatcher::InstallNopPatch(0x0054192E + 0x5, 0x2);
}