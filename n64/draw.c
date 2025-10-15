//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>
#include <stdbool.h>

#include "rdp.h"
#include "registers.h"
#include "core64.h"
#include "math.h"
#include "video.h"
#include "gfx.h"


void* __drawFramebuffer;

void SetDrawFramebuffer(void* addr)
{
	__drawFramebuffer = addr;
}

void BlitPixel(int x, int y, uint16_t color)
{
	volatile uint16_t* fb = __drawFramebuffer;
	video_rect_t res = GetResolution();
	if (x >= 0 && x < res.width && y >= 0 && y < res.height) {
		fb[y*res.width + x] = color;
	}
}

void BlitFontTexture()
{
	volatile uint16_t* fb = __drawFramebuffer;

	for (int uvy=0; uvy<96; ++uvy) {
		for (int uvx=0; uvx<48; ++uvx) {
			fb[(100+uvy)*320+(10+uvx)] = N64FontSmall[(95-uvy)*48+uvx];
		}
	}
}

void BlitTexture(void* textureData, int x, int y, int u, int v, int w, int h)
{
	video_rect_t res = GetResolution();
	volatile uint16_t* fb = __drawFramebuffer;

	// uint32_t* tex = textureData;
	uint16_t* tex = textureData;

	for (int yt=0; yt<h; ++yt) {
		for (int xt=0; xt<w; ++xt) {
			uint16_t texel = tex[((v+yt)*32) + u + xt];
			// uint32_t r = (texel>>8) & 31;
			// uint32_t g = (texel>>16) & 31;
			// uint32_t b = (texel>>24) & 31;
			// fb[(y+yt) * res.width + (x+xt)] = (r<<11) | (g<<6) | (b<<1);
			fb[(y+yt) * res.width + (x+xt)] = texel;
		}
	}
}

void DrawFontGlyph(uint32_t* font, uint8_t glyph, int x, int y)
{
	volatile uint16_t* fb = __drawFramebuffer;

	for (int uvy=0; uvy<6; ++uvy) {
		for (int uvx=0; uvx<6; ++uvx) {
			int fontTextureWidth = 48;
			int fontTextureHeight = 96;
			uint32_t fontPixel = font[((glyph/8)*6+(5-uvy))*fontTextureWidth + (glyph%8)*6+uvx];
			if (fontPixel /*font[glyph][uvy*8+uvx]*/) {
				fb[(y+uvy)*320+(x+uvx)] = 0xFFFF;
			}
		}
	}
}

void DrawFontGlyphWithBG(uint32_t* font, uint8_t glyph, int x, int y)
{
	volatile uint16_t* fb = __drawFramebuffer;

	for (int uvy=0; uvy<6; ++uvy) {
		for (int uvx=0; uvx<6; ++uvx) {
			int fontTextureWidth = 48;
			int fontTextureHeight = 96;
			uint32_t fontPixel = font[((glyph/8)*6+(5-uvy))*fontTextureWidth + (glyph%8)*6+uvx];
			if (fontPixel /*font[glyph][uvy*8+uvx]*/) {
				fb[(y+uvy)*320+(x+uvx)] = 0xFFFF;
			} else {
				fb[(y+uvy)*320+(x+uvx)] = (31<<11) | (31<<1);
			}
		}
	}
}

void DrawFontString(uint32_t* font, char* str, int x, int y)
{
	int len = strsize(str);
	for (int idx=0; idx<len; ++idx) {
		uint8_t c = str[idx];
		if (c >= 'a' && c <= 'z') {
			c = 'A' + (c-'a');
		}
		DrawFontGlyph(font, c, x+idx*6, y);
	}
}

void DrawFontStringWithBG(uint32_t* font, char* str, int x, int y)
{
	int len = strsize(str);
	for (int idx=0; idx<len; ++idx) {
		uint8_t c = str[idx];
		if (c >= 'a' && c <= 'z') {
			c = 'A' + (c-'a');
		}
		DrawFontGlyphWithBG(font, c, x+idx*6, y);
	}
}

void DrawStr(int x, int y, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char str[256];
	vsprint(str, 256, fmt, args);
	DrawFontString(N64Font, str, x, y);

	va_end(args);
}

void DrawStrBG(int x, int y, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char str[256];
	vsprint(str, 256, fmt, args);
	DrawFontStringWithBG(N64Font, str, x, y);

	va_end(args);
}

#if 0
void DrawLine(float v0x, float v0y, float v1x, float v1y)
{
	volatile uint16_t* fb = __drawFramebuffer;
	video_rect_t res = GetResolution();
	// vec2_t l0 = {-50*cos, -50*sin};
	// vec2_t l1 = {+50*cos, +50*sin};
	// v0x += 100;
	// v0y += 100;
	// v1x += 100;
	// v1y += 100;

	// fxhi2_t v0 = {FloatToFxhi(v0x), FloatToFxhi(v0y)};
	// fxhi2_t v1 = {FloatToFxhi(v1x), FloatToFxhi(v1y)};
	// fxhi2_t diff = {v1.x - v0.x, v1.y - v0.y};
	// // fxhi_t stepDir = max(abs(diff.x), abs(diff.y));
	// fxhi2_t coef = diff;
	// fxhi_t* step = &coef.x;
	// fxhi_t* partial = &coef.y;
	// if (abs(*partial) > abs(*step)) {
	// 	swap(*step, *partial);
	// }

	// *step = (*step / abs(*step)) * FxhiMultiplier;
	// *partial = ((int64_t)diff.y * FxhiMultiplier) / diff.x;

	// // fxhi_t xcoef = max(min(((int64_t)diff.x * FxhiMultiplier) / diff.y, FxhiMultiplier), -FxhiMultiplier);
	// // fxhi_t ycoef = max(min(((int64_t)diff.y * FxhiMultiplier) / diff.x, FxhiMultiplier), -FxhiMultiplier);

	// // for (int i=0; i<(l1.x-l0.x); ++i) {
	// // 	fb[((int)l0.y+i)*res.width + ((int)l0.x+i)] = 0xFFFF;
	// // }
	// fb[((int)v0y)*res.width + ((int)v0x)] = 31<<1;
	// fb[((int)v1y)*res.width + ((int)v1x)] = 31<<1;
	// // for (;;) {
	// // 	fb[(v0.y>>16)*res.width + (v0.x>>16)] = 0xFFFF;
	// // 	v0.x += coef.x;
	// // 	v0.y += coef.y;

	// // 	if (v0.x>>16 == v1.x>>16 && v0.y>>16 == v1.y>>16) {
	// // 		break;
	// // 	}
	// // }
	// for (int i=0; i<max(abs(diff.x>>16), abs(diff.y>>16)); ++i) {
	// 	// fb[((int)l0.y+i)*res.width + ((int)l0.x+i)] = 0xFFFF;
	// 	// fb[(v0.y>>16)*res.width + (v0.x>>16)] = 0xFFFF;
	// 	BlitPixel(v0.x>>16, v0.y>>16, 0xFFFF);
	// 	v0.x += coef.x;
	// 	v0.y += coef.y;
	// }

	fb[((int)v0y)*res.width + ((int)v0x)] = 31<<1;
	fb[((int)v1y)*res.width + ((int)v1x)] = 31<<1;

	vec2_t v0 = {v0x, v0y};
	vec2_t v1 = {v1x, v1y};
	float diffx = v1.x - v0.x;
	float diffy = v1.y - v0.y;
	bool steep = abs(diffy) > abs(diffx);
	// if (steep) {
	// 	swap(v0.x, v0.y);
	// 	swap(v1.x, v1.y);
	// }
	int i0 = steep;
	int i1 = !i0;
	if (v0.f[i0] > v1.f[i0]) {
		swap(v0, v1);
	}

	float len = v1.f[i0] - v0.f[i0];
	float ratio = (v1.f[i1]-v0.f[i1]) / len;

	for (int i=0; i<len; ++i) {
		vec2_t p;
		p.f[i1] = v0.f[i1] + ratio*(float)i;
		p.f[i0] = v0.f[i0] + i;
		BlitPixel(p.x, p.y, 0xFFFF);
	}
}
#endif

void DrawLine(float v0x, float v0y, float v1x, float v1y, color16_t color)
{
	BlitPixel(v0x, v0y, 31<<1);
	BlitPixel(v1x, v1y, 31<<1);

	// vec2_t v0 = {v0x, v0y};
	// vec2_t v1 = {v1x, v1y};
	fxhi_t v0[] = {FloatToFxhi(v0x), FloatToFxhi(v0y)};
	fxhi_t v1[] = {FloatToFxhi(v1x), FloatToFxhi(v1y)};
	fxhi_t diffx = v1[0] - v0[0];
	fxhi_t diffy = v1[1] - v0[1];
	bool steep = abs(diffy) > abs(diffx);
	int i0 = steep;
	int i1 = !i0;
	if (v0[i0] > v1[i0]) {
		swap(v0[0], v1[0]);
		swap(v0[1], v1[1]);
	}

	int32_t len = ((v1[i0]-v0[i0]) >> 16) + 1;
	fxhi_t ratio = (v1[i1]-v0[i1]) / len;

	for (int i=0; i<len; ++i) {
		int32_t p[2];
		p[i1] = (v0[i1] + ratio*i) >> 16;
		p[i0] = (v0[i0] >> 16) + i;
		BlitPixel(p[0], p[1], color);
	}
}
