/* 
* Simple filtration of forbidden object types 
*/
using ObjTypes = MafiaSDK::C_Mission_Enum::ObjectTypes;
std::vector<ObjTypes> forbidden_objects = {
	ObjTypes::Car,
	ObjTypes::Dog,
	ObjTypes::Enemy,
	ObjTypes::Pumpar,
	ObjTypes::Trolley,
    ObjTypes::Player
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

inline auto init() {
	// Scene
    MemoryPatcher::InstallJmpHook(0x00544AFF, (DWORD)&Scene_CreateActor);
}
