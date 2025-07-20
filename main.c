//
//  Created by Matt Hartley on 20/07/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>

void* memset(void* dest, int value, size_t size)
{
	void* result = dest;
	while (size) {
		*(uint8_t*)dest = (uint8_t)value;
		++dest;
		--size;
	}

	return result;
}

#define VI_BASE 0xA4400000
#define VI_CURSOR (VI_BASE + 0x10)

#define CONTROL (void*)(VI_BASE+0x00)
#define ORIGIN (void*)(VI_BASE+0x04)
#define WIDTH (void*)(VI_BASE+0x08)
#define INTR (void*)(VI_BASE+0x0C)
#define CURSOR (void*)(VI_BASE+0x10)
#define BURST (void*)(VI_BASE+0x14)
#define VSYNC (void*)(VI_BASE+0x18)
#define HSYNC (void*)(VI_BASE+0x1C)
#define LEAP (void*)(VI_BASE+0x20)
#define HSTART (void*)(VI_BASE+0x24)
#define VSTART (void*)(VI_BASE+0x28)
#define XSCALE (void*)(VI_BASE+0x2C)
#define YSCALE (void*)(VI_BASE+0x30)

#define FRAMEBUFFER 0xA0200000

void WaitForVideoSync()
{
	while (*(volatile uint32_t*)VI_CURSOR < 0x200);
	while (*(volatile uint32_t*)VI_CURSOR >= 0x200);
}

void SetVI(void* loc, uint32_t value)
{
	*(volatile uint32_t*)loc = value;
}

int main()
{
	// lol we don't even have printf
	// printf("Hello N64 \n");

	// float fa[256] = {0};
	// float fb[256] = {0};
	// for (int i=0; i<256; ++i) {
	// 	fa[i] = (float)i;
	// 	fb[i] = (float)i;
	// }

	SetVI(CONTROL, 0b11 | (0b0011 << 12));
	SetVI(ORIGIN, FRAMEBUFFER);
	SetVI(WIDTH, 320*2);
	SetVI(INTR, 0x200);
	SetVI(CURSOR, 0);
	SetVI(BURST, 0x03E52239);
	SetVI(VSYNC, 0x0000020D /*240*/);
	SetVI(HSYNC, 0x00000C15 /*3093*/);
	SetVI(LEAP, 0x0C150C15 /*(320*2)<<16 | (320*2)*/);
	SetVI(HSTART, /*0x002501FF*/ /*(108<<16) | 748*/ 0x006c02ec);
	SetVI(VSTART, /*0x0000020D*/ /*(0x025<<16) | 0x1FF*/ 0x002501ff);
	SetVI(XSCALE, 0x00000200);
	SetVI(YSCALE, 0x00000400);

	uint16_t color = 0;
	uint16_t green = 0;

	volatile uint32_t* fb = (void*)FRAMEBUFFER;

	// for (int y=0; y<240; ++y) {
	// 	uint16_t c = 31;//(y & 0x1F) << 11;
	// 	for (int x=0; x<320; x += 1) {
	// 		fb[y*320+x] = color | (green<<5) | (y%31 << 11);
	// 		if (!color) {
	// 			color = 31;
	// 		} else {
	// 			color = 0;
	// 		}
	// 		// color += 1;
	// 		++green;
	// 		green = green % 63;
	// 	}

	// }

	// fb[0] = 0xFFFF;
	// fb[320 * 100] = 0xF800;
	// fb[320 * 200] = 0xF800;
	// fb[320 * 239] = 0xF800;
	// fb[320 * 1] = 0xF800;
	// fb[320 * 3] = 0xF800;

	// for (int y=0; y<2; ++y) {
	// 	fb[y*320+0] = 31;
	// }

	for (;;) {
		// uint16_t c = (y & 0x1F) << 11;
		for (int y=0; y<240; ++y) {
			for (int x=0; x<320; ++x) {
				fb[y*320+x] = (y & 1) ? 0xFFFFFFFF : 0;
				// fb[y*320+x] = color;
			}
		}

		fb[0*320+10] = 0xFFFF;

		// for (int i=0; i<256; ++i) {
		// 	fa[i] *= fb[i];
		// }

		WaitForVideoSync();
	}
}
