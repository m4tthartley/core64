//
//  Created by Matt Hartley on 08/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>


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

#define VI_MODE_16BIT 0b10
#define VI_MODE_32BIT 0b11

#define VI_AA_MODE_ENABLED (0b00<<8)
#define VI_AA_MODE_JUST_RESAMPLING (0b10<<8)
#define VI_AA_MODE_DISABLED (0b11<<8)

inline void SetVIRegister(uint32_t reg, uint32_t value)
{
	((uint32_t*)(VI_BASE))[reg] = value;
	// TODO: MEMORY BARRIER?
}

void InitDefaultVI()
{
	uint32_t* viregs = (uint32_t*)VI_BASE;	

	uint32_t controlMode = VI_MODE_16BIT | VI_AA_MODE_DISABLED; //0b00000000000000000000001100000010;
	viregs[victrl] = controlMode; //0b10;
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
	viregs[vivburst] = 0x000E0204;
	viregs[vixscale] = 0x00000200;
	viregs[viyscale] = 0x00000400;
}

void InitDefaultVIWithMemcpy()
{
	uint32_t viregs[14];
	uint32_t controlMode = 0b00000000000000000000001100000010;
	viregs[victrl] = controlMode; //0b10;
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
	viregs[vivburst] = 0x000E0204;
	viregs[vixscale] = 0x00000200;
	viregs[viyscale] = 0x00000400;

	// NOTE: This only works if I'm setting them directly like this ^
	// if I do it witht the memcpy it doesn't work, look into why

	// CopyMemory((void*)VI_BASE, (uint8_t*)viregs, sizeof(viregs));

	int num = 14;//sizeof(viregs);
	uint32_t* dest = (void*)VI_BASE;
	uint32_t* src = viregs;
	while (num) {
		*dest++ = *src++;
		--num;
	}
}
