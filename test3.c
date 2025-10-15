/*
	Created by Matt Hartley on 04/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "n64/core64.h"
#include "n64/math.h"
#include "n64/system.h"
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

	void* framebuffer = UncachedAddress(AllocMemory(320*288*2, 64));
	void* depthbuffer = UncachedAddress(AllocMemory(320*288*2, 64));
	SetVideoFramebuffer(framebuffer);

	for (;;) {
		memset(depthbuffer, 0xFFFF, 320*288*2);

		// PollTime();
		// time += GetDeltaTime()
		time += tofixed32(2.0f / 50.0f);

		RDP_StartCmdList(0, 0);

		RDP_SetColorImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 320, framebuffer);
		RDP_SetDepthImage(depthbuffer);
		RDP_SetScissor(0, 0, 320, 288);
		
		RDP_SetOtherModes(RDP_MODE_FILL);
		RDP_SetFillColor16((4<<11) | (4<<6) | (4<<1) | 1);
		RDP_FillRect(0, 0, 320, 288);

		RDP_SetFillColor16((0<<11) | (0<<6) | (31<<1) | 1);
		RDP_FillRect(100 + fx32tof(SineFx(time)) * 100.0f, 100 + fx32tof(CosineFx(time)) * 100.0f, 10, 10);

		RDP_SetOtherModes(RDP_MODE_1CYCLE | RDP_MODE_TEX_RGB | RDP_MODE_TEX_PERSP | (1<<5) | (1<<4));
		RDP_SetPrimitiveColor(Color32(0, 255, 255, 255));
		RDP_SetEnvironmentColor(0x00FFFFFF);
		
		uint32_t tileWidth = 64;
		uint32_t tileHeight = 64;
		extern uint8_t texture_test_start[];
		RDP_SetTextureImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, texture_test_start);
		RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, 0);
		RDP_LoadTile(0, 0, 0, 16, 16);
		RDP_LoadSync();
		RDP_SetTileSize(0, 16, 16);

		RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_SHADE, 0));
		// rdp_vertex_t verts[] = {
		// 	{{200, 50}, {255, 255, 255, 255}, {0.0f, 0.0f}},
		// 	{{260, 100}, {0, 0, 0, 255}, {32.0f, 0.0f}},
		// 	{{230, 150}, {255, 255, 255, 255}, {32.0f, 32.0f}},
		// 	{{170, 100}, {255, 255, 255, 255}, {0.0f, 32.0f}},
		// };
		// rdp_vertex_t verts[] = {
		// 	{{0, -50}, {255, 255, 255, 255}, {0.0f, 0.0f}},
		// 	{{60, 0}, {0, 0, 0, 255}, {32.0f, 0.0f}},
		// 	{{30, 50}, {255, 255, 255, 255}, {32.0f, 32.0f}},
		// 	{{-30, 0}, {255, 255, 255, 255}, {0.0f, 32.0f}},
		// };
		rdp_vertex_t verts[] = {
			{ .pos={-0.5f, -0.5f, +0.5f},	.normal={0, 0, 1},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f, -0.5f, +0.5f},	.normal={0, 0, 1},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f, +0.5f, +0.5f},	.normal={0, 0, 1},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f, +0.5f, +0.5f},	.normal={0, 0, 1},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={+0.5f, -0.5f, -0.5f},	.normal={0, 0, -1},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={-0.5f, -0.5f, -0.5f},	.normal={0, 0, -1},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={-0.5f, +0.5f, -0.5f},	.normal={0, 0, -1},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={+0.5f, +0.5f, -0.5f},	.normal={0, 0, -1},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f, -0.5f, -0.5f},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={-0.5f, -0.5f, +0.5f},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={-0.5f, +0.5f, +0.5f},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f, +0.5f, -0.5f},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={+0.5f, -0.5f, +0.5f},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f, -0.5f, -0.5f},	.normal={1, 0, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f, +0.5f, -0.5f},	.normal={1, 0, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={+0.5f, +0.5f, +0.5f},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f, +0.5f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f, +0.5f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f, +0.5f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f, +0.5f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f, -0.5f, +0.5f},	.normal={0, -1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f, -0.5f, +0.5f},	.normal={0, -1, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f, -0.5f, -0.5f},	.normal={0, -1, 0},	.color={255, 255, 255, 255},			.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f, -0.5f, -0.5f},	.normal={0, -1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},
		};
		// RDP_FillTriangleWithShade(verts2);

		float sin = fx32tof(SineFx(time));
		float cos = fx32tof(CosineFx(time));
		for (int i=0; i<arraysize(verts); ++i) {
			// nice
			verts[i].pos.xyz = vec3(verts[i].pos.x*cos + verts[i].pos.z*sin, verts[i].pos.y + fx32tof(SineFx(time/2))*2, verts[i].pos.z*cos - verts[i].pos.x*sin);
			verts[i].normal = vec3(verts[i].normal.x*cos + verts[i].normal.z*sin, verts[i].normal.y, verts[i].normal.z*cos - verts[i].normal.x*sin);
		}

		GFX_SetPosition(200, 100);

		// uint16_t indices[] = {
		// 	0, 1, 2, 0, 2, 3,
		// 	4, 0, 3, 4, 3, 7,
		// 	4, 5, 6, 4, 6, 7,
		// 	1, 5, 6, 1, 6, 2,
		// };
		// GFX_DrawIndices(verts, indices, arraysize(indices));
		// GFX_DrawVertices(verts, arraysize(verts));

		uint16_t indices[] = {
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23,
		};
		GFX_DrawIndices(verts, indices, arraysize(indices));

		RDP_FullSync();
		RDP_ExecuteAndWait();
		WaitForVideoSync();
	}
}
