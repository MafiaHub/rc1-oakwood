namespace graphics
{
    typedef IDirect3D9 *(WINAPI * d3dcreate9_t)(UINT);
    d3dcreate9_t d3dcreate9_original = nullptr;
    ID3DXSprite* main_sprite = nullptr;
    IDirect3DStateBlock9* state_block = nullptr;

    IDirect3D9* WINAPI d3dcreate9_hook(UINT SDKVersion) {
        IDirect3D9 *new_direct = d3dcreate9_original(SDKVersion);
        if (new_direct)
            return new CDirect3D9Proxy(new_direct);
     
        MessageBox(NULL, "Unable to create Direct3D9 interface.", "Fatal error", MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 0);
        return NULL;
    }

    inline auto hook() {
        d3dcreate9_original = (d3dcreate9_t)(DetourFunction(DetourFindFunction((char*)"d3d9.dll", (char*)"Direct3DCreate9"), (PBYTE)d3dcreate9_hook));
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

    inline auto create_font(IDirect3DDevice9* device, const char* font_name, unsigned int size, bool bold) -> ID3DXFont* {

        ID3DXFont* to_create = nullptr;

        if (FAILED(D3DXCreateFont(device, size, 0, (bold ? FW_BOLD : FW_NORMAL), 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, (DEFAULT_PITCH | FF_DONTCARE), font_name, &to_create))) {
            MessageBox(NULL, "Unable to create font", __FILE__, MB_OK);
            return nullptr;
        }

        return to_create;
    }

    inline auto get_text_size(ID3DXFont *font, const char *text)-> SIZE {
        if (font) {
            HDC dc = font->GetDC();
            SIZE size;
            GetTextExtentPoint32(dc, text, strlen(text), &size);
            return size;
        }
        return {};
    }

    struct Vertex2D {
        float x, y, z, rhw;
        DWORD color;
    };

    inline void draw_box(IDirect3DDevice9* device, float x, float y, float width, float height, DWORD color) {
        if (device) {
            const Vertex2D rect[] = {
                { x,			y,			0.0f, 1.0f,	color },
                { x + width,	y,			0.0f, 1.0f,	color },
                { x,			y + height, 0.0f, 1.0f,	color },
                { x + width,	y + height, 0.0f, 1.0f,	color },
            };

            device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            device->SetPixelShader(NULL);
            device->SetVertexShader(NULL);
            device->SetRenderState(D3DRS_ZENABLE, FALSE);
            device->SetTexture(0, NULL);
            device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &rect, sizeof(Vertex2D));
        }
    }

    /*
    *  Just draw some text boi
    */
    inline void draw_text(ID3DXFont* font, const char* text, float x, float y, float scale, unsigned long color, bool shadow) {
        if (text == nullptr || main_sprite == nullptr || font == nullptr) return;

        D3DXVECTOR2 scaling(scale, scale);
        D3DXMATRIX matrix;
        D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, &scaling, NULL, 0.0f, NULL);

        main_sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
        main_sprite->SetTransform(&matrix);

        if (shadow) {
            RECT shadow_rect;
            SetRect(&shadow_rect, x * (1.0f / scale) + 1, y * (1.0f / scale) + 1, 0, 0);
            font->DrawTextA(main_sprite, text, -1, &shadow_rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
        }

        RECT rect;
        SetRect(&rect, x * (1.0f / scale), y * (1.0f / scale), 0, 0);
        font->DrawTextA(main_sprite, text, -1, &rect, DT_NOCLIP, color);

        main_sprite->End();
    }

    inline D3DSURFACE_DESC get_backbuffer_desc(IDirect3DDevice9* device) {
        D3DSURFACE_DESC back_buffer_desc;
        if (device) {
            IDirect3DSurface9* back_buffer = nullptr;
            device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
            back_buffer->GetDesc(&back_buffer_desc);
            back_buffer->Release();
        }
        return back_buffer_desc;
    }

    inline auto init_main_sprite(IDirect3DDevice9* device) -> void {
        if (FAILED(D3DXCreateSprite(device, &main_sprite))) {
            MessageBox(NULL, "Unable to create sprite for drawing nameplates", "nameplates.hpp", MB_OK);
            return;
        }
    }

    inline auto init(IDirect3DDevice9* device) -> void {

        global_device = device;
        init_main_sprite(device);

        nameplates::init(device);
        input::hook_window();
        imgui::init(device);
    }

    inline auto device_lost() -> void {
        if (global_device) {
            global_device->Release();
            global_device = nullptr;
        }

        if (main_sprite) {
            main_sprite->Release();
            main_sprite = nullptr;
        }

        nameplates::device_lost();
        imgui::device_lost();
    }

    inline auto device_reset(IDirect3DDevice9* device) -> void {

        global_device = device;
        init_main_sprite(device);
        nameplates::device_reset(device);
        imgui::device_reset(device);

        if (input::InputState.input_blocked)
            input::block_input(false);
    }

    inline auto end_scene(IDirect3DDevice9* device) -> void {

        if (device && global_device) {
            device->CreateStateBlock(D3DSBT_ALL, &state_block);

            IDirect3DDevice9_SetVertexShader(device, NULL);
            IDirect3DDevice9_SetFVF(device, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
            IDirect3DDevice9_SetRenderState(device, D3DRS_ZENABLE, D3DZB_FALSE);
            IDirect3DDevice9_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
            IDirect3DDevice9_SetRenderState(device, D3DRS_LIGHTING, FALSE);

            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
            IDirect3DDevice9_SetRenderState(device, D3DRS_ALPHABLENDENABLE, TRUE);

            IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            IDirect3DDevice9_SetPixelShader(device, NULL);

            nameplates::render(device);
            imgui::render();

            if (state_block) {
                state_block->Apply();
                state_block->Release();
                state_block = nullptr;
            }
        }
    }
}