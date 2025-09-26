//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "n64/core64.h"

#include "n64/n64def.h"
#include "n64/system.c"
#include "n64/print.c"
#include "n64/util.c"
#include "n64/video.c"
#include "n64/interrupts.c"
#include "n64/draw.c"


volatile uint32_t __memorySize;
volatile uint32_t __tvType;
volatile uint32_t __resetType;
volatile uint32_t __consoleType;

uint32_t lastFrameTime;
uint32_t __deltaTime;


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

uint64_t __attribute__((aligned(8))) commandList[64];
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

typedef struct {
	uint32_t x;
	uint32_t y;
} vecscreen_t; // Currently just 32bit uint

typedef struct {
	void* buffer;
	uint32_t bufferSize; // in bytes
	uint32_t cursor; // in bytes
} rdpcmdlist_t;

rdpcmdlist_t RDP_CmdList(void* buffer, uint32_t size)
{
	rdpcmdlist_t result = {0};
	result.buffer = buffer;
	result.bufferSize = size;
	return result;
}

void RDP_Write(rdpcmdlist_t* cmdlist, uint32_t word)
{

	assert(cmdlist->cursor + 4 < cmdlist->bufferSize);
	volatile uint32_t* buffer = (uint32_t*)((uint8_t*)cmdlist->buffer + cmdlist->cursor);
	*buffer = word;
	cmdlist->cursor += 4;
}

void RDP_FillTriangle(rdpcmdlist_t* cmdlist, vecscreen_t* verts)
{
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);
	if (verts[1].y > verts[2].y) swap(verts[1], verts[2]);
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);

	rdpcommand_t cmd;
	int32_t v0x = verts[0].x, v0y = verts[0].y;
	int32_t v1x = verts[1].x, v1y = verts[1].y;
	int32_t v2x = verts[2].x, v2y = verts[2].y;
	int32_t lydiff = v2y-v1y;
	int32_t hydiff = v2y-v0y;
	int32_t mydiff = v1y-v0y;
	int32_t lxdiff = v2x-v1x;
	int32_t hxdiff = v2x-v0x;
	int32_t mxdiff = v1x-v0x;
	int32_t dxldy = lydiff ? (lxdiff*0x10000) / lydiff : 0;
	int32_t dxhdy = hydiff ? (hxdiff*0x10000) / hydiff : 0;
	int32_t dxmdy = mydiff ? (mxdiff*0x10000) / mydiff : 0;

	bool leftMajor = (hxdiff * mydiff - mxdiff * hydiff) <= 0;

	RDP_Write(cmdlist, (0b001<<27) | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_Write(cmdlist, (((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_Write(cmdlist, (v1x<<16));
	RDP_Write(cmdlist, dxldy);

	// line from lowest y to highest y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxhdy);

	// line from lowest y to middle y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxmdy);
}

void RDP_FillTriangleWithShade(rdpcmdlist_t* cmdlist, vecscreen_t* verts)
{
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);
	if (verts[1].y > verts[2].y) swap(verts[1], verts[2]);
	if (verts[0].y > verts[1].y) swap(verts[0], verts[1]);

	rdpcommand_t cmd;
	int32_t v0x = verts[0].x, v0y = verts[0].y;
	int32_t v1x = verts[1].x, v1y = verts[1].y;
	int32_t v2x = verts[2].x, v2y = verts[2].y;
	int32_t lydiff = v2y-v1y;
	int32_t hydiff = v2y-v0y;
	int32_t mydiff = v1y-v0y;
	int32_t lxdiff = v2x-v1x;
	int32_t hxdiff = v2x-v0x;
	int32_t mxdiff = v1x-v0x;
	int32_t dxldy = lydiff ? (lxdiff*0x10000) / lydiff : 0;
	int32_t dxhdy = hydiff ? (hxdiff*0x10000) / hydiff : 0;
	int32_t dxmdy = mydiff ? (mxdiff*0x10000) / mydiff : 0;

	bool leftMajor = (hxdiff * mydiff - mxdiff * hydiff) <= 0;

	RDP_Write(cmdlist, (0b001<<27) | (1<<26/*shade*/) | (leftMajor<<23/*lmajor*/) | ((v2y<<2)&0x3FFF));
	RDP_Write(cmdlist, (((v1y<<2)&0x3FFF)<<16) | ((v0y<<2)&0x3FFF));

	// low=high on the screen, high=low on the screen
	// line from middle y to highest y
	RDP_Write(cmdlist, (v1x<<16));
	RDP_Write(cmdlist, dxldy);

	// line from lowest y to highest y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxhdy);

	// line from lowest y to middle y
	RDP_Write(cmdlist, (v0x<<16));
	RDP_Write(cmdlist, dxmdy);

	// SHADE
	int r = 128;
	int g = 128;
	int b = 128;
	int a = 255;

	// int part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, (r<<16) | g);
	RDP_Write(cmdlist, (b<<16) | a);

	// int part change in shade along scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// fractional part of shade color at xh, floor(yh)
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// fractional part change in shade along scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// int part change along major edge
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// int part change each scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// frac part change along major edge
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);

	// frac part change each scanline
	RDP_Write(cmdlist, 0);
	RDP_Write(cmdlist, 0);
}

void RDP_FillRect(rdpcmdlist_t* cmdlist, int32_t x, int32_t y, int32_t width, int32_t height)
{
	uint32_t x0 = x<<2;
	uint32_t y0 = y<<2;
	uint32_t x1 = (x+width)<<2;
	uint32_t y1 = (y+height)<<2;

	RDP_Write(cmdlist, (0x36<<24) | (x1<<12) | (y1));
	RDP_Write(cmdlist, (x0<<12) | (y0));
}

int main()
{
	lastFrameTime = _GetClock();

	Log("System Started");

	// BOOT TESTING
#if 0
	volatile bootinfo_t* bootInfo = (volatile bootinfo_t*)0x80000300;
	
	#define RDRAM_BASE 0x80000000
	#define RDRAM_SIZE_4MB 0x00400000
	#define RDRAM_SIZE_8MB 0x00800000
	
	__memorySize = RDRAM_SIZE_4MB;
	// #define RDRAM_BASE 0x80000000

	volatile uint32_t* probeAddr = (volatile uint32_t*)(RDRAM_BASE + RDRAM_SIZE_4MB);
	uint32_t tmp = *probeAddr;
	*probeAddr = 0xCAFEBABE;
	__asm__ volatile("sync");
	if (*probeAddr == 0xCAFEBABE) {
		__memorySize = RDRAM_SIZE_8MB;
	}
	*probeAddr = tmp;

	#define NTSC 0
	#define PAL 1
	#define MPAL 2

	// tv type
	#define PIF_RAM ((volatile uint8_t*)0xBFC00700);
	volatile uint8_t* pif = PIF_RAM;
	__tvType = pif[0x24];
#endif
	////////

	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	InitDefaultVI();

	// for (int idx=0; idx<320*240; ++idx) {
	// 	fb[idx] = (31<<1) | 1;
	// }

	// char str[64];
	// sprint(str, 64, "cmdlist buffer: %32x", cmdlist.buffer);
	// DrawFontStringWithBG(N64Font, str, 10, 10);
	// sprint(str, 64, "cmdlist size: %32x", cmdlist.bufferSize);
	// DrawFontStringWithBG(N64Font, str, 10, 20);
	// sprint(str, 64, "dp status: %32x", *DP_STATUS);
	// DrawFontStringWithBG(N64Font, str, 10, 30);
	// sprint(str, 64, "dp frozen: %32x", *DP_STATUS & 2);
	// DrawFontStringWithBG(N64Font, str, 10, 40);

	*DP_STATUS = DP_STATUS_WRITE_XBUS_DMEM_DMA_RESET | DP_STATUS_WRITE_FLUSH_RESET | DP_STATUS_WRITE_FREEZE_RESET;
	while ((*DP_STATUS & (DP_STATUS_START_VALID | DP_STATUS_END_VALID))){};

	
	int pixel = 0;
	for (;;) {
		// clear framebuffer
		// memset((void*)VI_FRAMEBUFFERBASE, 0, 320*288*2);

#if 1
		{
			int i = 0;
			rdpcommand_t cmd = {0};
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
	
			// Set Combine Mode
			// cmd.word0 = (0x3C<<24) | (0xFF<<16);
			// cmd.word1 = 0x00000000;
			// commandList[i++] = *(uint64_t*)&cmd;
	
			// // Set Primitive Color
			// cmd.word0 = (0x3A<<24);
			// cmd.word1 = 0xFF0000FF;
			// commandList[i++] = *(uint64_t*)&cmd;

			// int32_t v0x = 50, v0y = 50;
			// int32_t v1x = 200, v1y = 150;
			// int32_t v2x = 150, v2y = 200;
			// int32_t dxldy = ((v2x-v1x)*0xFFFF/*<<16*/) / ((v2y-v1y));
			// int32_t dxhdy = ((v2x-v0x)*0xFFFF/*<<16*/) / ((v2y-v0y));
			// int32_t dxmdy = ((v1x-v0x)*0xFFFF/*<<16*/) / ((v1y-v0y));
			// cmd.word0 = (0b001<<27) | (1<<23/*lmajor*/) | (v2y<<2);
			// cmd.word1 = (v1y<<2<<16) | (v0y<<2);
			// // commandList[i++] = *(uint64_t*)&cmd;
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);
	
			// // line from middle y to highest y
			// cmd.word0 = (v1x<<16);
			// cmd.word1 = dxldy;
			// // commandList[i++] = *(uint64_t*)&cmd;
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);
	
			// // line from lowest y to highest y
			// cmd.word0 = (v0x<<16);
			// cmd.word1 = dxhdy;
			// // commandList[i++] = *(uint64_t*)&cmd;
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);
	
			// // line from lowest y to middle y
			// cmd.word0 = (v0x<<16);
			// cmd.word1 = dxmdy;
			// // commandList[i++] = *(uint64_t*)&cmd;
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);
	

			rdpcmdlist_t cmdlist = RDP_CmdList(commandList, sizeof(commandList));
	
			// Pipe Sync
			cmd.word0 = 0x27000000;
			cmd.word1 = 0;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// Set Other Modes
			cmd.word0 = (0x2F<<24) | (0x3<<20);
			cmd.word1 = 0;//0x00200000;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);

			// SetColorImage
			cmd.word0 = (0x3F<<24) | (0<<21) | (2<<19) | (320-1);
			cmd.word1 = VI_FRAMEBUFFERBASE & 0xFFFFFF;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// Set Scissor
			uint32_t sx0 = 1<<2;
			uint32_t sy0 = 1<<2;
			uint32_t sx1 = (320-2)<<2;
			uint32_t sy1 = (288-2)<<2;
			cmd.word0 = (0x2D<<24) | (sx0<<12) | (sy0);
			cmd.word1 = (sx1<<12) | (sy1);
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// SetFillColor
			cmd.word0 = 0x37 << 24;
			uint16_t color = (8<<1) | 1;
			cmd.word1 = (color << 16) | color;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);

			// // Pipe Sync
			cmd.word0 = 0x27000000;
			cmd.word1 = 0;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// // FillRect
			// uint32_t x0 = 100<<2;
			// uint32_t y0 = 80<<2;
			// uint32_t x1 = (320-1-100)<<2;
			// uint32_t y1 = (288-1-100)<<2;
			// cmd.word0 = (0x36<<24) | (x1<<12) | (y1);
			// cmd.word1 = (x0<<12) | (y0);
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);

			int ox = 1;
			int oy = 1;
			int w = 320-2;
			int h = 288-2;

			RDP_FillRect(&cmdlist, ox, oy, w/2, h/2);
			RDP_FillRect(&cmdlist, ox + w/2, oy, w/2, h/2);
			RDP_FillRect(&cmdlist, ox, oy + h/2, w/2, h/2);
			RDP_FillRect(&cmdlist, ox + w/2, oy + h/2, w/2, h/2);

			// Sync between different draw operations
			// Pipe Sync
			cmd.word0 = 0x27000000;
			cmd.word1 = 0;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// // SetFillColor
			cmd.word0 = 0x37 << 24;
			uint16_t triColor = (31<<6) | (12<<1) | 1;
			cmd.word1 = (triColor << 16) | triColor;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			// // Fill Triangle
			vecscreen_t vertsa[] = {
				{50, 50},
				{200, 150},
				{150, 200},
			};
			RDP_FillTriangle(&cmdlist, vertsa);
	
			// more triangles
			RDP_FillTriangle(&cmdlist, (vecscreen_t[]){
				{140, 40},
				{100, 60},
				{120, 30},
			});

			{
				// Shaded triangle
				vecscreen_t verts[] = {
					{200, 50},
					{260, 100},
					{230, 150},
				};
				RDP_FillTriangleWithShade(&cmdlist, verts);
			}

			// Pipe Sync
			cmd.word0 = 0x27000000;
			cmd.word1 = 0;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
			
			// // SetFillColor
			cmd.word0 = 0x37 << 24;
			triColor = (31<<11) | (12<<1) | 1;
			cmd.word1 = (triColor << 16) | triColor;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);

			static int x = 100;
			static int y = 100;
			static int speedx = 2;
			static int speedy = 2;
			vecscreen_t __attribute__((aligned(8))) verts2[] = {
				{x, y},
				{x+50, y+50},
				{x+45, y+5},
			};

			// // RDP_FillTriangle(&cmdlist, verts2);
			// // Test rect
			RDP_FillRect(&cmdlist, x, y, 50, 50);
			// x0 = x<<2;
			// y0 = y<<2;
			// x1 = (x+50)<<2;
			// y1 = (y+50)<<2;
			// cmd.word0 = (0x36<<24) | (x1<<12) | (y1);
			// cmd.word1 = (x0<<12) | (y0);
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);

			x += speedx;
			y += speedy;
			if (x+50 > 320) speedx = -2;
			if (x < 0) speedx = 2;
			if (y+50 > 288) speedy = -2;
			if (y < 0) speedy = 2;

			// Full Sync
			cmd.word0 = 0x29000000;
			cmd.word1 = 0;
			RDP_Write(&cmdlist, cmd.word0);
			RDP_Write(&cmdlist, cmd.word1);
	
			DataCacheWritebackInvalidate(commandList, cmdlist.cursor);
			uint32_t start = (unsigned long)commandList & 0x1FFFFFFF;
			uint32_t end = start + cmdlist.cursor;
			MemoryBarrier();
			*DP_START = start;
			MemoryBarrier();
			*DP_END = end;
			MemoryBarrier();
	
			while (*DP_CURRENT != *DP_END);
	
			// TODO: Use interrupt
			while (*DP_STATUS & (DP_STATUS_TMEM_BUSY | DP_STATUS_PIPE_BUSY));
		}
#endif

		uint32_t clockHz = 46875000;
		uint32_t clocksPerMs = clockHz / 1000;
		uint32_t time = _GetClock();
		uint32_t deltaClocks = time - lastFrameTime;
		lastFrameTime = time;

		__deltaTime = deltaClocks / clocksPerMs;

		// for (int idx=0; idx<320*240; ++idx) {
		// 	fb[idx] = 0x0;
		// }
		for (int x=0; x<16; ++x) {
			fb[pixel] = 0xFFFF;
			++pixel;
			pixel %= (320*240);
		}

		if (pixel == 4096) {
			Log("4096");
		}

		// char str[64];
		// sprint(str, 64, "dp current: %32x", *DP_CURRENT);
		// DrawFontString(N64Font, str, 10, 10);
		// sprint(str, 64, "dp start: %32x", *DP_START);
		// DrawFontStringWithBG(N64Font, str, 10, 70);
		// sprint(str, 64, "dp status: %32x", *DP_STATUS);
		// DrawFontStringWithBG(N64Font, str, 10, 80);
		// sprint(str, 64, "dp frozen: %32x", *DP_STATUS & 2);
		// DrawFontStringWithBG(N64Font, str, 10, 90);


		char str[64];
		sprint(str, 64, "%32x", __tvType);
		DrawFontString(N64Font, str, 10, 10);
		sprint(str, 64, "reset type: %32x", __resetType);
		DrawFontString(N64Font, str, 10, 20);
		sprint(str, 64, "memory size: %32x", __memorySize);
		DrawFontString(N64Font, str, 10, 30);
		sprint(str, 64, "console type: %32x", __consoleType);
		DrawFontString(N64Font, str, 10, 40);

		sprint(str, 64, "clock: %u", deltaClocks / clocksPerMs);
		DrawFontStringWithBG(N64Font, str, 10, 60);

		static int x = 100;
		static int y = 100;
		static int speedx = 2;
		static int speedy = 2;
		DrawFontString(N64Font, "test", x, y);
		x += speedx;
		y += speedy;
		if (x+50 > 320) speedx = -2;
		if (x < 0) speedx = 2;
		if (y+50 > 288) speedy = -2;
		if (y < 0) speedy = 2;
		
		UpdateLogs();

		WaitForVideoSync();
	}
}

uint32_t GetDeltaTime()
{
	return __deltaTime;
}
