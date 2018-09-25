#pragma once
namespace graphics {
	
	#define OURQUAD (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
	struct QuadVertex {
		float x, y, z;
		float rhw;
		DWORD color;
		float tu, tv;
	};

	IDirect3DVertexBuffer8* vertex_buffer	= nullptr;
	IDirect3DDevice8* global_device			= nullptr;

	inline auto init(IDirect3DDevice8* device) -> void {

		global_device = device;
		mod_log("DirectX Hooked !");

		if (FAILED(device->CreateVertexBuffer(6 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY, OURQUAD, D3DPOOL_MANAGED, &vertex_buffer))) {
			MessageBoxA(NULL, "LHMP", "Graphics - unable to create vertex buffer !", MB_OK);
		}
	}

	DWORD				m_dwRenderTextureBlock;
	DWORD				m_dwRenderTextureBlockDeposit;

	void SetRenderTextureStateBlock()
	{
		for (int i = 0; i < 2; i++) {
			global_device->BeginStateBlock();

			global_device->SetRenderState(D3DRS_ZENABLE, TRUE);
			global_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			global_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			global_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			global_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			global_device->SetRenderState(D3DRS_ALPHAREF, 0x08);
			global_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			global_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			global_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			global_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
			global_device->SetRenderState(D3DRS_CLIPPING, TRUE);
			global_device->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
			global_device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
			global_device->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
			global_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
			global_device->SetRenderState(D3DRS_FOGENABLE, FALSE);

			//--- 
			global_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			global_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			global_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			//------
			global_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			global_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

			//------ important for rendering as sprite---
			global_device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
			global_device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
			global_device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

			global_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
			global_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			global_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			global_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			global_device->SetRenderState(D3DRS_WRAP0, 0);

			// now save it
			if (i == 0)
				global_device->EndStateBlock(&m_dwRenderTextureBlock);
			else
				global_device->EndStateBlock(&m_dwRenderTextureBlockDeposit);
		}
	}


	inline auto render_texture_primitive(int x, int y, float z, int w, int h, LPDIRECT3DTEXTURE8 texture, unsigned char alpha) -> void {
		
		
		global_device->SetTexture(0, texture);

		// x, y, z, rhw, color
		DWORD color = (alpha << 24);
		QuadVertex g_square_vertices[] = {
			{ (float)x, (float)y, z, 1.0f,color, 0.0f, 0.0f },
			{ (float)(x + w), (float)y, z, 1.0f, color, 1.0f, 0.0f },
			{ (float)x, (float)(y + h), z, 1.0f, color, 0.0f, 1.0f },
			{ (float)(x + w), (float)(y + h), z, 1.0f, color, 1.0f, 1.0f }
		};

		unsigned char* buffer;
		vertex_buffer->Lock(0, 0, &buffer, 0);
		memcpy(buffer, g_square_vertices, sizeof(g_square_vertices));
		vertex_buffer->Unlock();

		global_device->SetPixelShader(NULL);
		global_device->SetVertexShader(OURQUAD);
		global_device->SetStreamSource(0, vertex_buffer, sizeof(QuadVertex));
		global_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	LPD3DXFONT pFont = nullptr;
	inline void draw_text(int x, int y, DWORD color, const char *str)
	{
		if (pFont == nullptr) {
			// Create the D3DX Font
			HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, 0, 0, 0, 0, "Verdana");
			if (FAILED(D3DXCreateFont(global_device, hFont, &pFont))) {
				return;
			}
		}
		
		RECT TextRect = { x, y, 0, 0 };
		pFont->Begin();
		pFont->DrawText(str, -1, &TextRect, DT_CALCRECT, 0);
		pFont->DrawText(str, -1, &TextRect, DT_LEFT, color);
		pFont->End();
		//pFont->Release();
	}


	int get_text_width(const char *szText)
	{
		RECT rcRect = { 0,0,0,0 };
		if (pFont)
		{
			// calculate required rect
			pFont->DrawText(szText, strlen(szText), &rcRect, DT_CALCRECT,
				D3DCOLOR_XRGB(0, 0, 0));
		}

		// return width
		return rcRect.right - rcRect.left;
	}

	inline auto device_lost(IDirect3DDevice8* device) -> void {

	}

	inline auto device_reset(IDirect3DDevice8* device) -> void {

	}

	inline auto world_to_screen(D3DXVECTOR3 input) -> D3DXVECTOR3 {

		D3DXVECTOR3 out;
		D3DVIEWPORT8 viewport;
		global_device->GetViewport(&viewport);
		D3DXMATRIX projection, view, world;
		global_device->GetTransform(D3DTS_VIEW, &view);
		D3DXMatrixIdentity(&world);
		global_device->GetTransform(D3DTS_PROJECTION, &projection);
		D3DXVec3Project(&out, &input, &viewport, &projection, &view, &world);
		return out;
	}

	/* 
	* Renders simple nicknames 
	* TODO(DavoSK): Render to texture with antialasing to dont see nicknames behind walls
	*/
	inline auto render_nicknames() -> void {

		for (u32 i = 0; i < ctx.max_entities; i++) {

			librg_entity_t *entity = librg_entity_fetch(&ctx, i);
			if (!entity || entity->id == local_player.entity.id) continue;

			if (entity->type == TYPE_PLAYER && entity->user_data) {
				auto player = reinterpret_cast<mafia_player*>(entity->user_data);

				if (player->ped) {
					
					auto player_pos = player->ped->GetInterface()->neckFrame->GetInterface()->mPosition;
					auto screen = world_to_screen({ player_pos.x, player_pos.y + 0.4f, player_pos.z });
					
					if (screen.z > 0.0f) {
						auto size = MafiaSDK::GetMission()->GetGame()->GetIndicators()->TextSize(player->name, 20.0f, 1, 0);
						
						//draw 8x more smoother ?
						for(int i = 0; i < 8; i++)
							MafiaSDK::GetMission()->GetGame()->GetIndicators()->OutText(player->name, screen.x - (size / 2), screen.y, size, 20.0f, 0xFFFFFFFF, 1, 1);
					}
				}
			}
		}
	}

	inline auto end_scene(IDirect3DDevice8* device) -> void {

	}
}