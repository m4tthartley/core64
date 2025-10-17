//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>

#include "core64.h"
#include "draw.h"
#include "registers.h"
#include "util.h"

#include "interrupts.h"

// extern volatile uint32_t __viInterruptCounter;
// extern volatile uint32_t __viInterrupt;




uint32_t __INTERRUPT_FRAME_SIZE = (sizeof(interruptframe_t) & ~7) + 8;
// uint32_t __EF_AT = offsetof(exceptionframe_t, at);
// uint32_t __EF_V0 = offsetof(exceptionframe_t, v0);
// uint32_t __EF_V1 = offsetof(exceptionframe_t, v1);
// uint32_t __EF_A0 = offsetof(exceptionframe_t, a0);
// uint32_t __EF_A1 = offsetof(exceptionframe_t, a1);
// uint32_t __EF_A2 = offsetof(exceptionframe_t, a2);
// uint32_t __EF_A3 = offsetof(exceptionframe_t, a3);
// uint32_t __EF_T0 = offsetof(exceptionframe_t, t0);
// uint32_t __EF_T1 = offsetof(exceptionframe_t, t1);
// uint32_t __EF_T2 = offsetof(exceptionframe_t, t2);
// uint32_t __EF_T3 = offsetof(exceptionframe_t, t3);
// uint32_t __EF_T4 = offsetof(exceptionframe_t, t4);
// uint32_t __EF_T5 = offsetof(exceptionframe_t, t5);
// uint32_t __EF_T6 = offsetof(exceptionframe_t, t6);
// uint32_t __EF_T7 = offsetof(exceptionframe_t, t7);
// uint32_t __EF_T8 = offsetof(exceptionframe_t, t8);
// uint32_t __EF_T9 = offsetof(exceptionframe_t, t9);
// uint32_t __EF_SP = offsetof(exceptionframe_t, sp);
// uint32_t __EF_RA = offsetof(exceptionframe_t, ra);

// uint32_t __EF_STATUS = offsetof(exceptionframe_t, status);
// uint32_t __EF_CAUSE = offsetof(exceptionframe_t, cause);
// uint32_t __EF_EPC = offsetof(exceptionframe_t, epc);
// uint32_t __EF_BADADDR = offsetof(exceptionframe_t, badaddr);

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
		case MI_INTERRUPT_SP: return "Mi_Interrupt_Sp";
		case MI_INTERRUPT_SI: return "Mi_Interrupt_Si";
		case MI_INTERRUPT_AI: return "Mi_Interrupt_Ai";
		case MI_INTERRUPT_VI: return "Mi_Interrupt_Vi";
		case MI_INTERRUPT_PI: return "Mi_Interrupt_Pi";
		case MI_INTERRUPT_DP: return "Mi_Interrupt_Dp";

		default: return "Unknown Interrupt";
	}
}

void ExceptionHandler(interruptframe_t* frame)
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

	// if (cause == 15) {
	// 	// ignore floating point error
	// 	return;
	// }

	
	char str[256];
	sprint(str, 256, "ExcCode(%u)  %s", cause, GetExcCodeName(cause));
	DrawFontStringWithBG(N64Font, str, 10, 25);
	
	switch (cause) {
		case 15: {
			uint32_t fcsr = _GetFCSR();
			switch ((fcsr>>12) & 0x3F) {
				case (1<<1):
					DrawFontStringWithBG(N64Font, "Inexact", 10, 45);
					break;
				case (1<<2):
					DrawFontStringWithBG(N64Font, "Underflow", 10, 45);
					break;
				case (1<<3):
					DrawFontStringWithBG(N64Font, "Overflow", 10, 45);
					break;
				case (1<<4):
					DrawFontStringWithBG(N64Font, "Divide by Zero", 10, 45);
					break;
				case (1<<5):
					DrawFontStringWithBG(N64Font, "Invalid Operation", 10, 45);
					break;
			}

			DrawStrBG(10, 55, "EPC: %32x", frame->epc);
		} break;

		default:
			sprint(str, 256, "Stack pointer: %32x", frame->sp);
			DrawFontStringWithBG(N64Font, str, 10, 45);
			sprint(str, 256, "Status: %32x", frame->status);
			DrawFontStringWithBG(N64Font, str, 10, 55);
			sprint(str, 256, "Cause: %8x, %u", cause, cause);
			DrawFontStringWithBG(N64Font, str, 10, 65);
			sprint(str, 256, "Bad Address: %32x", /**(uint32_t*)0xA0001000*/ frame->badaddr);
			DrawFontStringWithBG(N64Font, str, 10, 75);
			sprint(str, 256, "EPC: %32x", frame->epc);
			DrawFontStringWithBG(N64Font, str, 10, 85);
			break;
	}

	uint16_t color = 0;
	// uint32_t idx = 0;
	// for (;;) {
	// 	uint32_t y = idx >> 3;
	// 	uint32_t x = idx & 7;
	// 	fb[(240-8+y)*320 + (x)] = (color) ;
	// 	// ++fbIndex;
	// 	// fbIndex %= (320*240);
	// 	color += 32;

	// 	++idx;
	// 	idx &= 63;
	// }

	// int dirs[][2] = {
	// 	{0, -1},
	// 	{1, 0},
	// 	{0, 1},
	// 	{-1, 0},
	// };
	// // int dx = 0;
	// // int dy = 1;
	// int dir = 0;
	// int x = 160;
	// int y = 120;
	// for (;;) {
	// 	if (x < 150 || x > 170 || y < 110 || y > 130) {
	// 		++dir;
	// 		dir &= 3;
	// 	}

	// 	x += dirs[dir][0];
	// 	y += dirs[dir][1];

	// 	fb[(240-8+y)*320 + (x)] = (color);
	// 	color += 32;
	// }

	for (;;);
}

void UnhandledInterrupt(uint32_t type)
{
	char str[256];
	sprint(str, 256, "Unhandled Interrupt");
	int x = 320/2 - (strsize(str)*6 / 2);
	int y = 240/2;
	DrawFontStringWithBG(N64Font, str, x, y);

	sprint(str, 256, "%s  0x%32x", GetInterruptName(type & 0xFF), type);
	x = 320/2 - (strsize(str)*6 / 2);
	DrawFontStringWithBG(N64Font, str, x, y + 10);

	sprint(str, 256, "MI_INTERRUPT  0x%32x", ((uint32_t*)MI_BASE)[MI_INTERRUPT]);
	x = 320/2 - (strsize(str)*6 / 2);
	DrawFontStringWithBG(N64Font, str, x, y + 20);

	sprint(str, 256, "MI_INTERRUPT_MASK  0x%32x", ((uint32_t*)MI_BASE)[MI_INTERRUPT_MASK]);
	x = 320/2 - (strsize(str)*6 / 2);
	DrawFontStringWithBG(N64Font, str, x, y + 30);

	for (;;);
}

void HandleInterrupt_Reset()
{
	char str[256];
	sprint(str, 256, "Reset Interrupt");
	int x = 320/2 - (strsize(str)*6 / 2);
	int y = 240/2;
	DrawFontStringWithBG(N64Font, str, x, y);

	for (;;);
}

void HandleInterrupt_Timer()
{
	char str[256];
	sprint(str, 256, "Timing Interrupt");
	int x = 320/2 - (strsize(str)*6 / 2);
	int y = 240/2;
	DrawFontStringWithBG(N64Font, str, x, y);

	// for (;;);

	// Log("Timing Interrupt");
	return;
}

void HandleInterrupt_Cart()
{
	char str[256];
	sprint(str, 256, "Cart Interrupt");
	int x = 320/2 - (strsize(str)*6 / 2);
	int y = 240/2;
	DrawFontStringWithBG(N64Font, str, x, y);

	for (;;);
}

void HandleInterrupt_MI()
{
	uint32_t type = ((uint32_t*)MI_BASE)[MI_INTERRUPT] & ((uint32_t*)MI_BASE)[MI_INTERRUPT_MASK];

	if (type & MI_INTERRUPT_VI) {
		HandleVideoInterrupt();
		ResetVideoCurrentLine();

		return;
	}

	if (type & MI_INTERRUPT_PI) {
		// TODO: Call a user defined callback here
		// Log("MI_INTERRUPT_PI");
		return;
	}

	UnhandledInterrupt(type);
}

void AssertionScreen(const char* function, const char* filename, int line, const char* expr)
{
	char str[256];
	sprint(str, 256, "Assertion Fired");
	DrawFontStringWithBG(N64Font, str, 320/2 - (strlen(str)*6/2), 10);

	sprint(str, 256, "in %s(), at %s:%i", function, filename, line);
	DrawFontStringWithBG(N64Font, str, 320/2 - (strlen(str)*6/2), 20);

	DrawFontStringWithBG(N64Font, (char*)expr, 320/2 - (strlen(expr)*6/2), 30);

	for (;;);
}
