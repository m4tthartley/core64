/*
	Created by Matt Hartley on 03/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include <stdbool.h>
#include "rdp.h"
#include "util.h"


uint64_t __attribute__((aligned(8))) commandList[64];

rdpcmdlist_t RDP_CmdList(void* buffer, uint32_t size)
{
	rdpcmdlist_t result = {0};
	result.buffer = buffer;
	result.bufferSize = size;
	return result;
}

void RDP_Write(rdpcmdlist_t* cmdlist, uint32_t word)
{
	assert(cmdlist->cursor + 4 < cmdlist->bufferSize);
	uint32_t* buffer = (uint32_t*)((uint8_t*)cmdlist->buffer + cmdlist->cursor);
	*buffer = word;
	cmdlist->cursor += 4;
}

#define RDPCMD_LOAD_SYNC 0x26000000
#define RDPCMD_PIPE_SYNC 0x27000000
#define RDPCMD_FULL_SYNC 0x29000000

#define RDPCMD_SET_PRIMITIVE_COLOR 0x3A000000
#define RDPCMD_SET_ENVIRONMENT_COLOR 0x3B000000

void RDP_LoadSync(rdpcmdlist_t* cmdlist)
{
	RDP_Write(cmdlist, RDPCMD_LOAD_SYNC);
	RDP_Write(cmdlist, 0);
}

void RDP_PipeSync(rdpcmdlist_t* cmdlist)
{
	RDP_Write(cmdlist, RDPCMD_PIPE_SYNC);
	RDP_Write(cmdlist, 0);
}

void RDP_FullSync(rdpcmdlist_t* cmdlist)
{
	RDP_Write(cmdlist, RDPCMD_FULL_SYNC);
	RDP_Write(cmdlist, 0);
}

void RDP_SetPrimitiveColor(rdpcmdlist_t* cmdlist, color32_t color)
{
	RDP_Write(cmdlist, RDPCMD_SET_PRIMITIVE_COLOR);
	RDP_Write(cmdlist, color);
}

void RDP_SetEnvironmentColor(rdpcmdlist_t* cmdlist, color32_t color)
{
	RDP_Write(cmdlist, RDPCMD_SET_ENVIRONMENT_COLOR);
	RDP_Write(cmdlist, color);
}

uint64_t RDP_CombinerRGB(uint64_t a, uint64_t b, uint64_t c, uint64_t d)
{
	uint64_t result = 0;
	if (!a) a = RDP_COMB_A_ZERO;
	if (!b) b = RDP_COMB_B_ZERO;
	if (!c) c = RDP_COMB_C_ZERO;
	if (!d) d = RDP_COMB_D_ZERO;
	result = (a<<37) | (c<<32) | (b<<24) | (d<<6)
			| (a<<52) | (c<<47) | (b<<28) | (d<<15);
	return result;
}

void RDP_SetCombineMode(rdpcmdlist_t* cmdlist, uint64_t combinerMask)
{
	// RDP_Write(&cmdlist, (0x3C<<24) | (RDP_COMB_PRIMITIVE<<20) | (RDP_COMB_SHADE<<15) | (RDP_COMB_PRIMITIVE<<12) | (RDP_COMB_SHADE<<9));
	// RDP_Write(&cmdlist, 0);

	// RDP_Write(&cmdlist, (0x3C<<24) | (RDP_COMB_PRIMITIVE<<20) | (RDP_COMB_SHADE<<15) | (RDP_COMB_PRIMITIVE<<5) | (RDP_COMB_SHADE<<0));
	// RDP_Write(&cmdlist, (8<<24) | (7<<6));//(RDP_COMB_PRIMITIVE<<12) | (RDP_COMB_SHADE<<9)
	// uint64_t combine = RDP_CombinerRGB(RDP_COMB_PRIMITIVE, 0, RDP_COMB_SHADE, 0);
	// RDP_Write(&cmdlist, (0x3C<<24) | (combine >> 32));
	// RDP_Write(&cmdlist, (combine & 0xFFFFFFFF));

	RDP_Write(cmdlist, (0x3C<<24) | (combinerMask >> 32));
	RDP_Write(cmdlist, (combinerMask & 0xFFFFFFFF));
}

void RDP_FillTriangle(rdpcmdlist_t* cmdlist, vecscreen_t* verts)
{
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);
	if (verts[1].y > verts[2].y) swap(verts[1], verts[2]);
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);

	rdpcommand_t cmd;
	int32_t v0x = verts[0].x, v0y = verts[0].y;
	int32_t v1x = verts[1].x, v1y = verts[1].y;
	int32_t v2x = verts[2].x, v2y = verts[2].y;
	int32_t lydiff = v2y-v1y;
	int32_t hydiff = v2y-v0y;
	int32_t mydiff = v1y-v0y;
	int32_t lxdiff = v2x-v1x;
	int32_t hxdiff = v2x-v0x;
	int32_t mxdiff = v1x-v0x;
	int32_t dxldy = lydiff ? (lxdiff*0x10000) / lydiff : 0;
	int32_t dxhdy = hydiff ? (hxdiff*0x10000) / hydiff : 0;
	int32_t dxmdy = mydiff ? (mxdiff*0x10000) / mydiff : 0;

	bool leftMajor = (hxdiff * mydiff - mxdiff * hydiff) <= 0;

	RDP_Write(cmdlist, (0b001<<27) | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_Write(cmdlist, (((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_Write(cmdlist, (v1x<<16));
	RDP_Write(cmdlist, dxldy);

	// line from lowest y to highest y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxhdy);

	// line from lowest y to middle y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxmdy);
}

void RDP_FillTriangleWithShade(rdpcmdlist_t* cmdlist, rdp_vertex_t* verts)
{
	rdp_vertex_t v0 = verts[0];
	rdp_vertex_t v1 = verts[1];
	rdp_vertex_t v2 = verts[2];

	if (v0.pos.y > v1.pos.y) swap(v0, v1);
	if (v1.pos.y > v2.pos.y) swap(v1, v2);
	if (v0.pos.y > v1.pos.y) swap(v0, v1);

	rdpcommand_t cmd;
	int32_t v0x = v0.pos.x, v0y = v0.pos.y;
	int32_t v1x = v1.pos.x, v1y = v1.pos.y;
	int32_t v2x = v2.pos.x, v2y = v2.pos.y;
	int32_t lydiff = v2y-v1y;
	int32_t hydiff = v2y-v0y;
	int32_t mydiff = v1y-v0y;
	int32_t lxdiff = v2x-v1x;
	int32_t hxdiff = v2x-v0x;
	int32_t mxdiff = v1x-v0x;
	int32_t dxldy = lydiff ? (lxdiff*0x10000) / lydiff : 0;
	int32_t dxhdy = hydiff ? (hxdiff*0x10000) / hydiff : 0;
	int32_t dxmdy = mydiff ? (mxdiff*0x10000) / mydiff : 0;

	bool leftMajor = (hxdiff * mydiff - mxdiff * hydiff) <= 0;

	RDP_Write(cmdlist, (0b001<<27) | (1<<26/*shade*/) | (1<<25/*texture*/) | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_Write(cmdlist, (((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_Write(cmdlist, (v1x<<16));
	RDP_Write(cmdlist, dxldy);

	// line from lowest y to highest y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxhdy);

	// line from lowest y to middle y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxmdy);

	// SHADE

#if 0
	// int part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, (64<<16) | 64);
	RDP_Write(cmdlist, (64<<16) | 64);
	// int part change in shade along scanline
	RDP_Write(cmdlist, (4<<16));
	RDP_Write(cmdlist, 0);
	// fractional part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
	// fractional part change in shade along scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
	// int part change along major edge
	RDP_Write(cmdlist, (((uint32_t)-1)<<16));
	RDP_Write(cmdlist, 0);
	// int part change each scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
	// frac part change along major edge
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
	// frac part change each scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
#endif

	/*
		{x = 200, y = 50, C = 240 (just red component to test)}
		{x = 260, y = 100, C = 120 (just red component to test)}
		{x = 230, y = 150, C = 60 (just red component to test)}

		hx = 230-200 = 30
		hy = 150-50 = 100
		mx = 260-200 = 60
		my = 100-50 = 50

		mr = -120;
		hr = -180;

		nxr = hy*mr - my*hr = -12,000 - -9,000 = -3,000

		const float nz = (data->hx*data->my) - (data->hy*data->mx); = -4,500
    	data->attr_factor = -1.0f / nz; = -0.0002222222222

		DrDx = nxr * attr_factor = 0.6666666666

		{
			hr/hx = -180 / 30 = -6
			mr/mx = -120 / 60 = -2
		}

		{
			dx1 = 260-200 = 60
			dy1 = 100-50 = 50
			dx2 = 230-200 = 30
			dy2 = 150-50 = 100

			dc1 = -120;
			dc2 = -180;

			det = dx1*dy2 - dx2*dy1 = 4500
			a = (dc1*dy2 - dc2*dy1) / det = -0.6666666667
			b = (dx1*dc2 - dx2*dc1) / det = -1.6
			c = c0 - a*x0 - b*y0 = 453.33333334
			edgeVector = (230-200) / (150-50) = 0.3

			dedge = b + a * edgeVector = -1.8
		}
	*/

	// Long edge = v0->v2, longest edge
	// High edge = v0->v1, high edge of the triangle arm
	// Low edge = v1->v2, low edge of the triangle arm

	// float HighEdgeColorDelta = v1.color.x - v0.color.x;
	vec4_t highEdgeColorDelta = sub4(v1.color, v0.color);
	// float longEdgeColorDelta = v2.color.x - v0.color.x;
	vec4_t longEdgeColorDelta = sub4(v2.color, v0.color);

	float longDiffX = v2x-v0x;
	float longDiffY = v2y-v0y;
	float highDiffX = v1x-v0x;
	float highDiffY = v1y-v0y;
	float lowDiffX = v2x-v1x;
	float lowDiffY = v2y-v1y;

	float determinant = highDiffX*longDiffY - longDiffX*highDiffY;

	vec4_t xCoef = div4f(sub4(mul4f(highEdgeColorDelta, longDiffY), mul4f(longEdgeColorDelta, highDiffY)), determinant);
	vec4_t yCoef = div4f(sub4(mul4f(longEdgeColorDelta, highDiffX), mul4f(highEdgeColorDelta, longDiffX)), determinant);
	vec4_t edgeCoef = add4(yCoef, mul4f(xCoef, (longDiffX/longDiffY)));

	// xCoef = -2.0f;
	// yCoef = -2.0f;
	// edgeCoef = -2.0f;

	// xCoef *= 65536.0f;
	// xCoef = mul4f(xCoef, 65536.0f);
	// yCoef *= 65536.0f;
	// yCoef = mul4f(yCoef, 65536.0f);
	// edgeCoef *= 65536.0f;
	// edgeCoef = mul4f(edgeCoef, 65536.0f);

	// float f = 65536.0f;
	// int32_t xCoefFixed = xCoef;
	vec4fixed32_t xCoefFixed = vec4tofixed32(xCoef);
	// int32_t yCoefFixed = yCoef;
	vec4fixed32_t yCoefFixed = vec4tofixed32(yCoef);
	// int32_t edgeCoefFixed = edgeCoef;
	vec4fixed32_t edgeCoefFixed = vec4tofixed32(edgeCoef);

	// vec4fixed32_t startColor = vec4tofixed32(v0.color);
	uint32_t r = ((uint32_t)v0.color.r) & 0xFF;
	uint32_t g = ((uint32_t)v0.color.g) & 0xFF;
	uint32_t b = ((uint32_t)v0.color.b) & 0xFF;
	uint32_t a = ((uint32_t)v0.color.a) & 0xFF;

	// int part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, (r<<16) | g);
	RDP_Write(cmdlist, (b<<16) | a);
	// int part change in shade along scanline
	RDP_Write(cmdlist, (xCoefFixed.r&0xFFFF0000) | ((xCoefFixed.g>>16)&0xFFFF));
	RDP_Write(cmdlist, (xCoefFixed.b&0xFFFF0000) | ((xCoefFixed.a>>16)&0xFFFF));
	// fractional part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
	// fractional part change in shade along scanline
	RDP_Write(cmdlist, (xCoefFixed.r<<16) | (xCoefFixed.g&0xFFFF));
	RDP_Write(cmdlist, (xCoefFixed.b<<16) | (xCoefFixed.a&0xFFFF));
	// int part change along major edge
	RDP_Write(cmdlist, (edgeCoefFixed.r&0xFFFF0000) | ((edgeCoefFixed.g>>16)&0xFFFF));
	RDP_Write(cmdlist, (edgeCoefFixed.b&0xFFFF0000) | ((edgeCoefFixed.a>>16)&0xFFFF));
	// int part change each scanline
	RDP_Write(cmdlist, (yCoefFixed.r&0xFFFF0000) | ((yCoefFixed.g>>16)&0xFFFF));
	RDP_Write(cmdlist, (yCoefFixed.b&0xFFFF0000) | ((yCoefFixed.a>>16)&0xFFFF));
	// frac part change along major edge
	RDP_Write(cmdlist, (edgeCoefFixed.r<<16) | (edgeCoefFixed.g&0xFFFF));
	RDP_Write(cmdlist, (edgeCoefFixed.b<<16) | (edgeCoefFixed.a&0xFFFF));
	// frac part change each scanline
	RDP_Write(cmdlist, (yCoefFixed.r<<16) | (yCoefFixed.g&0xFFFF));
	RDP_Write(cmdlist, (yCoefFixed.b<<16) | (yCoefFixed.a&0xFFFF));

	// // int part change along major edge
	// RDP_Write(cmdlist, 0);
	// RDP_Write(cmdlist, 0);
	// // int part change each scanline
	// RDP_Write(cmdlist, 0);
	// RDP_Write(cmdlist, 0);
	// // frac part change along major edge
	// RDP_Write(cmdlist, 0);
	// RDP_Write(cmdlist, 0);
	// // frac part change each scanline
	// RDP_Write(cmdlist, 0);
	// RDP_Write(cmdlist, 0);

	// TEXTURE COORDINATES

	v0.texcoord.u *= 32;
	v0.texcoord.v *= 32;
	v1.texcoord.u *= 32;
	v1.texcoord.v *= 32;
	v2.texcoord.u *= 32;
	v2.texcoord.v *= 32;

	// hardcoding W as 1 for now
	float invw0 = 1.0f / 1.0f;//* 0x7FFF;
	float invw1 = 1.0f / 1.0f;//* 0x7FFF;
	float invw2 = 1.0f / 1.0f;//* 0x7FFF;

	vec3_t texData0 = vec3(v0.texcoord.u, v0.texcoord.v, invw0);
	vec3_t texData1 = vec3(v1.texcoord.u, v1.texcoord.v, invw1);
	vec3_t texData2 = vec3(v2.texcoord.u, v2.texcoord.v, invw2);
	vec3_t highEdgeTexDelta = sub3(texData1, texData0);
	vec3_t longEdgeTexDelta = sub3(texData2, texData0);

	vec3_t xTexCoef = div3f(sub3(mul3f(highEdgeTexDelta, longDiffY), mul3f(longEdgeTexDelta, highDiffY)), determinant);
	vec3_t yTexCoef = div3f(sub3(mul3f(longEdgeTexDelta, highDiffX), mul3f(highEdgeTexDelta, longDiffX)), determinant);
	vec3_t edgeTexCoef = add3(yTexCoef, mul3f(xTexCoef, (longDiffX/longDiffY)));
	// xTexCoef = vec3(32, 32, 0);
	// yTexCoef = vec3(0, 0, 0);
	// edgeTexCoef = vec3(0, 0, 0);

	vec3fx32_t xTexCoefFixed = vec3tofixed32(xTexCoef);
	vec3fx32_t yTexCoefFixed = vec3tofixed32(yTexCoef);
	vec3fx32_t edgeTexCoefFixed = vec3tofixed32(edgeTexCoef);

	uint32_t u = tofixed32(v0.texcoord.u); //((uint32_t)v0.texcoord.u) & 0xFF;
	uint32_t v = tofixed32(v0.texcoord.v); //((uint32_t)v0.texcoord.v) & 0xFF;
	uint32_t w = tofixed32(invw0); //((uint32_t)invw0) & 0xFF;

	// int part at xh, floor(yh)
	RDP_Write(cmdlist, (u&0xFFFF0000) | ((v>>16)&0xFFFF));
	RDP_Write(cmdlist, (w&0xFFFF0000));
	// int part change along scanline
	RDP_Write(cmdlist, (xTexCoefFixed.u&0xFFFF0000) | ((xTexCoefFixed.v>>16)&0xFFFF));
	RDP_Write(cmdlist, (xTexCoefFixed.w&0xFFFF0000));
	// fractional part at xh, floor(yh)
	RDP_Write(cmdlist, ((u&0xFFFF)<<16) | (v&0xFFFF));
	RDP_Write(cmdlist, ((w&0xFFFF)<<16));
	// fractional part along scanline
	RDP_Write(cmdlist, (xTexCoefFixed.u<<16) | (xTexCoefFixed.v&0xFFFF));
	RDP_Write(cmdlist, (xTexCoefFixed.w<<16));
	// int part change along major edge
	RDP_Write(cmdlist, (edgeTexCoefFixed.u&0xFFFF0000) | ((edgeTexCoefFixed.v>>16)&0xFFFF));
	RDP_Write(cmdlist, (edgeTexCoefFixed.w&0xFFFF0000));
	// int part change each scanline
	RDP_Write(cmdlist, (yTexCoefFixed.u&0xFFFF0000) | ((yTexCoefFixed.v>>16)&0xFFFF));
	RDP_Write(cmdlist, (yTexCoefFixed.w&0xFFFF0000));
	// frac part change along major edge
	RDP_Write(cmdlist, (edgeTexCoefFixed.u<<16) | (edgeTexCoefFixed.v&0xFFFF));
	RDP_Write(cmdlist, (edgeTexCoefFixed.w<<16));
	// frac part change each scanline
	RDP_Write(cmdlist, (yTexCoefFixed.u<<16) | (yTexCoefFixed.v&0xFFFF));
	RDP_Write(cmdlist, (yTexCoefFixed.w<<16));
}

void RDP_FillRect(rdpcmdlist_t* cmdlist, int32_t x, int32_t y, int32_t width, int32_t height)
{
	uint32_t x0 = x<<2;
	uint32_t y0 = y<<2;
	uint32_t x1 = (x+width)<<2;
	uint32_t y1 = (y+height)<<2;

	RDP_Write(cmdlist, (0x36<<24) | (x1<<12) | (y1));
	RDP_Write(cmdlist, (x0<<12) | (y0));
}
