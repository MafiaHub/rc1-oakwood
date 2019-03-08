namespace imgui {
    inline void render() {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;

            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        //toggle input for imgui here temporary
        if (input::is_key_down(VK_F1))
            input::toggle_block_input();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    inline void init_style() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.GrabRounding = 0.f;
        style.WindowRounding = 0.f;
        style.ScrollbarRounding = 0.f;
        style.FrameRounding = 0.f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        auto io = ImGui::GetIO();
        io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 16);
        
        style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
        style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
    }

    inline void init(IDirect3DDevice9* device) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        init_style();
        ImGui_ImplWin32_Init(MafiaSDK::GetIGraph()->GetMainHWND());
        ImGui_ImplDX9_Init(device);
    }
}
