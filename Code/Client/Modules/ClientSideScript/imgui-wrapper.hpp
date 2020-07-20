// General purpose - Wrapping Dear ImGui for usage with AngelScript

#pragma region Begin Functions
bool imGuiBegin(std::string name, bool p_open = false, int flags = 0)
{
    return ImGui::Begin(name.c_str(), &p_open, flags);
}

bool imGuiBeginChild(std::string id, float size_x = 0, float size_y = 0, bool border = false, int flags = 0)
{
    return ImGui::BeginChild(id.c_str(), ImVec2(size_x, size_y), border, flags);
}

bool imGuiBeginChildFrame(float size_x = 0, float size_y = 0, int flags = 0)
{
    return ImGui::BeginChildFrame(0, ImVec2(size_x, size_y), flags);
}
#pragma endregion

#pragma region End Functions
void imGuiEnd()
{
    ImGui::End();
}

void imGuiEndChild()
{
    ImGui::EndChild();
}

void imGuiEndChildFrame()
{
    ImGui::EndChildFrame();
}
#pragma endregion
