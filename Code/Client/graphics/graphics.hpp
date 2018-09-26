#pragma once
namespace graphics {
	
	typedef IDirect3D9 *(WINAPI * d3dcreate9_t)(UINT);
	d3dcreate9_t d3dcreate9_original = nullptr;
	IDirect3DDevice9* global_device	= nullptr;
	
	
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	IDirect3D9* WINAPI d3dcreate9_hook(UINT SDKVersion) {
		IDirect3D9 *new_direct = d3dcreate9_original(SDKVersion);
		if (new_direct) {
			return new CDirect3D9Proxy(new_direct);
		}

		MessageBox(NULL, "Unable to create Direct3D9 interface.", "Fatal error", MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
		return NULL;
	}

	inline auto hook() -> void {

		while (!GetModuleHandle("d3d9.dll")) {
			Sleep(100);
		}

		d3dcreate9_original = (d3dcreate9_t)(DetourFunction(DetourFindFunction((char*)"d3d9.dll", (char*)"Direct3DCreate9"), (PBYTE)d3dcreate9_hook));
	}

	inline auto init(IDirect3DDevice9* device) -> void {

		global_device = device;	
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplWin32_Init(*(HWND*)(0x101C5458));
		ImGui_ImplDX9_Init(device);
		ImGui::StyleColorsDark();
	}

	inline auto device_lost(IDirect3DDevice9* device) -> void {

	}

	inline auto device_reset(IDirect3DDevice9* device) -> void {

	}

	inline auto world_to_screen(D3DXVECTOR3 input) -> D3DXVECTOR3 {

		D3DXVECTOR3 out;
		D3DVIEWPORT9 viewport;
		global_device->GetViewport(&viewport);
		D3DXMATRIX projection, view, world;
		global_device->GetTransform(D3DTS_VIEW, &view);
		D3DXMatrixIdentity(&world);
		global_device->GetTransform(D3DTS_PROJECTION, &projection);
		D3DXVec3Project(&out, &input, &viewport, &projection, &view, &world);
		return out;
	}

	/* 
	* Renders simple nicknames 
	* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
	*/
	inline auto render_nicknames() -> void {

		for (u32 i = 0; i < ctx.max_entities; i++) {

			librg_entity_t *entity = librg_entity_fetch(&ctx, i);
			if (!entity || entity->id == local_player.entity.id) continue;

			if (entity->type == TYPE_PLAYER && entity->user_data) {
				auto player = reinterpret_cast<mafia_player*>(entity->user_data);

				if (player->ped) {
					
					auto player_pos = player->ped->GetInterface()->neckFrame->GetInterface()->mPosition;
					auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.4f, player_pos.z });
					
					if (screen.z > 0.0f) {
						auto size = MafiaSDK::GetMission()->GetGame()->GetIndicators()->TextSize(player->name, 20.0f, 1, 0);
						
						//draw 8x more smoother ?
						for(int i = 0; i < 8; i++)
							MafiaSDK::GetMission()->GetGame()->GetIndicators()->OutText(player->name, screen.x - (size / 2), screen.y, size, 20.0f, 0xFFFFFFFF, 1, 1);
					}
				}
			}
		}
	}

	inline auto end_scene(IDirect3DDevice9* device) -> void {

		// Start the Dear ImGui frame
		ImGui::GetIO().MouseDrawCursor = input::InputState.input_blocked;
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if(input::InputState.input_blocked) {
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                         

			ImGui::Text("This is some useful text.");             
			ImGui::Checkbox("Demo Window", &show_demo_window);     
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);              
			ImGui::ColorEdit3("clear color", (float*)&clear_color);

			if (ImGui::Button("Button"))                       
				counter++;

			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}