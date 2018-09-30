#pragma once
namespace chat {

	struct ChatCommand {
		std::string command_name;
		std::function<void(std::vector<std::string>)> command_ptr;
	};

	std::vector<ChatCommand> chat_commands;
    std::vector<std::string> chat_messages;
    constexpr unsigned int VK_T = 0x54;
	KeyToggle key_chat_open(VK_T);
	KeyToggle key_chat_send(VK_RETURN);

	auto register_command(const std::string & name, std::function<void(std::vector<std::string>)> ptr) {
		if(ptr != nullptr) {
			chat_commands.push_back({name, ptr});
		}
	}

	auto get_vector_of_args(std::string command_str) {
		return split(command_str, " ");
	}

	auto parse_command(std::string command_str) {
		if(!command_str.empty()) {
			for(auto command : chat_commands) {
				if(command_str.find(command.command_name) != std::string::npos) {
					if(command.command_ptr != nullptr) 
						command.command_ptr(get_vector_of_args(command_str));
				}
			}
		}
	}

	auto init() {
		register_command("/q", [&](std::vector<std::string> args) {
			exit(0);
		});
	}

    auto render() {

		if(!MafiaSDK::GetMission()->GetGame() ||
		   !MafiaSDK::GetMission()->GetGame()->GetLocalPlayer()) return; 
        
		ImGuiStyle& style = ImGui::GetStyle();
		if (input::InputState.input_blocked && MafiaSDK::IsWindowFocused()) {
			style.Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24, 200);
		}
		else {
			style.Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24, 0);
		}


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
				
				if (strlen(add_text)) {
					librg_send(&network_context, NETWORK_SEND_CHAT_MSG, data, {
						librg_data_wu8(&data, zpl_strlen(add_text));
						librg_data_wptr(&data, (void *)add_text, zpl_strlen(add_text));
					});

					if (add_text[0] == '/')
						parse_command(add_text);

					strcpy(add_text, "");
				}

				input::toggle_block_input();
			}
		}

		ImGui::SetScrollHere(1.0f);
		ImGui::End();
    }
}