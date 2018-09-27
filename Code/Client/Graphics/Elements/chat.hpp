#pragma once
namespace chat {

    std::vector<std::string> chat_messages;
    constexpr unsigned int VK_T = 0x54;
	KeyToggle key_chat_open(VK_T);
	KeyToggle key_chat_send(VK_RETURN);

    auto render() {

        if(!MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()) return;

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

		if (!input::InputState.input_blocked && key_chat_open) {
			input::toggle_block_input();
		}

		if (input::InputState.input_blocked) {
			
			static char add_text[128] = "";
			ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("", add_text, IM_ARRAYSIZE(add_text));
			
			if (key_chat_send) {
				chat_messages.push_back("<" + GlobalConfig.username + "> " + add_text);
				input::toggle_block_input();
				strcpy(add_text, "");
			}
		}

		ImGui::SetScrollHere(1.0f);
		ImGui::End();
    }
}