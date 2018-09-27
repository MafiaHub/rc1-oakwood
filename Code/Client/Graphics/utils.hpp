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