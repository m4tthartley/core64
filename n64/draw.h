/*
	Created by Matt Hartley on 12/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __DRAW_HEADER__
#define __DRAW_HEADER__

#include <stdint.h>


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

void SetDrawFramebuffer(void* addr);
void BlitPixel(int x, int y, uint16_t color);
void BlitFontTexture();
void BlitTexture(void* textureData, int x, int y, int u, int v, int w, int h);
void DrawFontGlyph(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontGlyphWithBG(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontString(uint32_t* font, char* str, int x, int y);
void DrawFontStringWithBG(uint32_t* font, char* str, int x, int y);
void DrawStr(int x, int y, char* fmt, ...);
void DrawStrBG(int x, int y, char* fmt, ...);
void DrawLine(float v0x, float v0y, float v1x, float v1y, color16_t color);


#endif
