/*
	Created by Matt Hartley on 04/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "n64/core64.h"
#include "n64/math.h"
#include "n64/video.c"
#include "n64/rdp.c"
#include "n64/gfx.c"
#include "n64/system.c"
#include "n64/interrupts.c"
#include "n64/draw.c"
#include "n64/print.c"
#include "n64/util.c"


int main()
{
	InitDefaultVI();

	static fixed32_t time = 0;
	static fixed32_t time2 = 0x10000;

	for (;;) {
		// PollTime();
		// time += GetDeltaTime()
		time += tofixed32(2.0f / 50.0f);
		time2 += tofixed32(2.0f / 50.0f);

		RDP_StartCmdList(0, 0);

		RDP_SetColorImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 320);
		RDP_SetScissor(0, 0, 320, 288);
		
		RDP_SetOtherModes(RDP_MODE_FILL);
		RDP_SetFillColor16((4<<11) | (4<<6) | (4<<1) | 1);
		RDP_FillRect(0, 0, 320, 288);

		RDP_SetFillColor16((0<<11) | (0<<6) | (31<<1) | 1);
		RDP_FillRect(100 + fx32tof(SineFx(time)) * 100.0f, 100 + fx32tof(CosineFx(time)) * 100.0f, 10, 10);

		RDP_SetOtherModes(RDP_MODE_1CYCLE | RDP_MODE_TEX_RGB);
		RDP_SetPrimitiveColor(Color32(0, 255, 255, 255));
		RDP_SetEnvironmentColor(0x00FFFFFF);
		
		uint32_t tileWidth = 32;
		uint32_t tileHeight = 32;
		extern uint8_t texture_test_start[];
		RDP_SetTextureImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, texture_test_start);
		RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, 0);
		RDP_LoadTile(0, 0, 0, tileWidth, tileHeight);
		RDP_LoadSync();
		RDP_SetTileSize(0, 32, 32);

		RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_SHADE, 0));
		rdp_vertex_t verts[] = {
			{{200, 50}, {255, 255, 255, 255}, {0.0f, 0.0f}},
			{{260, 100}, {0, 0, 0, 255}, {32.0f, 0.0f}},
			{{230, 150}, {255, 255, 255, 255}, {32.0f, 32.0f}},
			{{170, 100}, {255, 255, 255, 255}, {0.0f, 32.0f}},
		};
		// RDP_FillTriangleWithShade(verts2);

		float sin = fx32tof(SineFx(time));
		float cos = fx32tof(CosineFx(time));
		for (int i=0; i<4; ++i) {
			// nice
			verts[i].pos = vec2(verts[i].pos.x*cos + verts[i].pos.y*sin, verts[i].pos.y*cos + verts[i].pos.x*sin);
		}

		uint16_t indices[] = {0, 1, 2, 0, 2, 3};
		GFX_Draw(verts, indices, 6);

		RDP_FullSync();
		RDP_ExecuteAndWait();
		WaitForVideoSync();
	}
}
