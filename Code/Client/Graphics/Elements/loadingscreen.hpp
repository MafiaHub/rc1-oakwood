#pragma once
namespace loadingscreen {
    constexpr float     texture_width               = 1920.0f;
    constexpr float     texture_height              = 1080.0f;
    float               current_loading_progress    = 0.0f;
    IDirect3DTexture9*  loadingscreen_texture       = nullptr;

    /* 
    * To get progress of loading we hooking progress bar SetValue
    */
    typedef int(__thiscall* GM_ProgressBar_SetVal_t)(DWORD _this, float progress);
    GM_ProgressBar_SetVal_t set_val_original = nullptr;

    int __fastcall SetVal_Hook(DWORD _this, DWORD edx, float progress) {
        current_loading_progress = progress;
        return set_val_original(_this, progress);
    }

	inline void init(IDirect3DDevice9* device) {
        if (device) {
           	D3DXCreateTextureFromFileExA(
           		device, 
           		(GlobalConfig.localpath + "\\files\\oakwood.png").c_str(),
                texture_width,
                texture_height,
           		1, 0, 
           		D3DFMT_X8R8G8B8,
				D3DPOOL_DEFAULT, 
				D3DX_FILTER_LINEAR,
				D3DX_FILTER_LINEAR, 
				0x1000ff00,
				NULL, NULL, &loadingscreen_texture
			);
        }

        if (set_val_original == nullptr) {
            set_val_original = reinterpret_cast<GM_ProgressBar_SetVal_t>(
                DetourFunction((PBYTE)0x005DF350, (PBYTE)SetVal_Hook)
            );
        }
    }

    void changeTex(std::string path)
    {
        if (global_device)
        {
            if (loadingscreen_texture) {
                loadingscreen_texture->Release();
                loadingscreen_texture = nullptr;
            }

            D3DXCreateTextureFromFileExA(
                global_device,
                path.c_str(),
                texture_width,
                texture_height,
                1, 0,
                D3DFMT_X8R8G8B8,
                D3DPOOL_DEFAULT,
                D3DX_FILTER_LINEAR,
                D3DX_FILTER_LINEAR,
                0x1000ff00,
                NULL, NULL, &loadingscreen_texture
            );
        }
    }

    inline void render(IDirect3DDevice9* device) {
        if (device != nullptr && loadingscreen_texture != nullptr 
            && current_loading_progress > 0.00f && current_loading_progress < 1.0f) {

            graphics::main_sprite->Begin(NULL);
            float screen_x = (float)MafiaSDK::GetIGraph()->Scrn_sx();
            float screen_y = (float)MafiaSDK::GetIGraph()->Scrn_sy();
            float scale_x = screen_y / texture_height;
            
            D3DXMATRIX loading_screen_transform;
            D3DXVECTOR2 scale = { scale_x, scale_x };
            D3DXVECTOR2 position = { ((float)MafiaSDK::GetIGraph()->Scrn_sx() / 2.0f) - ((texture_width / 2.0f) * scale_x), 0.0f };

            D3DXMatrixTransformation2D(&loading_screen_transform, NULL, 0.0, &scale, NULL, NULL, &position);
            graphics::main_sprite->SetTransform(&loading_screen_transform);
            graphics::main_sprite->Draw(loadingscreen_texture, NULL, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
            graphics::main_sprite->End();

            float loading_bar_width = 420.0f;
            constexpr float loading_bar_height = 12.0f;

            graphics::draw_box(device, (screen_x / 2.0f) - (loading_bar_width / 2.0f) - 9, screen_y - loading_bar_height - 48, 0, loading_bar_width + 8, loading_bar_height + 8, 0xFF2A2A2A);
            graphics::draw_box(device, (screen_x / 2.0f) - (loading_bar_width / 2.0f) - 5, screen_y - loading_bar_height - 44, 0, loading_bar_width * current_loading_progress, loading_bar_height, 0xFFFF0000);

            //graphics::draw_box(device, 0.0f, screen_y - loading_bar_height, 0.0f, screen_x * current_loading_progress, loading_bar_height, 0xFFFF0000);
        }
    }

	/*
    * When device lost 
    * Free all needed elements
    */
	inline void device_lost() {
        if (loadingscreen_texture) {
            loadingscreen_texture->Release();
            loadingscreen_texture = nullptr;
        }
	}

	/* 
    * Recreated all freed elements after device is reseted
    */
    inline void device_reset(IDirect3DDevice9* device) {
        if (device) {
            init(device);
        }
    }
}
