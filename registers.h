//
//  Created by Matt Hartley on 12/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#define __EF_AT 8
#define __EF_V0 16
#define __EF_V1 24
#define __EF_A0 32
#define __EF_A1 40
#define __EF_A2 48
#define __EF_A3 56
#define __EF_T0 64
#define __EF_T1 72
#define __EF_T2 80
#define __EF_T3 88
#define __EF_T4 96
#define __EF_T5 104
#define __EF_T6 112
#define __EF_T7 120
#define __EF_T8 128
#define __EF_T9 136
#define __EF_SP 240
#define __EF_RA 248

#define __EF_STATUS 256
#define __EF_CAUSE 260
#define __EF_EPC 264
#define __EF_BADADDR 268


// COP0 registers
#define C0_RANDOM $1
#define C0_BADVADDR $8
#define C0_COUNT $9
#define C0_COMPARE $11
#define C0_SR $12
#define C0_CAUSE $13
#define C0_EPC $14
#define C0_PROID $15
#define C0_CONFIG $16
#define C0_WATCHPOINT $18

// Status register bitmasks
#define StatusRegister_Cop1Usable			0x20000000 // Floating point coprocessor usable
#define StatusRegister_FloatRegisters		0x04000000
#define StatusRegister_ExceptionVectors		0x00400000 // BEV
#define StatusRegister_SoftReset			0x00100000

#define StatusRegister_KernelExtAddressing	0x00000080
#define StatusRegister_SupervisorAddressing	0x00000040
#define StatusRegister_UserAddressing		0x00000020

#define StatusRegister_CPUExecutionMode		0x00000018
#define StatusRegister_ErrorLevel			0x00000004
#define StatusRegister_ExceptionLevel		0x00000002
#define StatusRegister_InterruptsEnabled	0x00000001

#define SR_CU1	0x20000000 // Floating point coprocessor usable
#define SR_FR	0x04000000
#define SR_BEV	0x00400000 // BEV
#define SR_PE	0x00100000 // Soft Reset

#define SR_KX	0x00000080
#define SR_SX	0x00000040
#define SR_UX	0x00000020

#define SR_KSU	0x00000018
#define SR_ERL	0x00000004
#define SR_EXL	0x00000002
#define SR_IE	0x00000001

#define Cop0_DefaultStatusRegister StatusRegister_Cop1Usable|\
									StatusRegister_SoftReset|\
									StatusRegister_FloatRegisters|\
									StatusRegister_KernelExtAddressing|\
									StatusRegister_SupervisorAddressing|\
									StatusRegister_UserAddressing|\
									StatusRegister_InterruptsEnabled

// Cop0 interrupts
#define C0_INTERRUPT_0 0x0100
#define C0_INTERRUPT_1 0x0200
#define C0_INTERRUPT_2 0x0400
#define C0_INTERRUPT_3 0x0800
#define C0_INTERRUPT_4 0x1000
#define C0_INTERRUPT_5 0x2000
#define C0_INTERRUPT_6 0x4000
#define C0_INTERRUPT_7 0x8000
#define C0_INTERRUPT_RCP C0_INTERRUPT_2
#define C0_INTERRUPT_CART C0_INTERRUPT_3
#define C0_INTERRUPT_PRENMI C0_INTERRUPT_4 // Pre Non-Maskable Interrupt
#define C0_INTERRUPT_TIMER C0_INTERRUPT_7


// Mips Interface
#define MI_BASE 0xA4300000
#define MI_MODE 0
#define MI_VERSION 1
#define MI_INTERRUPT 2
#define MI_INTERRUPT_MASK 3
// #define MI_INTR_MASK ((volatile uint32_t*)(MI_BASE + 0x0C))

#define MI_INT_VI_SET 0x80
#define MI_INT_VI_CLEAR 0x40

#define MI_INTERRUPT_SP 0x01
#define MI_INTERRUPT_SI 0x02
#define MI_INTERRUPT_AI 0x04
#define MI_INTERRUPT_VI 0x08
#define MI_INTERRUPT_PI 0x10
#define MI_INTERRUPT_DP 0x20


// Video Interface
#define VI_BASE 0xA4400000
// #define VI_CURSOR (VI_BASE + 0x10)

// #define CONTROL (void*)(VI_BASE+0x00)
// #define ORIGIN (void*)(VI_BASE+0x04)
// #define WIDTH (void*)(VI_BASE+0x08)
// #define INTR (void*)(VI_BASE+0x0C)
// #define CURSOR (void*)(VI_BASE+0x10)
// #define BURST (void*)(VI_BASE+0x14)
// #define VSYNC (void*)(VI_BASE+0x18)
// #define HSYNC (void*)(VI_BASE+0x1C)
// #define LEAP (void*)(VI_BASE+0x20)
// #define HSTART (void*)(VI_BASE+0x24)
// #define VSTART (void*)(VI_BASE+0x28)
// #define XSCALE (void*)(VI_BASE+0x2C)
// #define YSCALE (void*)(VI_BASE+0x30)

#define VI_CONTROL			0 /* 0000 */
#define VI_ORIGIN			1 /* 0004 */
#define VI_WIDTH			2 /* 0008 */
#define VI_INTLINE			3 /* 000C */
#define VI_CURRENTLINE		4 /* 0010 */
#define VI_BURST			5 /* 0014 */
#define VI_VSYNC			6 /* 0018 */
#define VI_HSYNC			7 /* 001C */
#define VI_SYNCLEAP			8 /* 0020 */
#define VI_HORIZONTALVIDEO	9 /* 0024 */
#define VI_VERTICALVIDEO	10 /* 0028 */
#define VI_VERTICALBURST	11 /* 002C */
#define VI_XSCALE			12 /* 0030 */
#define VI_YSCALE			13 /* 0034 */
#define VI_REGISTERCOUNT	14

#define VI_COLOR_MODE_16 0b10
#define VI_COLOR_MODE_32 0b11

#define VI_AA_ENABLED (0b00<<8)
#define VI_AA_JUST_RESAMPLING (0b10<<8)
#define VI_AA_DISABLED (0b11<<8)

#define VI_FRAMEBUFFERBASE 0xA0200000

