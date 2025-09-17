//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef CORE64_H
#define CORE64_H

#include <stdint.h>


// UTIL
#define assert(exp) if (!(exp)) { DrawFontStringWithBG(N64Font, "AssertionFired", 320/2-7, 10); (*(volatile int*)0)=0; }

// #define swap(a, b) a=b-a; b=b-a; a=b+a;
#define swap(a, b) { typeof(a) __tmp=a; a=b; b=__tmp; }

// PRINT
uint32_t strsize(char* str);
int sprint(char* buf, int len, char* fmt, ...);

// VIDEO
void HandleVideoInterrupt();
void ResetVideoCurrentLine();

// DRAW
void BlitFontTexture();
void DrawFontGlyph(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontGlyphWithBG(uint32_t* font, uint8_t glyph, int x, int y);
void DrawFontString(uint32_t* font, char* str, int x, int y);
void DrawFontStringWithBG(uint32_t* font, char* str, int x, int y);

// FONTS
#include "../_resources/n64font.h"
#include "../_resources/n64fontwide.h"


#endif
