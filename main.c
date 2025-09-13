//
//  Created by Matt Hartley on 20/07/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

/*
	TODO
	-	Separate ROM and RAM memory
		data shouldn't be copied over to RAM automatically
		as apparently that won't work on real hardware
		research this

	- Flush cache after copying data to RAM
*/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "print.c"
#include "util.c"
#include "video.c"
#include "resources/n64font.h"
#include "resources/n64fontwide.h"


void WaitForVideoSync()
{
	volatile uint32_t* viregs = (uint32_t*)VI_BASE;
	while (viregs[VI_CURRENTLINE] < 0x200);
	while (viregs[VI_CURRENTLINE] >= 0x200);
}

void SetVI(void* loc, uint32_t value)
{
	*(volatile uint32_t*)loc = value;
}

#if 0
void DrawRandomIntoFramebuffer32()
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	for (int y=0; y<240; ++y) {
		for (int x=0; x<320; ++x) {
			uint32_t r = Rand32();
			fb[y*320+x] = r;
		}
	}
}
#endif

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

int main()
{
	InitDefaultVI();

	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	uint16_t color = 0;
	uint32_t fbIndex = 0;
	for (;;) {
		for (int idx=0; idx<320; ++idx) {
			fb[fbIndex] = (color) ;
			++fbIndex;
			fbIndex %= (320*240);
			++color;
		}

		fb[10*320+10] = 0xFFFF;
		fb[10*320+11] = 0xF0F0;

		DrawFontString(N64Font, "HELLO WORLD 256.4096", 20, 30);

		if (fbIndex > 1024) {
			// Trigger error to test exceptions
			// int* asd = 0;
			// *asd = 5;
		}

		// WaitForVideoSync();
		fb[10*320+20] = 0xFFFF;
	}


	return 0;
}

typedef struct {
	uint32_t status;
	uint32_t cause;
	uint32_t epc;
	uint32_t addr;

	uint32_t sp;
} exceptionframe_t;

char* GetExcCodeName(uint8_t code)
{
	switch (code) {
		case 0: return "Interrupt";

		case 1: return "TLB Modification";
		case 2: return "TLB Load Miss";
		case 3: return "TLB Store Miss";

		case 4: return "Address Load Error";
		case 5: return "Address Store Error";

		case 6: return "Hardware Bus Error (instruction fetch)";
		case 7: return "Hardware Bus Error (load/store data)";

		case 8: return "Syscall";
		case 9: return "Breakpoint";
		case 10: return "Invalid Instruction";
		case 11: return "Coprocessor Unusable";
		case 12: return "Arithmetic Overflow";
		case 13: return "Trap";
		case 15: return "Floating Point Error";
		case 23: return "Watch Interrupt";

		default: return "Unknown Exception";
	}
}

char* GetInterruptName(uint8_t code)
{
	switch (code) {
		case MI_INTERRUPT_SP: return "Mi_InterruptSp";
		case MI_INTERRUPT_SI: return "Mi_InterruptSi";
		case MI_INTERRUPT_AI: return "Mi_InterruptAi";
		case MI_INTERRUPT_VI: return "Mi_InterruptVi";
		case MI_INTERRUPT_PI: return "Mi_InterruptPi";
		case MI_INTERRUPT_DP: return "Mi_InterruptDp";

		default: return "Unknown Interrupt";
	}
}

void ExceptionHandler(exceptionframe_t* frame)
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	volatile uint32_t* miregs = (uint32_t*)MI_BASE;
	uint32_t miStatus = miregs[MI_INTERRUPT] & miregs[MI_INTERRUPT_MASK];

	// for (int y=0; y<240; ++y) {
	// 	for (int x=0; x<320; ++x) {
	// 		fb[y*320+x] = (31<<11) | (31<<1);
	// 	}
	// }

	// Interrupt, this is just to visualize interrupt info for debugging
	if (!(frame->cause & 0xFF)) {
		char str[256];
		sprint(str, 256, "Interrupt: %i %s", miStatus, GetInterruptName(miStatus));
		DrawFontStringWithBG(N64Font, str, 10, 5);
		sprint(str, 256, "Cause: %32x, %u", frame->cause, frame->cause);
		DrawFontStringWithBG(N64Font, str, 10, 15);

		sprint(str, 256, "Mi Interrupt: %32x", miregs[MI_INTERRUPT]);
		DrawFontStringWithBG(N64Font, str, 10, 25);
		sprint(str, 256, "Mi Interrupt Mask: %32x", miregs[MI_INTERRUPT_MASK]);
		DrawFontStringWithBG(N64Font, str, 10, 35);

		if (miStatus & MI_INTERRUPT_SP) {
			DrawFontStringWithBG(N64Font, "SP Interrupt", 10, 45);
		}
		if (miStatus & MI_INTERRUPT_SI) {
			DrawFontStringWithBG(N64Font, "SI Interrupt", 10, 45);
		}
		if (miStatus & MI_INTERRUPT_AI) {
			DrawFontStringWithBG(N64Font, "AI Interrupt", 10, 45);
		}
		if (miStatus & MI_INTERRUPT_VI) {
			DrawFontStringWithBG(N64Font, "VI Interrupt", 10, 45);
		}
		if (miStatus & MI_INTERRUPT_PI) {
			DrawFontStringWithBG(N64Font, "PI Interrupt", 10, 45);
		}
		if (miStatus & MI_INTERRUPT_DP) {
			DrawFontStringWithBG(N64Font, "DP Interrupt", 10, 45);
		}

		return;
	}

	// Exception info
	DrawFontStringWithBG(N64Font, "CPU Exception", 10, 5);
	
	uint8_t cause = (frame->cause & 0b01111100) >> 2;
	
	char str[256];
	sprint(str, 256, "ExcCode(%u)  %s", cause, GetExcCodeName(cause));
	DrawFontStringWithBG(N64Font, str, 10, 25);

	sprint(str, 256, "Stack pointer: %32x", frame->sp);
	DrawFontStringWithBG(N64Font, str, 10, 45);
	sprint(str, 256, "Status: %32x", frame->status);
	DrawFontStringWithBG(N64Font, str, 10, 55);
	sprint(str, 256, "Cause: %8x, %u", cause, cause);
	DrawFontStringWithBG(N64Font, str, 10, 65);
	sprint(str, 256, "Bad Address: %32x", /**(uint32_t*)0xA0001000*/ frame->addr);
	DrawFontStringWithBG(N64Font, str, 10, 75);
	sprint(str, 256, "EPC: %32x", frame->epc);
	DrawFontStringWithBG(N64Font, str, 10, 85);
}
