namespace effects {

    bool is_enabled = false;
    IDirect3DVertexBuffer9* vertex_buffer = nullptr;
    IDirect3DTexture9* back_buffer_texture = nullptr;
    
    ID3DXEffect* effect = nullptr;
    D3DXMATRIX shader_matrix;

    #define OURVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
    struct thisVertex {
        float x, y, z;
        DWORD color;
        float tu, tv;
    };

    inline void load(std::string effect_file) {

        if (effect != nullptr)
            effect->Release();

        LPD3DXBUFFER compilation_errors;
        if (FAILED(D3DXCreateEffectFromFile(global_device, effect_file.c_str(), NULL,
            NULL, D3DXSHADER_OPTIMIZATION_LEVEL3, NULL, &effect, &compilation_errors))) {
            
            if (compilation_errors != nullptr) {
                std::ofstream shader_log("shaderlog.txt");
                shader_log << (char*)compilation_errors->GetBufferPointer();
            }
            return;
        }

        D3DXMatrixIdentity(&shader_matrix);
        effect->SetMatrix("ShaderMatrix", &shader_matrix);
    }

    inline void init(IDirect3DDevice9* device) {

        u32 buffer_size = 4 * sizeof(thisVertex);
        if (FAILED(device->CreateVertexBuffer(buffer_size, D3DUSAGE_WRITEONLY, OURVERTEX, D3DPOOL_DEFAULT, &vertex_buffer, NULL)))
            return;

        thisVertex* vectices = nullptr;
        if (FAILED(vertex_buffer->Lock(0, buffer_size, (void**)&vectices, 0)))
            return;

        vectices[0].x = -1.0f;
        vectices[0].y = -1.0f;
        vectices[0].z = 0.0f;
        vectices[0].color = 0xffff0000;
        vectices[0].tu = 0.0;
        vectices[0].tv = 0.0;

        vectices[1].x = -1.0f;
        vectices[1].y = 1.0f;
        vectices[1].z = 0.0f;
        vectices[1].color = 0xff0000ff;
        vectices[1].tu = 1.0;
        vectices[1].tv = 0.0;

        vectices[2].x = 1.0f;
        vectices[2].y = -1.0f;
        vectices[2].z = 0.0f;
        vectices[2].color = 0xff00ff00;
        vectices[2].tu = 0.0;
        vectices[2].tv = 1.0;

        vectices[3].x = 1.0f;
        vectices[3].y = 1.0f;
        vectices[3].z = 0.0f;
        vectices[3].color = 0xffffffff;
        vectices[3].tu = 1.0;
        vectices[3].tv = 1.0;

        vertex_buffer->Unlock();
    }

    inline void device_lost() {
        if (back_buffer_texture) {
            back_buffer_texture->Release();
            back_buffer_texture = nullptr;
        }
        
        if (vertex_buffer) {
            vertex_buffer->Release();
            vertex_buffer = nullptr;
        }

        if (effect) {
            effect->Release();
            effect = nullptr;
        }
    }

    inline void device_reset(IDirect3DDevice9* device) {
        init(device);
    }

    inline void render(IDirect3DDevice9* device) {

        if(device && is_enabled && effect) {
            
            u32 passes;
            effect->Begin(&passes, 0);

            for (u32 i = 0; i < passes; i++) {

                effect->BeginPass(i);
                IDirect3DSurface9* back_buffer_surface = nullptr;
                device->GetRenderTarget(0, &back_buffer_surface);

                if (back_buffer_texture == nullptr) {
                    D3DSURFACE_DESC surf_desc;
                    back_buffer_surface->GetDesc(&surf_desc);
                    D3DXCreateTexture(device, surf_desc.Width, surf_desc.Height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET,
                        surf_desc.Format, D3DPOOL_DEFAULT, &back_buffer_texture);
                }

                LPDIRECT3DSURFACE9 back_buffer_texture_surface;
                back_buffer_texture->GetSurfaceLevel(0, &back_buffer_texture_surface);
                device->StretchRect(back_buffer_surface, NULL, back_buffer_texture_surface, NULL, D3DTEXF_NONE);

                effect->SetTexture("BackBufferTex", back_buffer_texture);
                effect->CommitChanges();

                device->SetFVF(OURVERTEX);
                device->SetStreamSource(0, vertex_buffer, 0, sizeof(thisVertex));
                device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
                back_buffer_surface->Release();

                effect->EndPass();
            }

            effect->End();
        }
    }    
}
