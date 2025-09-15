//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "n64/print.c"
#include "n64/util.c"
#include "n64/video.c"
#include "n64/interrupts.c"
#include "n64/draw.c"
#include "n64/core64.h"


#define DP_BASE 0xA4100000
#define DP_START ((volatile uint32_t*)(DP_BASE + 0x0))
#define DP_END ((volatile uint32_t*)(DP_BASE + 0x4))
#define DP_CURRENT ((volatile uint32_t*)(DP_BASE + 0x8))
#define DP_STATUS ((volatile uint32_t*)(DP_BASE + 0xC))

#define DP_STATUS_DMEN_DMA 0x001
#define DP_STATUS_FREEZE 0x002
#define DP_STATUS_FLUSH 0x004
#define DP_STATUS_GCLK_ALIVE 0x008
#define DP_STATUS_TMEM_BUSY 0x010
#define DP_STATUS_PIPE_BUSY 0x020
#define DP_STATUS_BUSY 0x040
#define DP_STATUS_BUFFER_READY 0x080
#define DP_STATUS_DMA_BUSY 0x100
#define DP_STATUS_END_VALID 0x200
#define DP_STATUS_START_VALID 0x400

#define DP_STATUS_WRITE_XBUS_DMEM_DMA_RESET 0x001
#define DP_STATUS_WRITE_XBUS_DMEM_DMA_SET 0x002
#define DP_STATUS_WRITE_FREEZE_RESET 0x004
#define DP_STATUS_WRITE_FREEZE_SET 0x008
#define DP_STATUS_WRITE_FLUSH_RESET 0x010
#define DP_STATUS_WRITE_FLUSH_SET 0x020
#define DP_STATUS_WRITE_TMEM_COUNTER_RESET 0x040
#define DP_STATUS_WRITE_PIPE_COUNTER_RESET 0x080
#define DP_STATUS_WRITE_CMD_COUNTER_RESET 0x100
#define DP_STATUS_WRITE_CLOCK_COUNTER_RESET 0x200

// uint64_t raw;
// struct {
// 	uint32_t w0;
// 	uint32_t w1;
// };
// struct {
// 	uint16_t s0;
// 	uint16_t s1;
// 	uint16_t s2;
// 	uint16_t s3;
// };
// struct {
// 	uint8_t b0;
// 	uint8_t b1;
// 	uint8_t b2;
// 	uint8_t b3;
// 	uint8_t b4;
// 	uint8_t b5;
// 	uint8_t b6;
// 	uint8_t b7;
// };
typedef struct {
	uint32_t word0;
	uint32_t word1;
} rdpcommand_t;

uint64_t __attribute__((aligned(8))) commandList[32];
// uint64_t* commandList = (uint64_t*)0xA0100000;

void DataCacheWritebackInvalidate(volatile void* addr, uint32_t size)
{
	int linesize = 16;
	void* line = (void*)((unsigned long)addr & ~(linesize-1));
	uint32_t len = size + (addr-line);
	for (int l=0; l<len; l+=linesize) {
		asm ("\tcache %0, (%1)\n" :: "i" (0x15), "r" (line + l));
	}
}

void MemoryBarrier()
{
	asm volatile ("" : : : "memory");
}

int main()
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	InitDefaultVI();

	for (int idx=0; idx<320*240; ++idx) {
		fb[idx] = (31<<1) | 1;
	}

	char str[64];
	sprint(str, 64, "dp current: %32x", *DP_CURRENT);
	DrawFontStringWithBG(N64Font, str, 10, 10);
	sprint(str, 64, "dp start: %32x", *DP_START);
	DrawFontStringWithBG(N64Font, str, 10, 20);
	sprint(str, 64, "dp status: %32x", *DP_STATUS);
	DrawFontStringWithBG(N64Font, str, 10, 30);
	sprint(str, 64, "dp frozen: %32x", *DP_STATUS & 2);
	DrawFontStringWithBG(N64Font, str, 10, 40);

	*DP_STATUS = DP_STATUS_WRITE_XBUS_DMEM_DMA_RESET | DP_STATUS_WRITE_FLUSH_RESET | DP_STATUS_WRITE_FREEZE_RESET;
	while ((*DP_STATUS & (DP_STATUS_START_VALID | DP_STATUS_END_VALID)));

	{
		int i = 0;
		rdpcommand_t cmd;
#if 0
		// Sync
		// commandList[i++] = 0xE700000000000000ULL;
// cmd.b0 = 0x29;
		cmd.raw = 0;
		// cmd.raw = (uint64_t)0x29 << 56;
		cmd.w0 = 0x29000000;
		cmd.w1 = 0;
		commandList[i++] = cmd.raw;

		// SetColorImage
		// commandList[i++] = 0xFF00000000000000ULL
		// 				| (2ULL << 51)
		// 				| ((320ULL-1) << 32)
		// 				| (VI_FRAMEBUFFERBASE & 0xFFFFFF);
		cmd.raw = 0;
		cmd.b0 = 0x3F;
		cmd.b1 = 2 << 3;
		cmd.s1 = 320-1;
		// cmd.s2 = 320-1;
		cmd.w1 = VI_FRAMEBUFFERBASE & 0xFFFFFF;
		commandList[i++] = cmd.raw;
		// {
		// 	// Word 0: opcode + format/size/width
		// 	uint32_t w0 = (0x3F << 24)         // opcode
		// 				| (0    << 21)         // format = RGBA
		// 				| (2    << 19)         // size = 16bpp
		// 				| ((320-1) & 0x3FF);   // width-1

		// 	// Word 1: framebuffer base (physical address)
		// 	uint32_t w1 = (VI_FRAMEBUFFERBASE & 0x00FFFFFF);

		// 	// Store as two 32-bit words in the DL
		// 	commandList[i++] = ((uint64_t)w0 << 32) | w1;
		// }

		// SetFillColor
		// commandList[i++] = 0xF700000000000000 | 0x001F001FULL;
		cmd.raw = 0;
		cmd.b0 = 0x37;
		cmd.s2 = (31 << 11) | 1;
		cmd.s3 = (31 << 11) | 1;
		commandList[i++] = cmd.raw;

		// FillRect
		// commandList[i++] = 0xF600000000000000ULL
		// 				| (200ULL << 44)
		// 				| (150ULL << 32)
		// 				| (100ULL << 12)
		// 				| (50ULL << 0);
		uint64_t x0 = 100;
		uint64_t x1 = 200;
		uint64_t y0 = 50;
		uint64_t y1 = 150;
		cmd.raw = 0;
		cmd.b0 = 0x36;
		cmd.raw |= x1 << 44;
		cmd.raw |= y1 << 32;
		cmd.raw |= x0 << 12;
		cmd.raw |= y0;
		commandList[i++] = cmd.raw;

		// Sync
		// commandList[i++] = 0xE700000000000000ULL;
		// End
		// commandList[i++] = 0xDF00000000000000ULL;
		// commandList[i++] = 0;
		cmd.raw = 0;
		cmd.b0 = 0x29;
		commandList[i++] = cmd.raw;
#endif

		// Pipe Sync
		cmd.word0 = 0x28000000;
		cmd.word1 = 0;
		commandList[i++] = *(uint64_t*)&cmd;

		// Set Other Modes
		cmd.word0 = (0x2F<<24) | (0x3<<20);
		cmd.word1 = 0;//0x00200000;
		commandList[i++] = *(uint64_t*)&cmd;

		// Set Combine Mode
		cmd.word0 = (0x3C<<24) | (0xFF<<16);
		cmd.word1 = 0x00000000;
		commandList[i++] = *(uint64_t*)&cmd;

		// Set Primitive Color
		cmd.word0 = (0x3A<<24);
		cmd.word1 = 0xFF0000FF;
		commandList[i++] = *(uint64_t*)&cmd;

		// SetColorImage
		cmd.word0 = (0x3F<<24) | (0<<21) | (2<<19) | (320-1);
		cmd.word1 = VI_FRAMEBUFFERBASE & 0xFFFFFF;
		commandList[i++] = *(uint64_t*)&cmd;

		// Set Scissor
		uint32_t sx0 = 0<<2;
		uint32_t sy0 = 0<<2;
		uint32_t sx1 = 320<<2;
		uint32_t sy1 = 240<<2;
		cmd.word0 = (0x2D<<24) | (sx0<<12) | (sy0);
		cmd.word1 = (sx1<<12) | (sy1);
		commandList[i++] = *(uint64_t*)&cmd;

		// SetFillColor
		cmd.word0 = 0x37 << 24;
		cmd.word1 = (((31<<6)|1) << 16) | ((31<<11)|1);
		commandList[i++] = *(uint64_t*)&cmd;

		// FillRect
		// uint32_t x0 = 100<<2;
		// uint32_t y0 = 100<<2;
		// uint32_t x1 = 210<<2;
		// uint32_t y1 = 130<<2;
		// cmd.word0 = (0x36<<24) | (x1<<12) | (y1);
		// cmd.word1 = (x0<<12) | (y0);
		// commandList[i++] = *(uint64_t*)&cmd;

		// Fill Triangle
		uint32_t v0x = 100, v0y = 100;
		uint32_t v1x = 200, v1y = 150;
		uint32_t v2x = 150, v2y = 200;
		int32_t dxldy = ((v0x-v2x)<<16) / ((v0y-v2y));
		int32_t dxhdy = ((v1x-v2x)<<16) / ((v1y-v2y));
		int32_t dxmdy = ((v2x-v1x)<<16) / ((v2y-v1y));
		cmd.word0 = (0b001<<27) | (1<<23/*lmajor*/) | (v2y<<2);
		cmd.word1 = (v1y<<2<<16) | (v0y<<2);
		commandList[i++] = *(uint64_t*)&cmd;
		cmd.word0 = (v2x<<16);
		cmd.word1 = dxldy;
		commandList[i++] = *(uint64_t*)&cmd;
		cmd.word0 = (v2x<<16);
		cmd.word1 = dxhdy;
		commandList[i++] = *(uint64_t*)&cmd;
		cmd.word0 = (v1x<<16);
		cmd.word1 = dxmdy;
		commandList[i++] = *(uint64_t*)&cmd;

		// Sync
		cmd.word0 = 0x29000000;
		cmd.word1 = 0;
		commandList[i++] = *(uint64_t*)&cmd;

		DataCacheWritebackInvalidate(commandList, i*8);
		uint32_t start = (unsigned long)commandList & 0x1FFFFFFF;
		uint32_t end = start + (i*8);
		MemoryBarrier();
		*DP_START = start;
		MemoryBarrier();
		*DP_END = end;
		MemoryBarrier();

		// while (*DP_CURRENT == *DP_START);

		// sprint(str, 64, "dp current: %32x", *DP_CURRENT);
		// DrawFontStringWithBG(N64Font, str, 10, 50);
	}

	int pixel = 0;
	for (;;) {
		// for (int idx=0; idx<320*240; ++idx) {
		// 	fb[idx] = 0x0;
		// }
		fb[pixel] = 0xFFFF;
		++pixel;
		pixel %= (320*240);

		char str[64];
		sprint(str, 64, "dp current: %32x", *DP_CURRENT);
		DrawFontStringWithBG(N64Font, str, 10, 60);
		sprint(str, 64, "dp start: %32x", *DP_START);
		DrawFontStringWithBG(N64Font, str, 10, 70);
		sprint(str, 64, "dp status: %32x", *DP_STATUS);
		DrawFontStringWithBG(N64Font, str, 10, 80);
		sprint(str, 64, "dp frozen: %32x", *DP_STATUS & 2);
		DrawFontStringWithBG(N64Font, str, 10, 90);

		

		WaitForVideoSync();
	}
}
