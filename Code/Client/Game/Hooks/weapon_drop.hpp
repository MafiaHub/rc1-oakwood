#pragma once

namespace hooks
{
	char model[32];
	DWORD jump_back = 0x00580180;
	DWORD other_jmp = 0x0057FB78;
	DWORD jmp_back_capture = 0x0057FBA8;
	
	void copy_model(char* model_name) {
		sprintf(model, "%s", model_name);
	}

	__declspec(naked) void drop_capture_model() {
		__asm {
			lea     ebp, [eax + 24h]
			pushad
				push ebp
				call copy_model
				add esp, 0x4
			popad

			XOR eax, eax
			jmp jmp_back_capture
		}
	}

	__declspec(naked) void drop_in_weapon_init() {
		__asm
		{
			pushad
			    lea eax, model
				push eax
				push edi
				push ebp
				call drop_frame_init
				add esp, 0xC
			popad

			mov dword ptr ss : [esp + 0x20], eax 
			jmp jump_back
		}
	}

	__declspec(naked) void no_phys() { 
		__asm retn 4 
	}

	DWORD daco = 0x005A51A8;
	DWORD pickup_back = 0x594948;
	DWORD adddrcactor = 0x5A79A0;
	__declspec(naked) void weapon_pickup() {
		__asm {
			pushad
				push eax
				call drop_on_pickup
				add esp, 0x4
			popad

			push eax 
			mov eax, dword ptr ds : [0x63788C] 
			mov ecx, dword ptr ds : [eax + 0x24] 
			call adddrcactor
			jmp pickup_back
		}
	}
};

inline auto drop_init() -> void {
	MemoryPatcher::InstallJmpHook(0x0057FBA3, (DWORD)&hooks::drop_capture_model);
	MemoryPatcher::InstallJmpHook(0x00580176, (DWORD)&hooks::drop_in_weapon_init);
	MemoryPatcher::InstallJmpHook(0x00443650, (DWORD)&hooks::no_phys);
	MemoryPatcher::InstallJmpHook(0x00594923, (DWORD)&hooks::weapon_pickup);
}