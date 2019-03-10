#pragma once
#include "Graphics/d3d9/CDirect3DDevice9Proxy.h"
#include "Graphics/d3d9/CDirect3D9Proxy.h"

namespace nameplates {
    inline void init(IDirect3DDevice9* device);
    inline void device_lost();
    inline void device_reset(IDirect3DDevice9* device);
    inline void render(IDirect3DDevice9* device);
}

namespace gamemap {
    inline void init(IDirect3DDevice9* device);
}

namespace imgui {
    inline void render();
    inline void init(IDirect3DDevice9* device);
    inline void device_reset(IDirect3DDevice9* device);
    inline void device_lost();
}

#include "graphics.hpp"
