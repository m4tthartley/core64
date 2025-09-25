//
//  Created by Matt Hartley on 08/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>
#include <stdbool.h>

#include "n64def.h"


volatile uint32_t __viInterruptCounter = 0;
volatile uint32_t __viInterrupt = 0;

extern volatile uint32_t __tvType;

inline void SetVIRegister(uint32_t reg, uint32_t value)
{
	((uint32_t*)(VI_BASE))[reg] = value;
	// TODO: MEMORY BARRIER?
}

void EnableVideoInterrupts()
{
	uint32_t* interruptMask = ((uint32_t*)MI_BASE) + MI_INTERRUPT_MASK;
	*interruptMask = MI_INT_VI_SET ;//| MI_INT_DP_SET;
}

typedef struct {
	uint32_t tvType;
	uint32_t resetType;
	void* reserved;
	uint32_t memorySize;
	uint32_t consoleType;
	uint32_t version;
} bootinfo_t;

uint32_t burstDefaultValues[] = {
	// /*PAL*/ 0x03E52239,
	/*PAL*/ 0x0404233A,
	/*NTSC*/ 0x03E52239,
	/*MPAL*/ 0x03E52239,
};

uint32_t vtotalDefaultValues[] = {
	/*PAL*/ 625,
	/*NTSC*/ 525,
	/*MPAL*/ 525,
};

uint32_t htotalDefaultValues[] = {
	/*PAL*/ 3177,
	/*NTSC*/ 3093,
	/*MPAL*/ 3090,
};

uint32_t hTotalLeap[] = {
	/*PAL*/ (3182 << 16) | 3183,
	/*NTSC*/ 0x0C150C15,
	/*MPAL*/ 0x0C150C15,
};

uint32_t hvideoStart[] = {
	/*PAL*/ 128,
	/*NTSC*/ 108,
	/*MPAL*/ 108,
};
uint32_t hvideoEnd[] = {
	/*PAL*/ 768,
	/*NTSC*/ 748,
	/*MPAL*/ 748,
};

uint32_t vvideoStart[] = {
	// /*PAL*/ 144,
	// /*PAL*/ 0x02A,
	// /*PAL*/ 0x05F,
	/*PAL*/ 0x05F - 48,
	/*NTSC*/ 0x025,
	/*MPAL*/ 0x025,
};
uint32_t vvideoEnd[] = {
	// /*PAL*/ 431,
	// /*PAL*/ 0x26A,
	// /*PAL*/ 0x239,
	/*PAL*/ 0x239 + 48,
	/*NTSC*/ 0x1FF,
	/*MPAL*/ 0x1FF,
};

uint32_t vburst[] = {
	/*PAL*/ (0x009 << 16) | 0x26B,
	/*NTSC*/ (0x00E << 16) | 0x204,
	/*MPAL*/ (0x00E << 16) | 0x204,
};

void InitDefaultVI()
{
	// uint32_t* miregs = (uint32_t*)MI_BASE;
	volatile uint32_t* viregs = (uint32_t*)VI_BASE;

	// __tvType = 1;
	
	uint32_t controlMode = VI_COLOR_MODE_16 | VI_AA_DISABLED;
	viregs[VI_CONTROL] = controlMode;
	viregs[VI_ORIGIN] = VI_FRAMEBUFFERBASE;
	viregs[VI_WIDTH] = 320*1;
	// viregs[VI_INTLINE] = 2;//0x200;
	viregs[VI_CURRENTLINE] = 0;
	// viregs[VI_BURST] = 0x03E52239;
	viregs[VI_BURST] = burstDefaultValues[__tvType];
	// viregs[VI_VSYNC] = 0x0000020D;
	viregs[VI_VSYNC] = vtotalDefaultValues[__tvType];
	// viregs[VI_HSYNC] = 0x00000C15;
	viregs[VI_HSYNC] = htotalDefaultValues[__tvType] | (0x15 << 16);
	// viregs[VI_SYNCLEAP] = 0x0C150C15;
	viregs[VI_SYNCLEAP] = hTotalLeap[__tvType];
	// viregs[VI_HORIZONTALVIDEO] = 0x006c02ec;
	viregs[VI_HORIZONTALVIDEO] = (hvideoStart[__tvType] << 16) | hvideoEnd[__tvType];
	// viregs[VI_VERTICALVIDEO] = 0x002501ff;
	viregs[VI_VERTICALVIDEO] = (vvideoStart[__tvType] << 16) | vvideoEnd[__tvType];
	// viregs[VI_VERTICALBURST] = 0x000E0204;
	viregs[VI_VERTICALBURST] = vburst[__tvType];
	// viregs[VI_XSCALE] = 0x00000200;
	// viregs[VI_YSCALE] = 0x00000400;
	viregs[VI_XSCALE] = ((1024*320) + 320) / 640;
	// viregs[VI_YSCALE] = ((1024*240) + 120) / 240;
	viregs[VI_YSCALE] = 1 << 10;

	// 1024 = 1.0000000000
	// viregs[VI_YSCALE] = 1024 * (240.0f / 288.0f);
	// viregs[VI_YSCALE] = 820;
	
	// Enable VI interrupt
	viregs[VI_INTLINE] = 240;
	EnableVideoInterrupts();
}

void WaitForVideoSync()
{
	// volatile uint32_t* viregs = (uint32_t*)VI_BASE;
	// while (viregs[VI_CURRENTLINE] < 0x200);
	// while (viregs[VI_CURRENTLINE] >= 0x200);

	while (!__viInterrupt);
	__viInterrupt = 0;
}

void HandleVideoInterrupt()
{
	__viInterruptCounter += 1;
	__viInterrupt = 1;
}

void ResetVideoCurrentLine()
{
	volatile uint32_t* viregs = (uint32_t*)VI_BASE;
	viregs[VI_CURRENTLINE] = viregs[VI_CURRENTLINE];
}
