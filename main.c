//
//  Created by Matt Hartley on 20/07/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

/*
	TODO
	-	Separate ROM and RAM memory
		data shouldn't be copied over to RAM automatically
		as apparently that won't work on real hardware

	- Setup and copy to RAM .sdata, .sbss, .rodata?
	
	- Set up $gp small data pointer

	- Flush cache after copying data to RAM
*/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "util.c"
#include "video.c"
#include "resources/n64font.h"
#include "resources/n64fontwide.h"


void WaitForVideoSync()
{
	while (*(volatile uint32_t*)VI_CURSOR < 0x200);
	while (*(volatile uint32_t*)VI_CURSOR >= 0x200);
}

void SetVI(void* loc, uint32_t value)
{
	*(volatile uint32_t*)loc = value;
}

uint32_t randState32 = 0;//2463534242; // TODO: If I set this to zero it crashes
uint64_t pcgState = 0x853C49E6748FEA9BULL;
uint64_t pcgInc = 0xDA3E39CB94B95BDBULL;

uint32_t RandXorShift32()
{
	if (!randState32) {
		randState32 = 2463534242;
	}

	randState32 ^= randState32 << 13;
	randState32 ^= randState32 << 17;
	randState32 ^= randState32 << 5;
	return randState32;
}

uint32_t RandLCG()
{
	if (!randState32) {
		randState32 = 1;
	}
	randState32 = 1664525 * randState32 + 1013904223;
	return randState32;
}

uint32_t RandPCG()
{
	uint64_t oldState = pcgState;
	pcgState = oldState * 6364136223846793005ULL + (pcgInc | 1);
	uint32_t xor = ((oldState >> 18u) ^ oldState) >> 27u;
	uint32_t rot = oldState >> 59u;
	return (xor >> rot) | (xor << ((-rot) & 31));
}

uint32_t Rand32()
{
	return RandPCG();
}

uint32_t Rand32Range(uint32_t min, uint32_t max)
{
	uint32_t range = max - min;
	return min + (Rand32() % range);
}

// int32_t Rand32RangeSigned(int32_t min, int32_t max)
// {

// }

extern uint8_t __bss_start;
extern uint8_t __bss_end;

uint16_t clearColor = 196<<8;
uint16_t clearColor2 = 31;
uint16_t clearColor3 = 31;
uint16_t clearColor4 = 31;
uint8_t clearColor5 = 31;
uint8_t clearColor6 = 31;
uint8_t clearColor7 = 31;

uint8_t font[][8*8] = {
	// {
	// 	0,0,0,0,0,
	// 	0,0,0,0,0,
	// 	0,0,0,0,0,
	// 	0,0,0,0,0,
	// 	0,0,0,0,0,
	// },
	{
		0,1,1,1,0,0,0,0,
		1,0,0,0,1,0,0,0,
		1,0,0,0,1,0,0,0,
		1,1,1,1,1,0,0,0,
		1,0,0,0,1,0,0,0,
		1,0,0,0,1,0,0,0,
		1,0,0,0,1,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	{
		1,1,1,1,0,0,0,0,
		1,0,0,0,1,0,0,0,
		1,0,0,1,0,0,0,0,
		1,0,0,0,1,0,0,0,
		1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	{
		0,1,1,1,1,0,0,0,
		1,0,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0,
		0,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	{
		0,1,0,1,0,0,0,0,
		1,0,1,0,1,0,0,0,
		0,1,0,1,0,0,0,0,
		1,0,1,0,1,0,0,0,
		0,1,0,1,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	}
};

void DrawFontGlyph(uint32_t* font, uint8_t glyph, int x, int y)
{
	volatile uint16_t* fb = (void*)FRAMEBUFFER;

	for (int uvy=0; uvy<6; ++uvy) {
		for (int uvx=0; uvx<6; ++uvx) {
			int fontTextureWidth = 48;
			int fontTextureHeight = 96;
			uint32_t fontPixel = font[((glyph/8)*6+(5-uvy))*fontTextureWidth + (glyph%8)*6+uvx];
			if (fontPixel /*font[glyph][uvy*8+uvx]*/) {
				fb[(y+uvy)*320+(x+uvx)] = 0xFFFF;
			} else {
				fb[(y+uvy)*320+(x+uvx)] = 0x0000;
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

void FormatString(char* str, ...)
{
	va_list va;
	va_start(va, str);
}

int main()
{
	// memset(&__bss_start, 0, &__bss_end-&__bss_start);
	// memset(&pcgState, 0, &pcgInc-&pcgState);
	// float x = 5;
	// float y = 5;
	// float z = 5;
	// uint16_t w = 5;

	// lol we don't even have printf
	// printf("Hello N64 \n");

	// float fa[256] = {0};
	// float fb[256] = {0};
	// for (int i=0; i<256; ++i) {
	// 	fa[i] = (float)i;
	// 	fb[i] = (float)i;
	// }

	// SetVI(CONTROL, 0b11 | (0b0011 << 12));
	// SetVI(ORIGIN, FRAMEBUFFER);
	// SetVI(WIDTH, 320*2);
	// SetVI(INTR, 0x200);
	// SetVI(CURSOR, 0);
	// SetVI(BURST, 0x03E52239);
	// SetVI(VSYNC, 0x0000020D /*240*/);
	// SetVI(HSYNC, 0x00000C15 /*3093*/);
	// SetVI(LEAP, 0x0C150C15 /*(320*2)<<16 | (320*2)*/);
	// SetVI(HSTART, /*0x002501FF*/ /*(108<<16) | 748*/ 0x006c02ec);
	// SetVI(VSTART, /*0x0000020D*/ /*(0x025<<16) | 0x1FF*/ 0x002501ff);
	// SetVI(XSCALE, 0x00000200);
	// SetVI(YSCALE, 0x00000400);

	// uint32_t viregs[] = {
	// 	0x00000000, 0x00000000, 0x00000000, 0x00000002,
	// 	0x00000000, 0x03E52239, 0x0000020D, 0x00000C15,
	// 	0x0C150C15, 0x006C02EC, 0x002501FF, 0x000E0204,
	// 	0x00000000, 0x00000000,
	// };

	// // viregs[viorigin] = FRAMEBUFFER;
	// // viregs[viwidth] = 320*2;
	// // viregs[vixscale] = 0x00000200;
	// // viregs[viyscale] = 0x00000400;
	// // viregs[victrl] = 0b11 ;//| (0b0011 << 12);

	InitDefaultVI();

	volatile uint16_t* fb = (void*)FRAMEBUFFER;

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	for (;;) {
		// for (int y=0; y<240; ++y) {
		// 	for (int x=0; x<320; ++x) {

		// 		uint32_t r = Rand32();

		// 		fb[y*320+x] = r;
		// 	}
		// }

		fb[10*320+10] = 0xFFFF;
		fb[10*320+11] = 0xF0F0;
		// fb[10*320+12] = 0x00FF00FF;
		// fb[10*320+13] = 0x0000FFFF;

		// for (int i=0; i<256; ++i) {
		// 	fa[i] *= fb[i];
		// }

		for (int uvy=0; uvy<96; ++uvy) {
			for (int uvx=0; uvx<48; ++uvx) {
				fb[(100+uvy)*320+(10+uvx)] = N64FontSmall[(95-uvy)*48+uvx];
			}
		}

		DrawFontGlyph(N64FontSmall, 'A', 20, 20);
		DrawFontGlyph(N64FontSmall, 'B', 26, 20);
		DrawFontGlyph(N64FontSmall, 'C', 32, 20);

		DrawFontString(N64FontSmall, "HELLO WORLD 256.4096", 20, 30);
		DrawFontString(N64Font, "HELLO WORLD 256.4096", 20, 40);
		DrawFontString(N64Font, "1089108398274985 01928302938493567", 20, 50);
		DrawFontString(N64Font, "The quick brown fox jumps over the lazy dog", 20, 60);

		// int x = 20;
		// int y = 20;
		// for (int uvy=0; uvy<5; ++uvy) {
		// 	for (int uvx=0; uvx<5; ++uvx) {
		// 		if (font[0][uvy*5+uvx]) {
		// 			fb[(y+uvy)*320+(x+uvx)] = 0xFFFF;
		// 		} else {
		// 			fb[(y+uvy)*320+(x+uvx)] = 0x0000;
		// 		}
		// 	}
		// }
		// x = 20;
		// y = 30;
		// for (int uvy=0; uvy<5; ++uvy) {
		// 	for (int uvx=0; uvx<5; ++uvx) {
		// 		if (font[1][uvy*5+uvx]) {
		// 			fb[(y+uvy)*320+(x+uvx)] = 0xFFFFFFFF;
		// 		} else {
		// 			fb[(y+uvy)*320+(x+uvx)] = 0x00000000;
		// 		}
		// 	}
		// }

		WaitForVideoSync();
	}

	return 0;
}
