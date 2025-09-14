//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>

#include "system.h"


void BlitFontTexture()
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	for (int uvy=0; uvy<96; ++uvy) {
		for (int uvx=0; uvx<48; ++uvx) {
			fb[(100+uvy)*320+(10+uvx)] = N64FontSmall[(95-uvy)*48+uvx];
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
	int len = strlen(str);
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
	int len = strlen(str);
	for (int idx=0; idx<len; ++idx) {
		uint8_t c = str[idx];
		if (c >= 'a' && c <= 'z') {
			c = 'A' + (c-'a');
		}
		DrawFontGlyphWithBG(font, c, x+idx*6, y);
	}
}
