//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef CORE64_H
#define CORE64_H

#include <stdint.h>
#include <stdarg.h>


// SYSTEM
void Log(char* str, ...);
void UpdateLogs();
uint32_t GetDeltaTime();

// PRINT
uint32_t strsize(char* str);
int sprint(char* buf, int len, char* fmt, ...);
int vsprint(char* buf, int len, char* fmt, va_list args);

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

// ASM
uint32_t _GetClock();


#endif
