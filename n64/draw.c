//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>

#include "registers.h"
#include "core64.h"
#include "video.h"


void BlitFontTexture()
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	for (int uvy=0; uvy<96; ++uvy) {
		for (int uvx=0; uvx<48; ++uvx) {
			fb[(100+uvy)*320+(10+uvx)] = N64FontSmall[(95-uvy)*48+uvx];
		}
	}
}

void BlitTexture(void* textureData, int x, int y, int u, int v, int w, int h)
{
	video_rect_t res = GetResolution();
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

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
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

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
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

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
