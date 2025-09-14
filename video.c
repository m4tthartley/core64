//
//  Created by Matt Hartley on 08/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>

#include "registers.h"


uint32_t __viInterruptCounter = 0;

inline void SetVIRegister(uint32_t reg, uint32_t value)
{
	((uint32_t*)(VI_BASE))[reg] = value;
	// TODO: MEMORY BARRIER?
}

void EnableVideoInterrupts()
{
	uint32_t* interruptMask = ((uint32_t*)MI_BASE) + MI_INTERRUPT_MASK;
	*interruptMask = MI_INT_VI_SET;
}

void InitDefaultVI()
{
	// uint32_t* miregs = (uint32_t*)MI_BASE;
	volatile uint32_t* viregs = (uint32_t*)VI_BASE;
	
	uint32_t controlMode = VI_COLOR_MODE_16 | VI_AA_DISABLED;
	viregs[VI_CONTROL] = controlMode;
	viregs[VI_ORIGIN] = VI_FRAMEBUFFERBASE;
	viregs[VI_WIDTH] = 320*1;
	// viregs[VI_INTLINE] = 2;//0x200;
	viregs[VI_CURRENTLINE] = 0;
	viregs[VI_BURST] = 0x03E52239;
	viregs[VI_VSYNC] = 0x0000020D;
	viregs[VI_HSYNC] = 0x00000C15;
	viregs[VI_SYNCLEAP] = 0x0C150C15;
	viregs[VI_HORIZONTALVIDEO] = 0x006c02ec;
	viregs[VI_VERTICALVIDEO] = 0x002501ff;
	viregs[VI_VERTICALBURST] = 0x000E0204;
	viregs[VI_XSCALE] = 0x00000200;
	viregs[VI_YSCALE] = 0x00000400;
	
	// Enable VI interrupt
	viregs[VI_INTLINE] = 240;
	EnableVideoInterrupts();
}

void ResetVideoCurrentLine()
{
	volatile uint32_t* viregs = (uint32_t*)VI_BASE;
	viregs[VI_CURRENTLINE] = viregs[VI_CURRENTLINE];
}
