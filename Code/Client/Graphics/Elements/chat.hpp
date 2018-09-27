#pragma once
namespace chat {

    std::vector<std::string> chat_messages;
    constexpr unsigned int VK_T = 0x54;
	KeyToggle key_chat_open(VK_T);
	KeyToggle key_chat_send(VK_RETURN);

    auto render() {

		if(!MafiaSDK::GetMission()->GetGame() ||
		   !MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()) return; 
        
		ImGui::Begin("Mafia Multiplayer - Chat", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::SetWindowSize(ImVec2(400, 300));
		ImGui::SetWindowPos(ImVec2(20, 20));
		
		ImGui::BeginChild("scrolling");
		if (!chat_messages.empty()) {
			for (auto message : chat_messages) {
				ImGui::TextUnformatted(message.c_str());
			}
		}
		
		ImGui::EndChild();

		if (!input::InputState.input_blocked && MafiaSDK::IsWindowFocused() && key_chat_open) {
			input::toggle_block_input();
		}

		if (input::InputState.input_blocked && MafiaSDK::IsWindowFocused()) {
			
			static char add_text[128] = "";
			ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("", add_text, IM_ARRAYSIZE(add_text));
			
			if (key_chat_send) {
				input::toggle_block_input();

				librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
					librg_data_wu8(&data, zpl_strlen(add_text));
					librg_data_wptr(&data, (void *)add_text, zpl_strlen(add_text));
				});

				strcpy(add_text, "");
			}
		}

		ImGui::SetScrollHere(1.0f);
		ImGui::End();
    }
}