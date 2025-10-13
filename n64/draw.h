/*
	Created by Matt Hartley on 12/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __DRAW_HEADER__
#define __DRAW_HEADER__

#include <stdint.h>


void BlitFontTexture();
void BlitTexture(void* textureData, int x, int y, int u, int v, int w, int h);
void DrawFontGlyph(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontGlyphWithBG(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontString(uint32_t* font, char* str, int x, int y);
void DrawFontStringWithBG(uint32_t* font, char* str, int x, int y);
void DrawStr(int x, int y, char* fmt, ...);
void DrawStrBG(int x, int y, char* fmt, ...);


#endif
