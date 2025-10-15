/*
	Created by Matt Hartley on 02/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __RDP_HEADER__
#define __RDP_HEADER__

#include <stdint.h>
#include "math.h"


#define RDP_COMB_COMBINED		0
#define RDP_COMB_TEX0			1
#define RDP_COMB_TEX1			2
#define RDP_COMB_PRIMITIVE		3
#define RDP_COMB_SHADE			4
#define RDP_COMB_ENVIRONMENT	5

#define RDP_COMB_A_ONE			6
#define RDP_COMB_A_NOISE		7
#define RDP_COMB_A_ZERO			8
#define RDP_COMB_A_ALPHA_ONE	6
#define RDP_COMB_A_ALPHA_ZERO	7

#define RDP_COMB_B_CENTER		6
#define RDP_COMB_B_K4			7
#define RDP_COMB_B_ZERO			8
#define RDP_COMB_B_ALPHA_ONE	6
#define RDP_COMB_B_ALPHA_ZERO	7

#define RDP_COMB_C_SCALE				6
#define RDP_COMB_C_COMBINED_ALPHA 		7
#define RDP_COMB_C_TEX0_ALPHA			8
#define RDP_COMB_C_TEX1_ALPHA			9
#define RDP_COMB_C_PRIMITIVE_ALPHA		10
#define RDP_COMB_C_SHADE_ALPHA			11
#define RDP_COMB_C_ENVIRONMENT_ALPHA	12
#define RDP_COMB_C_LOD_FRACTION			13
#define RDP_COMB_C_PRIM_LOD_FRAC		14
#define RDP_COMB_C_K5					15
#define RDP_COMB_C_ZERO					16
#define RDP_COMB_C_ALPHA_PRIM_LOD_FRAC	6
#define RDP_COMB_C_ALPHA_ZERO			7

#define RDP_COMB_D_ONE			6
#define RDP_COMB_D_ZERO			7
#define RDP_COMB_D_ALPHA_ONE	6
#define RDP_COMB_D_ALPHA_ZERO	7

// #define RDP_COMB_1CYCLE_RGB(a, b, c, d) (((uint64_t)a<<37) | ((uint64_t)c<<32) | ((uint64_t)b<<24) | ((uint64_t)d<<6))

typedef struct {
	uint32_t word0;
	uint32_t word1;
} rdpcommand_t;

typedef struct {
	uint32_t x;
	uint32_t y;
} vecscreen_t; // Currently just 32bit uint

typedef struct {
	vec4_t pos;
	float invw;
	vec3_t normal;
	vec4_t color;
	vec2_t texcoord;
} rdp_vertex_t;

typedef struct {
	void* buffer;
	uint32_t bufferSize; // in bytes
	uint32_t cursor; // in bytes
} rdpcmdlist_t;

// typedef union {
// 	uint32_t packed;
// 	struct {
// 		uint8_t r;
// 		uint8_t g;
// 		uint8_t b;
// 		uint8_t a;
// 	};
// } color32_t;
typedef uint32_t color32_t;
typedef uint16_t color16_t;

// #define Color32(red, green, blue, alpha) ((color32_t){ .r=red, .g=green, .b=blue, .a=alpha })
#define Color32(r, g, b, a) ((r&0xFF)<<24 | (g&0xFF)<<16 | (b&0xFF)<<8 | (a&0xFF)<<0)
// #define Color16(r, g, b, a) (((r>>3)&31)<<11 | ((g>>3)&31)<<6 | ((b>>3)&31)<<1 | (a&1))
#define Color16(r, g, b, a) ((r&31)<<11 | (g&31)<<6 | (b&31)<<1 | (a&1))

rdpcmdlist_t RDP_CmdList(void* buffer, uint32_t size);
void RDP_Write(rdpcmdlist_t* cmdlist, uint32_t word);

void RDP_LoadSync();
void RDP_PipeSync();
void RDP_FullSync();

void RDP_SetOtherModes(uint64_t mask);

void RDP_SetTileSize(uint32_t tileIndex, uint32_t width, uint32_t height);
void RDP_LoadBlock(uint32_t tileIndex, uint32_t s0, uint32_t t0, uint32_t length);
void RDP_LoadTile(uint32_t tileIndex, uint32_t s0, uint32_t t0, uint32_t s1, uint32_t t1);
void RDP_SetTile(uint32_t tileIndex, uint32_t textureFormatMask, uint32_t width, uint32_t tmemOffset);

void RDP_SetFogColor(color32_t color);
void RDP_SetBlendColor(color32_t color);
void RDP_SetPrimitiveColor(color32_t color);
void RDP_SetEnvironmentColor(color32_t color);

uint64_t RDP_CombinerRGB(uint64_t a, uint64_t b, uint64_t c, uint64_t d);
void RDP_SetCombineMode(uint64_t combinerMask);

void RDP_SetTextureImage(uint32_t textureFormatMask, uint32_t width, void* address);

void RDP_FillTriangle(vecscreen_t* verts);
void RDP_FillTriangleWithShade(rdp_vertex_t* verts);
void RDP_Triangle(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2);
void RDP_FillRect(int32_t x, int32_t y, int32_t width, int32_t height);


#endif
