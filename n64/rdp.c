/*
	Created by Matt Hartley on 03/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include <stdbool.h>
#include "draw.h"
#include "rdp.h"
#include "util.h"
#include "system.h"
#include "registers.h"


uint8_t __attribute__((aligned(8))) _commandList[1024*4];
uintptr_t _commandListBuffer;
uint32_t  _commandListSize; // in bytes
uint32_t  _commandListCursor; // in bytes


rdpcmdlist_t RDP_CmdList(void* buffer, uint32_t size)
{
	rdpcmdlist_t result = {0};
	result.buffer = buffer;
	result.bufferSize = size;
	return result;
}

void RDP_StartCmdList(void* buffer, uint32_t size)
{
	if (!buffer) {
		buffer = _commandList;
		size = sizeof(_commandList);
	}

	_commandListBuffer = (uintptr_t)buffer;
	_commandListSize = size;
	_commandListCursor = 0;
}

void RDP_WriteWord(uint32_t word)
{
	assert(_commandListBuffer);
	assert(_commandListCursor + 4 < _commandListSize);
	uint32_t* buffer = (uint32_t*)((uint8_t*)_commandListBuffer + _commandListCursor);
	*buffer = word;
	_commandListCursor += 4;
}

void RDP_Write(rdpcmdlist_t* cmdlist, uint32_t word)
{
#if 0
	assert(cmdlist->cursor + 4 < cmdlist->bufferSize);
	uint32_t* buffer = (uint32_t*)((uint8_t*)cmdlist->buffer + cmdlist->cursor);
	*buffer = word;
	cmdlist->cursor += 4;
#endif
	RDP_WriteWord(word);
}

#define RDPCMD_FILL_TRIANGLE (0b001<<27)
#define RDPCMD_TEXTURE_RECTANGLE 0x24000000
#define RDPCMD_TEXTURE_RECTANGLE_FLIPPED 0x25000000

#define RDPCMD_LOAD_SYNC 0x26000000
#define RDPCMD_PIPE_SYNC 0x27000000
#define RDPCMD_FULL_SYNC 0x29000000

#define RDPCMD_SET_SCISSOR 0x2D000000
#define RDPCMD_SET_OTHER_MODES 0x2F000000

#define RDPCMD_SET_TILE_SIZE 0x32000000
#define RDPCMD_LOAD_BLOCK 0x33000000
#define RDPCMD_LOAD_TILE 0x34000000
#define RDPCMD_SET_TILE 0x35000000

#define RDPCMD_SET_FILL_COLOR 0x37000000
#define RDPCMD_SET_FOG_COLOR 0x38000000
#define RDPCMD_SET_BLEND_COLOR 0x39000000
#define RDPCMD_SET_PRIMITIVE_COLOR 0x3A000000
#define RDPCMD_SET_ENVIRONMENT_COLOR 0x3B000000

#define RDPCMD_SET_TEXTURE_IMAGE 0x3D000000
#define RDPCMD_SET_DEPTH_IMAGE 0x3E000000
#define RDPCMD_SET_COLOR_IMAGE 0x3F000000

#define RDP_MODE_1CYCLE ((uint64_t)0<<52)
#define RDP_MODE_2CYCLE ((uint64_t)1<<52)
#define RDP_MODE_COPY ((uint64_t)2<<52)
#define RDP_MODE_FILL ((uint64_t)3<<52)

#define RDP_MODE_ATOMIC_PRIM ((uint64_t)1<<55)
#define RDP_MODE_TEX_PERSP ((uint64_t)1<<51)
#define RDP_MODE_TEX_DETAIL ((uint64_t)1<<50)
#define RDP_MODE_TEX_SHARPEN ((uint64_t)1<<49)
#define RDP_MODE_TEX_LOD ((uint64_t)1<<48)
#define RDP_MODE_TEX_LUT ((uint64_t)1<<47)
#define RDP_MODE_TEX_LUT_TYPE ((uint64_t)1<<46)
#define RDP_MODE_TEX_SAMPLE_TYPE ((uint64_t)1<<45)
#define RDP_MODE_TEX_BI_SAMPLE_MODE ((uint64_t)1<<44)
#define RDP_MODE_BI_LERP ((uint64_t)1<<43)
#define RDP_MODE_TEX_RGB ((uint64_t)1<<43)
#define RDP_MODE_BI_LERP_CYCLE2 ((uint64_t)1<<42)
#define RDP_MODE_TEX_RGB_CYCLE2 ((uint64_t)1<<42)
#define RDP_MODE_TEX_CONVERT_ONE ((uint64_t)1<<41)
#define RDP_MODE_CHROMA_KEY ((uint64_t)1<<40)

#define RDP_MODE_RGB_DITHER_SQUARE ((uint64_t)0<<38)
#define RDP_MODE_RGB_DITHER_BAYER ((uint64_t)1<<38)
#define RDP_MODE_RGB_DITHER_NOISE ((uint64_t)2<<38)
#define RDP_MODE_RGB_DITHER_DISABLE ((uint64_t)3<<38)
#define RDP_MODE_ALPHA_DITHER_SAME ((uint64_t)0<<36)
#define RDP_MODE_ALPHA_DITHER_INVERSE ((uint64_t)1<<36)
#define RDP_MODE_ALPHA_DITHER_NOISE ((uint64_t)2<<36)
#define RDP_MODE_ALPHA_DITHER_DISABLE ((uint64_t)3<<36)

#define RDP_TEXTURE_IMAGE_FORMAT_RGBA ((uint32_t)0<<21)
#define RDP_TEXTURE_IMAGE_FORMAT_YUV ((uint32_t)1<<21)
#define RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED ((uint32_t)2<<21)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA ((uint32_t)3<<21)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY ((uint32_t)4<<21)
#define RDP_TEXTURE_IMAGE_PIXEL_SIZE_4 ((uint32_t)0<<19)
#define RDP_TEXTURE_IMAGE_PIXEL_SIZE_8 ((uint32_t)1<<19)
#define RDP_TEXTURE_IMAGE_PIXEL_SIZE_16 ((uint32_t)2<<19)
#define RDP_TEXTURE_IMAGE_PIXEL_SIZE_32 ((uint32_t)3<<19)
#define RDP_TEXTURE_IMAGE_FORMAT_RGBA16 (RDP_TEXTURE_IMAGE_FORMAT_RGBA | RDP_TEXTURE_IMAGE_PIXEL_SIZE_16)
#define RDP_TEXTURE_IMAGE_FORMAT_RGBA32 (RDP_TEXTURE_IMAGE_FORMAT_RGBA | RDP_TEXTURE_IMAGE_PIXEL_SIZE_32)
#define RDP_TEXTURE_IMAGE_FORMAT_YUV16 (RDP_TEXTURE_IMAGE_FORMAT_YUV | RDP_TEXTURE_IMAGE_PIXEL_SIZE_16)
#define RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED4 (RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED | RDP_TEXTURE_IMAGE_PIXEL_SIZE_4)
#define RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED8 (RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED | RDP_TEXTURE_IMAGE_PIXEL_SIZE_8)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA4 (RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA | RDP_TEXTURE_IMAGE_PIXEL_SIZE_4)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA8 (RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA | RDP_TEXTURE_IMAGE_PIXEL_SIZE_8)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA16 (RDP_TEXTURE_IMAGE_FORMAT_INTENSITY_ALPHA | RDP_TEXTURE_IMAGE_PIXEL_SIZE_16)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY4 (RDP_TEXTURE_IMAGE_FORMAT_INTENSITY | RDP_TEXTURE_IMAGE_PIXEL_SIZE_4)
#define RDP_TEXTURE_IMAGE_FORMAT_INTENSITY8 (RDP_TEXTURE_IMAGE_FORMAT_INTENSITY | RDP_TEXTURE_IMAGE_PIXEL_SIZE_8)

#define RDP_TRI_ENABLE_SHADE (1<<26)
#define RDP_TRI_ENABLE_TEXTURE (1<<25)
#define RDP_TRI_ENABLE_ZBUFFER (1<<24)
#define RDP_TRI_ENABLE_LMAJOR (1<<23)

enum {
	TEXTURE_FORMAT_RGBA32 = 0,
	TEXTURE_FORMAT_RGBA16,
	TEXTURE_FORMAT_PALETTED8,
	TEXTURE_FORMAT_PALETTED4,
	TEXTURE_FORMAT_INTENSITY8,
	TEXTURE_FORMAT_INTENSITY4,
} textureformat_t;

uint64_t _textureImageFormatTable[] = {
	/* TEXTURE_FORMAT_RGBA32 */ RDP_TEXTURE_IMAGE_FORMAT_RGBA32,
	/* TEXTURE_FORMAT_RGBA16 */ RDP_TEXTURE_IMAGE_FORMAT_RGBA16,
	/* TEXTURE_FORMAT_PALETTED8 */ RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED8,
	/* TEXTURE_FORMAT_PALETTED4 */ RDP_TEXTURE_IMAGE_FORMAT_COLOR_INDEXED4,
	/* TEXTURE_FORMAT_INTENSITY8 */ RDP_TEXTURE_IMAGE_FORMAT_INTENSITY8,
	/* TEXTURE_FORMAT_INTENSITY4 */ RDP_TEXTURE_IMAGE_FORMAT_INTENSITY4,
};

void RDP_LoadSync()
{
	RDP_WriteWord(RDPCMD_LOAD_SYNC);
	RDP_WriteWord(0);
}

void RDP_PipeSync()
{
	RDP_WriteWord(RDPCMD_PIPE_SYNC);
	RDP_WriteWord(0);
}

void RDP_FullSync()
{
	RDP_WriteWord(RDPCMD_FULL_SYNC);
	RDP_WriteWord(0);
}

void RDP_SetScissor(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{	
	// uint32_t sx0 = 1<<2;
	// uint32_t sy0 = 1<<2;
	// uint32_t sx1 = (320-2)<<2;
	// uint32_t sy1 = (288-2)<<2;
	// cmd.word0 = (0x2D<<24) | (sx0<<12) | (sy0);
	// cmd.word1 = (sx1<<12) | (sy1);
	// RDP_Write(&cmdlist, cmd.word0);
	// RDP_Write(&cmdlist, cmd.word1);

	RDP_WriteWord(RDPCMD_SET_SCISSOR | ((x0<<2)<<12) | ((y0<<2)));
	RDP_WriteWord(((x1<<2)<<12) | ((y1<<2)));
}

void RDP_SetOtherModes(uint64_t mask)
{
	// RDP_Write(cmdlist, (0x2F<<24) | (0x0<<20) | (1<<11)); // 1cycle mode
	// RDP_Write(cmdlist, 0);

	RDP_WriteWord(RDPCMD_SET_OTHER_MODES | (mask>>32));
	RDP_WriteWord((mask&0xFFFFFFFF));
}

void RDP_SetTileSize(uint32_t tileIndex, uint32_t width, uint32_t height)
{
	// RDP_Write(cmdlist, RDPCMD_SET_TILE_SIZE);
	// RDP_Write(cmdlist, (/*tile*/0<<24) | (((tileWidth-1)<<2)<<12) | (((tileHeight-1)<<2)<<0));

	RDP_WriteWord(RDPCMD_SET_TILE_SIZE);
	RDP_WriteWord((tileIndex<<24) | (((width-1)<<2)<<12) | (((height-1)<<2)<<0));
}

void RDP_LoadBlock(uint32_t tileIndex, uint32_t s0, uint32_t t0, uint32_t length)
{
	// TODO: This does not work properly yet!

	// RDP_Write(&cmdlist, (0x33<<24));
	// RDP_Write(&cmdlist, (/*tile*/7<<24) | (/*lower right S*/((32*32-1))<<12) | ((2048+line-1)/line) /*((1<<11) / 32)*/);
	// (1<<11) / 8

	RDP_WriteWord(RDPCMD_LOAD_BLOCK | (s0<<12) | (t0<<0));
	uint32_t line = 32*2 / 8;
	// uint32_t dxt = (2048 + line - 1) / line;
	// uint32_t dxt = (1<<11) / 32;
	uint32_t dxt = 0;
	RDP_WriteWord((tileIndex<<24) | ((length-1)<<12) | (dxt&0xFFF));
}

void RDP_LoadTile(uint32_t tileIndex, uint32_t s0, uint32_t t0, uint32_t s1, uint32_t t1)
{
	// RDP_Write(&cmdlist, (0x34<<24) | (/*upper left S*/0<<12) | (/*upper left T*/0<<0));
	// RDP_Write(&cmdlist, (/*tile*/7<<24) | (/*lower right S*/((tileWidth-1)<<2)<<12) | (/*lower right T*/((tileHeight-1)<<2)<<0));

	RDP_WriteWord(RDPCMD_LOAD_TILE | ((s0<<2)<<12) | ((t0<<2)<<0));
	RDP_WriteWord((tileIndex<<24) | (((s1-1)<<2)<<12) | (((t1-1)<<2)<<0));
}

void RDP_SetTile(uint32_t tileIndex, uint32_t textureFormatMask, uint32_t width, uint32_t tmemOffset)
{
	// uint32_t line = 32*2 / 8;
	// RDP_Write(&cmdlist, (0x35<<24) | (/*format*/0<<21) | (/*size*/2<<19) | (line<<9) | (/*tmem addr*/0));
	// RDP_Write(&cmdlist, (/*tile*/7<<24));

	uint32_t pixelSize = (textureFormatMask>>19) & 3;
	uint32_t line = width*pixelSize / 8;
	RDP_WriteWord(RDPCMD_SET_TILE | textureFormatMask | (line<<9) | tmemOffset);
	RDP_WriteWord((tileIndex<<24));
}

void RDP_SetFillColor32(color32_t color)
{
	// cmd.word0 = 0x37 << 24;
	// uint16_t color = (8<<1) | 1;
	// cmd.word1 = (color << 16) | color;
	// RDP_Write(&cmdlist, cmd.word0);
	// RDP_Write(&cmdlist, cmd.word1);

	RDP_WriteWord(RDPCMD_SET_FILL_COLOR);
	RDP_WriteWord(color);
}
void RDP_SetFillColor16(color16_t color)
{
	RDP_WriteWord(RDPCMD_SET_FILL_COLOR);
	RDP_WriteWord((color << 16) | color);
}

void RDP_SetFogColor(color32_t color)
{
	RDP_WriteWord(RDPCMD_SET_FOG_COLOR);
	RDP_WriteWord(color);
}

void RDP_SetBlendColor(color32_t color)
{
	RDP_WriteWord(RDPCMD_SET_BLEND_COLOR);
	RDP_WriteWord(color);
}

void RDP_SetPrimitiveColor(color32_t color)
{
	RDP_WriteWord(RDPCMD_SET_PRIMITIVE_COLOR);
	RDP_WriteWord(color);
}

void RDP_SetEnvironmentColor(color32_t color)
{
	RDP_WriteWord(RDPCMD_SET_ENVIRONMENT_COLOR);
	RDP_WriteWord(color);
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

void RDP_SetCombineMode(uint64_t combinerMask)
{
	// RDP_Write(&cmdlist, (0x3C<<24) | (RDP_COMB_PRIMITIVE<<20) | (RDP_COMB_SHADE<<15) | (RDP_COMB_PRIMITIVE<<12) | (RDP_COMB_SHADE<<9));
	// RDP_Write(&cmdlist, 0);

	// RDP_Write(&cmdlist, (0x3C<<24) | (RDP_COMB_PRIMITIVE<<20) | (RDP_COMB_SHADE<<15) | (RDP_COMB_PRIMITIVE<<5) | (RDP_COMB_SHADE<<0));
	// RDP_Write(&cmdlist, (8<<24) | (7<<6));//(RDP_COMB_PRIMITIVE<<12) | (RDP_COMB_SHADE<<9)
	// uint64_t combine = RDP_CombinerRGB(RDP_COMB_PRIMITIVE, 0, RDP_COMB_SHADE, 0);
	// RDP_Write(&cmdlist, (0x3C<<24) | (combine >> 32));
	// RDP_Write(&cmdlist, (combine & 0xFFFFFFFF));

	RDP_WriteWord((0x3C<<24) | (combinerMask >> 32));
	RDP_WriteWord((combinerMask & 0xFFFFFFFF));
}

void RDP_SetTextureImage(uint32_t textureFormatMask, uint32_t width, void* address)
{
	// RDP_Write(cmdlist, RDPCMD_SET_TEXTURE_IMAGE | (/*format*/0<<21) | (/*size*/2<<19) | (/*width*/32-1));
	// RDP_Write(cmdlist, ((address&0x1FFFFFFF)|0xA0000000));
	/*>> 3*/
	RDP_WriteWord(RDPCMD_SET_TEXTURE_IMAGE | textureFormatMask | (width-1));
	RDP_WriteWord((((uintptr_t)address&0x1FFFFFFF)|0xA0000000));
}

void RDP_SetDepthImage(void* address)
{
	RDP_WriteWord(RDPCMD_SET_DEPTH_IMAGE);
	RDP_WriteWord((uintptr_t)address);
}

void RDP_SetColorImage(uint32_t textureFormatMask, uint16_t width, void* address)
{
	// cmd.word0 = RDPCMD_SET_COLOR_IMAGE | textureFormatMask | (width-1);
	// cmd.word1 = VI_FRAMEBUFFERBASE & 0xFFFFFF;
	RDP_WriteWord(RDPCMD_SET_COLOR_IMAGE | textureFormatMask | (width-1));
	RDP_WriteWord((uintptr_t)address);
}

void RDP_FillTriangle(vecscreen_t* verts)
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

	RDP_WriteWord((0b001<<27) | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_WriteWord((((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_WriteWord((v1x<<16));
	RDP_WriteWord(dxldy);

	// line from lowest y to highest y
	RDP_WriteWord((v0x<<16));
	RDP_WriteWord(dxhdy);

	// line from lowest y to middle y
	RDP_WriteWord((v0x<<16));
	RDP_WriteWord(dxmdy);
}

bool __rdpEnableTriangleShade = 1;
bool __rdpEnableTriangleTexture = 1;
bool __rdpEnableTriangleZBuffer = 1;

void RDP_Triangle(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
{
	// rdp_vertex_t v0 = verts[0];
	// rdp_vertex_t v1 = verts[1];
	// rdp_vertex_t v2 = verts[2];

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

	uint32_t options = 0;
	if (__rdpEnableTriangleShade) {
		options |= RDP_TRI_ENABLE_SHADE;
	}
	if (__rdpEnableTriangleTexture) {
		options |= RDP_TRI_ENABLE_TEXTURE;
	}
	if (__rdpEnableTriangleZBuffer) {
		options |= RDP_TRI_ENABLE_ZBUFFER;
	}

	RDP_WriteWord((0b001<<27) | options | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_WriteWord((((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_WriteWord((v1x<<16));
	RDP_WriteWord(dxldy);

	// line from lowest y to highest y
	RDP_WriteWord((v0x<<16));
	RDP_WriteWord(dxhdy);

	// line from lowest y to middle y
	RDP_WriteWord((v0x<<16));
	RDP_WriteWord(dxmdy);

	// SHADE

	if (__rdpEnableTriangleShade) {
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
		// float longEdgeColorDelta = v2.color.x - v0.color.x;
		vec4_t highEdgeColorDelta = sub4(v1.color, v0.color);
		vec4_t longEdgeColorDelta = sub4(v2.color, v0.color);

		float longDiffX = v2x-v0x;
		float longDiffY = v2y-v0y;
		float highDiffX = v1x-v0x;
		float highDiffY = v1y-v0y;
		float lowDiffX = v2x-v1x;
		float lowDiffY = v2y-v1y;

		float edgeRatio = divsafe(longDiffX, longDiffY);
		float determinant = highDiffX*longDiffY - longDiffX*highDiffY;

		vec4_t xCoef = div4f(sub4(mul4f(highEdgeColorDelta, longDiffY), mul4f(longEdgeColorDelta, highDiffY)), determinant);
		vec4_t yCoef = div4f(sub4(mul4f(longEdgeColorDelta, highDiffX), mul4f(highEdgeColorDelta, longDiffX)), determinant);
		vec4_t edgeCoef = add4(yCoef, mul4f(xCoef, edgeRatio));

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
		// int32_t yCoefFixed = yCoef;
		// int32_t edgeCoefFixed = edgeCoef;
		vec4fixed32_t xCoefFixed = vec4tofixed32(xCoef);
		vec4fixed32_t yCoefFixed = vec4tofixed32(yCoef);
		vec4fixed32_t edgeCoefFixed = vec4tofixed32(edgeCoef);

		// vec4fixed32_t startColor = vec4tofixed32(v0.color);
		uint32_t r = ((uint32_t)v0.color.r) & 0xFF;
		uint32_t g = ((uint32_t)v0.color.g) & 0xFF;
		uint32_t b = ((uint32_t)v0.color.b) & 0xFF;
		uint32_t a = ((uint32_t)v0.color.a) & 0xFF;

		// int part of shade color at xh, floor(yh)
		RDP_WriteWord((r<<16) | g);
		RDP_WriteWord((b<<16) | a);
		// int part change in shade along scanline
		RDP_WriteWord((xCoefFixed.r&0xFFFF0000) | ((xCoefFixed.g>>16)&0xFFFF));
		RDP_WriteWord((xCoefFixed.b&0xFFFF0000) | ((xCoefFixed.a>>16)&0xFFFF));
		// fractional part of shade color at xh, floor(yh)
		RDP_WriteWord(0);
		RDP_WriteWord(0);
		// fractional part change in shade along scanline
		RDP_WriteWord((xCoefFixed.r<<16) | (xCoefFixed.g&0xFFFF));
		RDP_WriteWord((xCoefFixed.b<<16) | (xCoefFixed.a&0xFFFF));
		// int part change along major edge
		RDP_WriteWord((edgeCoefFixed.r&0xFFFF0000) | ((edgeCoefFixed.g>>16)&0xFFFF));
		RDP_WriteWord((edgeCoefFixed.b&0xFFFF0000) | ((edgeCoefFixed.a>>16)&0xFFFF));
		// int part change each scanline
		RDP_WriteWord((yCoefFixed.r&0xFFFF0000) | ((yCoefFixed.g>>16)&0xFFFF));
		RDP_WriteWord((yCoefFixed.b&0xFFFF0000) | ((yCoefFixed.a>>16)&0xFFFF));
		// frac part change along major edge
		RDP_WriteWord((edgeCoefFixed.r<<16) | (edgeCoefFixed.g&0xFFFF));
		RDP_WriteWord((edgeCoefFixed.b<<16) | (edgeCoefFixed.a&0xFFFF));
		// frac part change each scanline
		RDP_WriteWord((yCoefFixed.r<<16) | (yCoefFixed.g&0xFFFF));
		RDP_WriteWord((yCoefFixed.b<<16) | (yCoefFixed.a&0xFFFF));

		// // int part change along major edge
		// RDP_WriteWord(0);
		// RDP_WriteWord(0);
		// // int part change each scanline
		// RDP_WriteWord(0);
		// RDP_WriteWord(0);
		// // frac part change along major edge
		// RDP_WriteWord(0);
		// RDP_WriteWord(0);
		// // frac part change each scanline
		// RDP_WriteWord(0);
		// RDP_WriteWord(0);
	}

	// float minw = 1.0f / max(max(v0.pos.z, v1.pos.z), v2.pos.z);
	// invw0 *= minw;
	// invw1 *= minw;
	// invw2 *= minw;
	// invw0 *= (float)0x7FFF;
	// invw1 *= (float)0x7FFF;
	// invw2 *= (float)0x7FFF;

	// TEXTURE COORDINATES
	if (__rdpEnableTriangleTexture) {
		float longDiffX = v2x-v0x;
		float longDiffY = v2y-v0y;
		float highDiffX = v1x-v0x;
		float highDiffY = v1y-v0y;
		float lowDiffX = v2x-v1x;
		float lowDiffY = v2y-v1y;

		vec3_t texData0 = vec3(v0.texcoord.u, v0.texcoord.v, v0.invw);
		vec3_t texData1 = vec3(v1.texcoord.u, v1.texcoord.v, v1.invw);
		vec3_t texData2 = vec3(v2.texcoord.u, v2.texcoord.v, v2.invw);
		vec3_t highEdgeTexDelta = sub3(texData1, texData0);
		vec3_t longEdgeTexDelta = sub3(texData2, texData0);

		float edgeRatio = divsafe(longDiffX, longDiffY);
		float determinant = highDiffX*longDiffY - longDiffX*highDiffY;

		vec3_t xTexCoef = div3fsafe(sub3(mul3f(highEdgeTexDelta, longDiffY), mul3f(longEdgeTexDelta, highDiffY)), determinant);
		vec3_t yTexCoef = div3fsafe(sub3(mul3f(longEdgeTexDelta, highDiffX), mul3f(highEdgeTexDelta, longDiffX)), determinant);
		vec3_t edgeTexCoef = add3(yTexCoef, mul3f(xTexCoef, edgeRatio));
		
		vec3fx32_t xTexCoefFixed = vec3tofixed32(xTexCoef);
		vec3fx32_t yTexCoefFixed = vec3tofixed32(yTexCoef);
		vec3fx32_t edgeTexCoefFixed = vec3tofixed32(edgeTexCoef);

		uint32_t finalu = tofixed32(texData0.u);
		uint32_t finalv = tofixed32(texData0.v);
		uint32_t finalw = tofixed32(v0.invw);

		// int part at xh, floor(yh)
		RDP_WriteWord((finalu&0xFFFF0000) | ((finalv>>16)&0xFFFF));
		RDP_WriteWord((finalw&0xFFFF0000));
		// int part change along scanline
		RDP_WriteWord((xTexCoefFixed.u&0xFFFF0000) | ((xTexCoefFixed.v>>16)&0xFFFF));
		RDP_WriteWord((xTexCoefFixed.w&0xFFFF0000));
		// fractional part at xh, floor(yh)
		RDP_WriteWord(((finalu&0xFFFF)<<16) | (finalv&0xFFFF));
		RDP_WriteWord(((finalw&0xFFFF)<<16));
		// fractional part along scanline
		RDP_WriteWord((xTexCoefFixed.u<<16) | (xTexCoefFixed.v&0xFFFF));
		RDP_WriteWord((xTexCoefFixed.w<<16));
		// int part change along major edge
		RDP_WriteWord((edgeTexCoefFixed.u&0xFFFF0000) | ((edgeTexCoefFixed.v>>16)&0xFFFF));
		RDP_WriteWord((edgeTexCoefFixed.w&0xFFFF0000));
		// int part change each scanline
		RDP_WriteWord((yTexCoefFixed.u&0xFFFF0000) | ((yTexCoefFixed.v>>16)&0xFFFF));
		RDP_WriteWord((yTexCoefFixed.w&0xFFFF0000));
		// frac part change along major edge
		RDP_WriteWord((edgeTexCoefFixed.u<<16) | (edgeTexCoefFixed.v&0xFFFF));
		RDP_WriteWord((edgeTexCoefFixed.w<<16));
		// frac part change each scanline
		RDP_WriteWord((yTexCoefFixed.u<<16) | (yTexCoefFixed.v&0xFFFF));
		RDP_WriteWord((yTexCoefFixed.w<<16));
	}

	if (__rdpEnableTriangleZBuffer) {
		float longDiffX = v2x-v0x;
		float longDiffY = v2y-v0y;
		float highDiffX = v1x-v0x;
		float highDiffY = v1y-v0y;
		float lowDiffX = v2x-v1x;
		float lowDiffY = v2y-v1y;

		float z0 = v0.pos.z * (float)0x7FFF;// / v0.pos.w;
		float z1 = v1.pos.z * (float)0x7FFF;// / v1.pos.w;
		float z2 = v2.pos.z * (float)0x7FFF;// / v2.pos.w;
		float highEdgeDelta = z1 - z0;
		float longEdgeDelta = z2 - z0;

		float edgeRatio = divsafe(longDiffX, longDiffY);
		float determinant = highDiffX*longDiffY - longDiffX*highDiffY;

		float xCoef = divsafe(highEdgeDelta*longDiffY - longEdgeDelta*highDiffY, determinant);
		float yCoef = divsafe(longEdgeDelta*highDiffX - highEdgeDelta*longDiffX, determinant);
		float edgeCoef = yCoef + xCoef*edgeRatio;
		
		fixed32_t xCoefFixed = tofixed32(xCoef);
		fixed32_t yCoefFixed = tofixed32(yCoef);
		fixed32_t edgeCoefFixed = tofixed32(edgeCoef);

		// uint32_t finalu = tofixed32(texData0.u);
		// uint32_t finalv = tofixed32(texData0.v);
		// uint32_t finalw = tofixed32(v0.invw);
		fixed32_t finalz = tofixed32(z0);

		RDP_WriteWord(finalz);
		// RDP_WriteWord((xCoefFixed&0xFFFF0000) | ((xCoefFixed>>16)&0xFFFF));
		// RDP_WriteWord((xCoefFixed&0xFFFF0000) | ((xCoefFixed>>16)&0xFFFF));
		// RDP_WriteWord((xCoefFixed&0xFFFF0000) | ((xCoefFixed>>16)&0xFFFF));
		RDP_WriteWord(xCoefFixed);
		RDP_WriteWord(edgeCoefFixed);
		RDP_WriteWord(yCoefFixed);
	}
}

void RDP_FillTriangleWithShade(rdp_vertex_t* verts)
{
	rdp_vertex_t v0 = verts[0];
	rdp_vertex_t v1 = verts[1];
	rdp_vertex_t v2 = verts[2];

	RDP_Triangle(v0, v1, v2);
}

void RDP_TextureRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t s0, uint16_t t0, uint16_t s1, uint16_t t1)
{
	// uint32_t w = 100;
	// uint32_t h = 100;
	// // u10.2 format
	// uint32_t x0 = (75) << 2;
	// uint32_t y0 = (150) << 2;
	// uint32_t x1 = (75+w) << 2;
	// uint32_t y1 = (150+h) << 2;
	// RDP_Write(&cmdlist, (0x24<<24) | (x1<<12) | (y1<<0));
	// RDP_Write(&cmdlist, (0<<24) | (x0<<12) | (y0<<0));
	
	// int32_t u = ((0)<<5) & 0xFFFF;
	// int32_t v = ((0)<<5) & 0xFFFF;
	// int32_t du = ((tileWidth)<<10) / 100;
	// int32_t dv = ((tileHeight)<<10) / 100;
	// RDP_Write(&cmdlist, (u<<16) | (v<<0));
	// RDP_Write(&cmdlist, (du<<16) | (dv<<0));

	uint32_t x0 = (x) << 2;
	uint32_t y0 = (y) << 2;
	uint32_t x1 = (x + width) << 2;
	uint32_t y1 = (y + height) << 2;
	RDP_WriteWord(RDPCMD_TEXTURE_RECTANGLE | (x1<<12) | (y1<<0));
	RDP_WriteWord((0<<24) | (x0<<12) | (y0<<0));
	
	int32_t u = (s0<<5) & 0xFFFF;
	int32_t v = (t0<<5) & 0xFFFF;
	int32_t du = (s1<<10) / width;
	int32_t dv = (t1<<10) / height;
	RDP_WriteWord((u<<16) | (v<<0));
	RDP_WriteWord((du<<16) | (dv<<0));
}

void RDP_FillRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
	uint32_t x0 = x<<2;
	uint32_t y0 = y<<2;
	uint32_t x1 = (x+width)<<2;
	uint32_t y1 = (y+height)<<2;

	RDP_WriteWord((0x36<<24) | (x1<<12) | (y1));
	RDP_WriteWord((x0<<12) | (y0));
}

void RDP_ExecuteAndWait()
{
	// DataCacheWritebackInvalidate(cmdlist.buffer, cmdlist.cursor);
	// uint32_t start = (unsigned long)_commandList & 0x1FFFFFFF;
	// uint32_t end = start + cmdlist.cursor;
	// MemoryBarrier();
	// *DP_START = start;
	// MemoryBarrier();
	// *DP_END = end;
	// MemoryBarrier();

	// while (*DP_CURRENT != *DP_END);

	// // TODO: Use interrupt
	// while (*DP_STATUS & (DP_STATUS_TMEM_BUSY | DP_STATUS_PIPE_BUSY));

	DataCacheWritebackInvalidate((void*)_commandListBuffer, _commandListCursor);
	uint32_t start = (unsigned long)_commandList & 0x1FFFFFFF;
	uint32_t end = start + _commandListCursor;
	MemoryBarrier();
	*DP_START = start;
	MemoryBarrier();
	*DP_END = end;
	MemoryBarrier();

	while (*DP_CURRENT != *DP_END);

	// TODO: Use interrupt
	while (*DP_STATUS & (DP_STATUS_TMEM_BUSY | DP_STATUS_PIPE_BUSY));
}
