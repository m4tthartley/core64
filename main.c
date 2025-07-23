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

// typedef unsigned long size_t;

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

void* memcpy(void* dest, const void* src, size_t num)
{
	for (int i=0; i<num; ++i) {
		((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
	}

	return dest;
}

float* CopyFloats(float* out, float* in, size_t num)
{
	for (int i=0; i<num; ++i) {
		// float a = out[i] * in[i];
		// out[i] = a;
		out[i] = in[i];
	}

	return out;
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

#define victrl		0 /* 0000 */
#define viorigin	1 /* 0004 */
#define viwidth		2 /* 0008 */
#define viintr		3 /* 000C */
#define vicurrent	4 /* 0010 */
#define viburst		5 /* 0014 */
#define vivsync		6 /* 0018 */
#define vihsync		7 /* 001C */
#define visyncleap	8 /* 0020 */
#define vihvideo	9 /* 0024 */
#define vivvideo	10 /* 0028 */
#define vivburst	11 /* 002C */
#define vixscale	12 /* 0030 */
#define viyscale	13 /* 0034 */

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

	uint32_t* viregs = (uint32_t*)VI_BASE;

	viregs[victrl] = 0b11;
	viregs[viorigin] = FRAMEBUFFER;
	viregs[viwidth] = 320*1;
	viregs[viintr] = 0x200;
	viregs[vicurrent] = 0;
	viregs[viburst] = 0x03E52239;
	viregs[vivsync] = 0x0000020D;
	viregs[vihsync] = 0x00000C15;
	viregs[visyncleap] = 0x0C150C15;
	viregs[vihvideo] = 0x006c02ec;
	viregs[vivvideo] = 0x002501ff;
	viregs[vivburst] = 0;
	viregs[vixscale] = 0x00000200;
	viregs[viyscale] = 0x00000400;

	// NOTE: This only works if I'm setting them directly like this ^
	// if I do it witht the memcpy it doesn't work, look into why

	// memcpy((void*)VI_BASE, viregs, sizeof(viregs));

	// uint16_t color = 0;
	// uint16_t green = 0;

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

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	for (;;) {
		// uint16_t c = (y & 0x1F) << 11;
		for (int y=0; y<240; ++y) {
			for (int x=0; x<320; ++x) {
				// fb[y*320+x] = (b+=1)<<8 | (g+=2)<<16 | (r+=3)<<24;
				// fb[y*320+x] = ((b+=1)%32) | ((g+=2)%32)<<5 | ((r+=4)%32)<<10;

				uint32_t r = Rand32();

				fb[y*320+x] = r;
				// fb[y*320+x] = clearColor;
				// ++clearColor;
			}
		}

		fb[0*320+10] = 0xFFFF;

		// for (int i=0; i<256; ++i) {
		// 	fa[i] *= fb[i];
		// }

		WaitForVideoSync();
	}

	return 0;
}
