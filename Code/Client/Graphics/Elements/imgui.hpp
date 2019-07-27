namespace imgui {

    #define CHECK_MENU_INPUT(func, ID) do {\
        bool state = modules::func::check_input();\
        if (state && menuActiveState != Menu_DebugMode) menuActiveState = ID;\
    } while (0);

    /*
        auto mafia_text         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        auto mafia_color        = ImVec4(0.533f, 0.0f, 0.0f, 1.00f);
        auto mafia_color_hover  = ImVec4(0.933f, 0.0f, 0.0f, 1.00f);
        auto mafia_color_active = ImVec4(0.733f, 0.0f, 0.0f, 1.00f);
        auto mafia_color_bg     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    */

    inline void render() {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiStyle& style = ImGui::GetStyle();
        if (input::InputState.input_blocked) {
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.75f);
        }
        else {
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.50f);
        }

        if (clientActiveState == ClientState_Connected) {
            CHECK_MENU_INPUT(chat, Menu_Chat);
            CHECK_MENU_INPUT(pausemenu, Menu_Pause);

#ifdef OAKWOOD_DEBUG
            CHECK_MENU_INPUT(debug, Menu_DebugMode);
#endif

            switch (menuActiveState) {
            case Menu_Chat: {
                modules::chat::render();
            } break;

            case Menu_Pause: {
                modules::pausemenu::render();
            } break;

#ifdef OAKWOOD_DEBUG
            case Menu_DebugMode: {
                modules::debug::render();
            } break;
#endif            
            }

            modules::playerlist::render();
        }

        /**/ if (clientActiveState == ClientState_Browser) {
            modules::mainmenu::render();
        }
        else if (clientActiveState == ClientState_Infobox) {
            modules::infobox::render();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    auto GetCustomGlyphRanges() -> const ImWchar * {
        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0100, 0x017F, // Latin Extended-A
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0,
        };
        return &ranges[0];
    }

    inline void init_style() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.GrabRounding = 0.f;
        style.WindowRounding = 0.f;
        style.ScrollbarRounding = 0.f;
        style.FrameRounding = 0.f;
        style.TabRounding = 0.f;
        style.TabBorderSize = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.533f, 0.0f, 0.0f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.533f, 0.0f, 0.0f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        auto io = ImGui::GetIO();
        io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 16, NULL, GetCustomGlyphRanges());
        io.FontAllowUserScaling = true;
    }

    inline void init(IDirect3DDevice9* device) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        init_style();
        ImGui_ImplWin32_Init(MafiaSDK::GetIGraph()->GetMainHWND());
        ImGui_ImplDX9_Init(device);

        modules::chat::init();
    }

    inline void device_reset(IDirect3DDevice9* device) {
        init(device);
    }

    inline void device_lost() {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}
