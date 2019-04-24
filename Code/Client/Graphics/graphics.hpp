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

    inline void draw_box(IDirect3DDevice9* device, float x, float y, float z, float width, float height, DWORD color) {
        if (device) {
            const Vertex2D rect[] = {
                { x,			y,			z, 1.0f,	color },
                { x + width,	y,			z, 1.0f,	color },
                { x,			y + height, z, 1.0f,	color },
                { x + width,	y + height, z, 1.0f,	color },
            };

            device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            device->SetPixelShader(NULL);
            device->SetVertexShader(NULL);
            device->SetRenderState(D3DRS_ZENABLE, TRUE);
            device->SetTexture(0, NULL);
            device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &rect, sizeof(Vertex2D));
        }
    }

    /*
    * Draw 2D textured quad as sprite with z buffer enabled
    *
    */
    inline void draw_texture(IDirect3DDevice9* device, IDirect3DTexture9* texture, float x, float y, float z, int w, int h, unsigned short alpha) {


        LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
        device->CreateStateBlock(D3DSBT_ALL, &pStateBlock);
        pStateBlock->Capture();

        struct texturedVertex {
            float x, y, z;
            float rhw;
            DWORD color;
            float tu, tv;
        };
        
        DWORD color = (alpha << 24);
        texturedVertex g_square_vertices[] = {
            { (float)x, (float)y, z, 1.0f, color, 0.0f, 0.0f },
            { (float)(x + w), (float)y, z, 1.0f, color, 1.0f, 0.0f },
            { (float)x, (float)(y + h), z, 1.0f, color, 0.0f, 1.0f },
            { (float)(x + w), (float)(y + h), z, 1.0f, color, 1.0f, 1.0f }
        };

        device->SetRenderState(D3DRS_ZENABLE, TRUE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        device->SetRenderState(D3DRS_ALPHAREF, 0x08);
        device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
        device->SetRenderState(D3DRS_CLIPPING, TRUE);
        device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
        device->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
        device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_FOGENABLE, FALSE);

        //--- 
        device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        //------
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

        //------ important for rendering as sprite---
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        device->SetRenderState(D3DRS_WRAP0, 0);
       
        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
        device->SetTexture(0, texture);
        device->SetPixelShader(NULL);
        device->SetVertexShader(NULL);
        device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &g_square_vertices, sizeof(texturedVertex));

        if (pStateBlock) {
            pStateBlock->Apply();
            pStateBlock->Release();
            pStateBlock = nullptr;
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

    inline void draw_text_ex(ID3DXFont* font, const char* text, float x, float y, float z, float scale, unsigned long color, bool shadow) {
        if (text == nullptr || main_sprite == nullptr || font == nullptr) return;

        D3DXVECTOR3 scaling(scale, scale, scale);
        D3DXVECTOR3 transform(x, y, z);
        D3DXMATRIX matrix;
        D3DXMatrixTransformation(&matrix, NULL, NULL, &scaling, NULL, NULL, &transform);

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
        loadingscreen::init(device);
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
        loadingscreen::device_lost();
    }

    inline auto device_reset(IDirect3DDevice9* device) -> void {
        
        global_device = device;
        init_main_sprite(device);
        nameplates::device_reset(device);
        imgui::device_reset(device);
        loadingscreen::device_reset(device);

        //NOTE(DavoSK): After refocusing restore input state
        input::block_input(input::InputState.input_blocked);
    }

    inline auto end_scene(IDirect3DDevice9* device) -> void {

        if (device && global_device) {
            device->CreateStateBlock(D3DSBT_ALL, &state_block);

           /* IDirect3DDevice9_SetVertexShader(device, NULL);
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
            IDirect3DDevice9_SetPixelShader(device, NULL);*/

            nameplates::render(device);
            imgui::render();
            loadingscreen::render(device);

            if (state_block) {
                state_block->Apply();
                state_block->Release();
                state_block = nullptr;
            }
        }
    }
}
