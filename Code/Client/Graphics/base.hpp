#pragma once

IDirect3DDevice9* global_device	= nullptr;

#include "Graphics/d3d9/CDirect3DDevice9Proxy.h"
#include "Graphics/d3d9/CDirect3D9Proxy.h"
#include "utils.hpp"

namespace graphics {
    
    typedef IDirect3D9 *(WINAPI * d3dcreate9_t)(UINT);
    d3dcreate9_t d3dcreate9_original = nullptr;
    
    IDirect3D9* WINAPI d3dcreate9_hook(UINT SDKVersion) {
        IDirect3D9 *new_direct = d3dcreate9_original(SDKVersion);
        if (new_direct) {
            return new CDirect3D9Proxy(new_direct);
        }

        MessageBox(NULL, "Unable to create Direct3D9 interface.", "Fatal error", MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 0);
        return NULL;
    }

    inline auto hook() {

        while (!GetModuleHandle("d3d9.dll")) {
            Sleep(100);
        }

        d3dcreate9_original = (d3dcreate9_t)(DetourFunction(DetourFindFunction((char*)"d3d9.dll", (char*)"Direct3DCreate9"), (PBYTE)d3dcreate9_hook));
    }

    inline D3DSURFACE_DESC get_backbuffer_desc(IDirect3DDevice9* device) {
        IDirect3DSurface9* back_buffer = nullptr;
        device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);

        D3DSURFACE_DESC back_buffer_desc;
        back_buffer->GetDesc(&back_buffer_desc);
        return back_buffer_desc;
    }

    inline auto init(IDirect3DDevice9* device) -> void {
        global_device = device;
        effects::init(device);
        cef::init(device);
        chat::init(device);
    }

    inline auto device_lost(IDirect3DDevice9* device) -> void {

    }

    inline auto device_reset(IDirect3DDevice9* device) -> void {
        global_device = device;
        effects::reset(device);
    }

    inline auto end_scene(IDirect3DDevice9* device) -> void {

        IDirect3DStateBlock9* pStateBlock = NULL;
        device->CreateStateBlock(D3DSBT_ALL, &pStateBlock);

        IDirect3DDevice9_SetVertexShader(device, NULL);
        IDirect3DDevice9_SetFVF(device, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        IDirect3DDevice9_SetRenderState(device, D3DRS_ZENABLE, D3DZB_FALSE);
        IDirect3DDevice9_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
        IDirect3DDevice9_SetRenderState(device, D3DRS_LIGHTING, FALSE);

        /* Enable color modulation by diffuse color */
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        /* Enable alpha modulation by diffuse alpha */
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        IDirect3DDevice9_SetRenderState(device, D3DRS_ALPHABLENDENABLE, TRUE);

        /* Disable second texture stage, since we're done */
        IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        IDirect3DDevice9_SetPixelShader(device, NULL);

        chat::update();
        cef::tick();
        cef::render_browsers();
        effects::render();
        
        pStateBlock->Apply();
        pStateBlock->Release();
    }
}
