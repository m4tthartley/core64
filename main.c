/*
	Created by Matt Hartley on 17/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "n64/core64.h"
#include "n64/math.h"
#include "n64/rdp.h"
#include "n64/system.h"
#include "n64/video.c"
#include "n64/rdp.c"
#include "n64/gfx.c"
#include "n64/system.c"
#include "n64/interrupts.c"
#include "n64/draw.c"
#include "n64/print.c"
#include "n64/util.c"
#include "n64/video.h"

extern uint8_t texture_test_start[];

int main()
{
	static fixed32_t time = 0;
	
	InitDefaultVI();
	void* framebuffer = UncachedAddress(AllocMemory(320*288*2, 64));
	void* depthbuffer = UncachedAddress(AllocMemory(320*288*2, 64));
	SetVideoFramebuffer(framebuffer);

	SetDrawFramebuffer(framebuffer);
	DrawStr(GetResolution().width/2 - 5*6, GetResolution().height / 2 - 6, "Loading...");

	bool map[16*16];
	for (int y=0; y<16; ++y) {
		for (int x=0; x<16; ++x) {
			map[y*16+x] = ((uint16_t*)texture_test_start)[(48+y)*64 + 48+x] != 1;
		}
	}

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
		RDP_SetFillColor16((2<<11) | (2<<6) | (2<<1) | 1);
		RDP_FillRect(0, 0, 320, 288);

		RDP_SetFillColor16((0<<11) | (0<<6) | (31<<1) | 1);
		// RDP_FillRect(100 + fx32tof(SineFx(time)) * 100.0f, 100 + fx32tof(CosineFx(time)) * 100.0f, 10, 10);

		// for (int y=0; y<16; ++y) {
		// 	for (int x=0; x<16; ++x) {
		// 		if (map[y*16+x]) {
		// 			RDP_FillRect(x*12, y*12, 10, 10);
		// 		}
		// 	}
		// }

		RDP_SetOtherModes(RDP_MODE_1CYCLE | RDP_MODE_TEX_RGB | RDP_MODE_TEX_PERSP | (1<<5) | (1<<4));
		RDP_SetPrimitiveColor(Color32(0, 255, 255, 255));
		RDP_SetEnvironmentColor(0x00FFFFFF);
		
		uint32_t tileWidth = 64;
		uint32_t tileHeight = 64;
		RDP_SetTextureImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, texture_test_start);
		RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, tileWidth, 0);
		RDP_LoadTile(0, 0, 0, 16, 16);
		RDP_LoadSync();
		RDP_SetTileSize(0, 16, 16);

		RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_SHADE, 0));

		rdp_vertex_t cubeVertices[] = {
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

		float sin = fx32tof(SineFx(time/10));
		float cos = fx32tof(CosineFx(time/10));
		float sin2 = fx32tof(SineFx(time/4));
		float cos2 = fx32tof(CosineFx(time/4));
		rdp_vertex_t* verts = cubeVertices;
		for (int i=0; i<arraysize(cubeVertices); ++i) {
			// nice
			verts[i].pos.xyz = vec3(verts[i].pos.x*cos + verts[i].pos.z*sin, verts[i].pos.y + fx32tof(SineFx(time/2))*2, verts[i].pos.z*cos - verts[i].pos.x*sin);
			verts[i].normal = vec3(verts[i].normal.x*cos + verts[i].normal.z*sin, verts[i].normal.y, verts[i].normal.z*cos - verts[i].normal.x*sin);
		}

		GFX_SetPosition(0, 0, -10);

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
		// GFX_DrawIndices(verts, indices, arraysize(indices));


		// Tunnel
		rdp_vertex_t tunnelSegmentBuffer[] = {
#define FLOOR(x, z) \
			{ .pos={-0.5f + x, +0.0f, -0.5f + z},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},\
			{ .pos={+0.5f + x, +0.0f, -0.5f + z},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},\
			{ .pos={+0.5f + x, +0.0f, +0.5f + z},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},\
			{ .pos={-0.5f + x, +0.0f, +0.5f + z},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// { .pos={-0.5f - 1, +0.0f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			// { .pos={+0.5f - 1, +0.0f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			// { .pos={+0.5f - 1, +0.0f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			// { .pos={-0.5f - 1, +0.0f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// { .pos={-0.5f + 1, +0.0f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			// { .pos={+0.5f + 1, +0.0f, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			// { .pos={+0.5f + 1, +0.0f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			// { .pos={-0.5f + 1, +0.0f, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			FLOOR(-1, -1)
			FLOOR(+0, -1)
			FLOOR(+1, -1)
			FLOOR(-1, +0)
			FLOOR(+0, +0)
			FLOOR(+1, +0)
			FLOOR(-1, +1)
			FLOOR(+0, +1)
			FLOOR(+1, +1)

			// bottom left corner
			{ .pos={-0.5f - 2, +0.0f + 1, -0.5f},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, -0.5f},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, +0.5f},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f - 2, +0.0f + 1, +0.5f},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f - 2, +0.0f + 1, -0.5f - 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, -0.5f - 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, +0.5f - 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f - 2, +0.0f + 1, +0.5f - 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f - 2, +0.0f + 1, -0.5f + 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, -0.5f + 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f - 2, +0.0f + 0, +0.5f + 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f - 2, +0.0f + 1, +0.5f + 1},	.normal=normalize3(vec3(1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// bottom right corner
			{ .pos={-0.5f + 2, +0.0f + 0, -0.5f},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, -0.5f},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, +0.5f},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f + 2, +0.0f + 0, +0.5f},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f + 2, +0.0f + 0, -0.5f - 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, -0.5f - 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, +0.5f - 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f + 2, +0.0f + 0, +0.5f - 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			{ .pos={-0.5f + 2, +0.0f + 0, -0.5f + 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, -0.5f + 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			{ .pos={+0.5f + 2, +0.0f + 1, +0.5f + 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			{ .pos={-0.5f + 2, +0.0f + 0, +0.5f + 1},	.normal=normalize3(vec3(-1, 1, 0)),	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// left wall
#define LEFTWALL(x, y) \
			{ .pos={-0.5f - 2, +0.0f + 1 + y, +0.5f + x},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},\
			{ .pos={-0.5f - 2, +0.0f + 1 + y, -0.5f + x},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},\
			{ .pos={-0.5f - 2, +1.0f + 1 + y, -0.5f + x},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},\
			{ .pos={-0.5f - 2, +1.0f + 1 + y, +0.5f + x},	.normal={1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// right wall
#define RIGHTWALL(x, y) \
			{ .pos={+0.5f + 2, +0.0f + 1 + y, -0.5f + x},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},\
			{ .pos={+0.5f + 2, +0.0f + 1 + y, +0.5f + x},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},\
			{ .pos={+0.5f + 2, +1.0f + 1 + y, +0.5f + x},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},\
			{ .pos={+0.5f + 2, +1.0f + 1 + y, -0.5f + x},	.normal={-1, 0, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// { .pos={-0.5f - 2, +0.0f + 2, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			// { .pos={-0.5f - 2, +0.0f + 2, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			// { .pos={-0.5f - 2, +1.0f + 2, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			// { .pos={-0.5f - 2, +1.0f + 2, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			// { .pos={-0.5f - 2, +0.0f + 3, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 0.0f}},
			// { .pos={-0.5f - 2, +0.0f + 3, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 0.0f}},
			// { .pos={-0.5f - 2, +1.0f + 3, -0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={16.0f, 16.0f}},
			// { .pos={-0.5f - 2, +1.0f + 3, +0.5f},	.normal={0, 1, 0},	.color={255, 255, 255, 255},	.texcoord={0.0f, 16.0f}},

			LEFTWALL(-1, +0)
			LEFTWALL(+0, +0)
			LEFTWALL(+1, +0)
			LEFTWALL(-1, +1)
			LEFTWALL(+0, +1)
			LEFTWALL(+1, +1)
			LEFTWALL(-1, +2)
			LEFTWALL(+0, +2)
			LEFTWALL(+1, +2)

			RIGHTWALL(-1, +0)
			RIGHTWALL(+0, +0)
			RIGHTWALL(+1, +0)
			RIGHTWALL(-1, +1)
			RIGHTWALL(+0, +1)
			RIGHTWALL(+1, +1)
			RIGHTWALL(-1, +2)
			RIGHTWALL(+0, +2)
			RIGHTWALL(+1, +2)
		};
		
		// uint16_t tunnelSegmentIndices[] = {
		// 	QUAD_INDICES(0),
		// 	QUAD_INDICES(1),
		// 	QUAD_INDICES(2),
		// 	QUAD_INDICES(3),
		// 	QUAD_INDICES(4),
		// 	QUAD_INDICES(5),
		// 	QUAD_INDICES(6),
		// 	QUAD_INDICES(7),
		// };

		GFX_SetPosition(0, -2, -5);
		__lights[0] = vec3(sin2*2.25f, 0, -4);

		verts = tunnelSegmentBuffer;
		// for (int i=0; i<arraysize(tunnelSegmentBuffer); ++i) {
		// 	verts[i].pos.xyz = vec3(verts[i].pos.x*cos + verts[i].pos.z*sin, verts[i].pos.y, verts[i].pos.z*cos - verts[i].pos.x*sin);
		// 	verts[i].normal = vec3(verts[i].normal.x*cos + verts[i].normal.z*sin, verts[i].normal.y, verts[i].normal.z*cos - verts[i].normal.x*sin);
		// }

		GFX_DrawQuadBuffer(tunnelSegmentBuffer, arraysize(tunnelSegmentBuffer));

		RDP_FullSync();
		RDP_ExecuteAndWait();

		// for (int idx=0; idx<__debugTriangleCount; ++idx) {
		// 	debugtriangle_t tri = __debugTriangles[idx];
		// 	DrawLine(tri.v0.x, tri.v0.y, tri.v1.x, tri.v1.y, Color16(31, 0, 31, 1));
		// 	DrawLine(tri.v1.x, tri.v1.y, tri.v2.x, tri.v2.y, Color16(31, 0, 31, 1));
		// 	DrawLine(tri.v2.x, tri.v2.y, tri.v0.x, tri.v0.y, Color16(31, 0, 31, 1));
		// }
		__debugTriangleCount = 0;

		WaitForVideoSync();
	}
}
