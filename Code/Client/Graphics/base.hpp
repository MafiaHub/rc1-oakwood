#pragma once

IDirect3DDevice9* global_device	= nullptr;

#include "Graphics/d3d9/CDirect3DDevice9Proxy.h"
#include "Graphics/d3d9/CDirect3D9Proxy.h"

#include "utils.hpp"

namespace graphics {
	
	typedef IDirect3D9 *(WINAPI * d3dcreate9_t)(UINT);
	d3dcreate9_t d3dcreate9_original = nullptr;
	
	IDirect3D9* WINAPI d3dcreate9_hook(UINT SDKVersion) {
		IDirect3D9 *new_direct = d3dcreate9_original(SDKVersion);
		if (new_direct) {
			return new CDirect3D9Proxy(new_direct);
		}

		MessageBox(NULL, "Unable to create Direct3D9 interface.", "Fatal error", MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
		return NULL;
	}

	inline auto hook() {

		while (!GetModuleHandle("d3d9.dll")) {
			Sleep(100);
		}

		d3dcreate9_original = (d3dcreate9_t)(DetourFunction(DetourFindFunction((char*)"d3d9.dll", (char*)"Direct3DCreate9"), (PBYTE)d3dcreate9_hook));
	}

	inline auto setup_imgui(IDirect3DDevice9* device) -> void {

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplWin32_Init(MafiaSDK::GetMainWindow());
		ImGui_ImplDX9_Init(device);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("ChatFont.ttf", 20);
		io.FontAllowUserScaling = true;

		ImGui::StyleColorsDark();

		ImColor mainColor = ImColor(150, 0, 0, 70);
		ImColor bodyColor = ImColor(24, 24, 24, 50);
		ImColor fontColor = ImColor(255, 255, 255, 255);

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
		ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
		ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
		ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
		ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(8, 8);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(8, 4);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 3.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.CurveTessellationTol = 1.25f;
		style.Colors[ImGuiCol_Text] = fontColor;
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = bodyColor;
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(.0f, .0f, .0f, .0f);
		style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
		style.Colors[ImGuiCol_Border] = mainColor;
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = frameBgColor;
		style.Colors[ImGuiCol_FrameBgHovered] = mainColorHovered;
		style.Colors[ImGuiCol_FrameBgActive] = mainColorActive;
		style.Colors[ImGuiCol_TitleBg] = mainColor;
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = mainColor;
		style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
		style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_CheckMark] = mainColor;
		style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
		style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_Button] = mainColor;
		style.Colors[ImGuiCol_ButtonHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ButtonActive] = mainColorActive;
		style.Colors[ImGuiCol_Header] = mainColor;
		style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
		style.Colors[ImGuiCol_HeaderActive] = mainColorActive;
		style.Colors[ImGuiCol_Column] = mainColor;
		style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ColumnActive] = mainColorActive;
		style.Colors[ImGuiCol_ResizeGrip] = mainColor;
		style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
		style.Colors[ImGuiCol_PlotLines] = mainColor;
		style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
		style.Colors[ImGuiCol_PlotHistogram] = mainColor;
		style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		chat::init();
	} 

	inline auto init(IDirect3DDevice9* device) -> void {

		global_device = device;	
		setup_imgui(device);
	}

	inline auto device_lost(IDirect3DDevice9* device) -> void {

	}

	inline auto device_reset(IDirect3DDevice9* device) -> void {

	}

	inline auto end_scene(IDirect3DDevice9* device) -> void {

		ImGui::GetIO().MouseDrawCursor = input::InputState.input_blocked;
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		chat::render();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	
		D3DLIGHT9 light;
		D3DMATERIAL9 material;

		if (MafiaSDK::GetMission()->GetGame() && MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()) {

			auto pos = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()->GetInterface()->humanObject.entity.position;
			ZeroMemory(&light, sizeof(light));
			light.Type = D3DLIGHT_SPOT;    // make the light type 'spot light'
			light.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			light.Position = D3DXVECTOR3(pos.x, pos.y, pos.z);
			light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			light.Range = 100.0f;    // a range of 100
			light.Attenuation0 = 0.0f;    // no constant inverse attenuation
			light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
			light.Attenuation2 = 0.0f;    // no square inverse attenuation
			light.Phi = D3DXToRadian(40.0f);    // set the outer cone to 30 degrees
			light.Theta = D3DXToRadian(20.0f);    // set the inner cone to 10 degrees
			light.Falloff = 1.0f;    // use the typical falloff

			global_device->SetLight(0, &light);
			global_device->LightEnable(0, TRUE);

			ZeroMemory(&material, sizeof(D3DMATERIAL9));
			material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			global_device->SetMaterial(&material);
		}
	}
}