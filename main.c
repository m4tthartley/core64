//
//  Created by Matt Hartley on 14/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "n64/core64.h"

#include "n64/registers.h"
#include "n64/rdp.h"
#include "n64/system.c"
#include "n64/print.c"
#include "n64/util.c"
#include "n64/video.c"
#include "n64/interrupts.c"
#include "n64/draw.c"

// #define IMPL
#include "n64/rdp.h"
#include "n64/rdp.c"
#include "n64/math.h"


volatile uint32_t __memorySize;
volatile uint32_t __tvType;
volatile uint32_t __resetType;
volatile uint32_t __consoleType;

uint32_t lastFrameTime;
uint32_t __deltaTime;


extern uint8_t texture_test_start[];
extern uint8_t texture_test_end[];

extern uint8_t __heap_start[];
extern uint8_t __heap_end[];

int main()
{
	lastFrameTime = _GetClock();

	Log("System Started");

	if (/*convert to 16bit*/1) {
		// uint32_t* image = (uint32_t*)texture_test_start;
		// uint16_t* heap = (uint16_t*)__heap_start;
		// for (int pxl=0; pxl<32*32; ++pxl) {
		// 	int a = ((image[pxl]>>24) & 0xFF) / 8;
		// 	int r = ((image[pxl]>>16) & 0xFF) / 8;
		// 	int g = ((image[pxl]>>8) & 0xFF) / 8;
		// 	int b = ((image[pxl]>>0) & 0xFF) / 8;
		// 	// heap[pxl] = (r<<11) | (g<<6) | (b<<1) | 1;
		// 	heap[pxl] = 0xF800;
		// }
	} else {
		// CopyMemory(__heap_start, texture_test_start, 32*32*4);
	}

	CopyMemory(__heap_start, texture_test_start, 32*32*2);
	DataCacheWritebackInvalidate(__heap_start, 32*32*2);
	// DataCacheWritebackInvalidate(texture_test_start, 32*32*2);

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
	

			rdpcmdlist_t cmdlist = RDP_CmdList(_commandList, sizeof(_commandList));
			RDP_StartCmdList(0, 0);
	
			// RDP_PipeSync();
			RDP_SetOtherModes(RDP_MODE_FILL);

			// // SetColorImage
			// cmd.word0 = (0x3F<<24) | (0<<21) | (2<<19) | (320-1);
			// cmd.word1 = VI_FRAMEBUFFERBASE & 0xFFFFFF;
			// RDP_Write(&cmdlist, cmd.word0);
			// RDP_Write(&cmdlist, cmd.word1);
			RDP_SetColorImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 320);
	
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

			RDP_PipeSync();
	
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

			RDP_FillRect(ox, oy, w, h);
			// RDP_FillRect(&cmdlist, ox, oy, w/2, h/2);
			// RDP_FillRect(&cmdlist, ox + w/2, oy, w/2, h/2);
			// RDP_FillRect(&cmdlist, ox, oy + h/2, w/2, h/2);
			// RDP_FillRect(&cmdlist, ox + w/2, oy + h/2, w/2, h/2);

			// Sync between different draw operations
			RDP_PipeSync();
	
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
			RDP_FillTriangle(vertsa);
	
			// more triangles
			RDP_FillTriangle((vecscreen_t[]){
				{140, 40},
				{100, 60},
				{120, 30},
			});

			RDP_PipeSync();
			
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
			RDP_FillRect(x, y, 50, 50);
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


			RDP_PipeSync();
			RDP_SetOtherModes(RDP_MODE_1CYCLE | RDP_MODE_TEX_RGB);
			RDP_SetPrimitiveColor(Color32(0, 255, 255, 255));
			RDP_SetEnvironmentColor(0x00FFFFFF);

			uint32_t tileWidth = 32;
			uint32_t tileHeight = 32;
			RDP_SetTextureImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, __heap_start);
			RDP_PipeSync();

			RDP_SetTile(7, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, 0);

			RDP_LoadTile(7, 0, 0, tileWidth, tileHeight);
			// RDP_LoadBlock(7, 0, 0, 32*32);

			// // Load Sync
			// RDP_Write(&cmdlist, 0x26000000);
			// RDP_Write(&cmdlist, 0);
			RDP_LoadSync();

			// // Set Tile
			// uint32_t line = 32*2 / 8;
			// RDP_Write(&cmdlist, (0x35<<24) | (/*format*/0<<21) | (/*size*/2<<19) | (line<<9) | (/*tmem addr*/0));
			// RDP_Write(&cmdlist, (/*tile*/0<<24));
			RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, 0);

			// // Set Tile Size
			// RDP_Write(&cmdlist, (0x32<<24));
			// RDP_Write(&cmdlist, (/*tile*/0<<24) | (((tileWidth-1)<<2)<<12) | (((tileHeight-1)<<2)<<0));
			RDP_SetTileSize(0, 32, 32);

			// Set Combine Mode
			RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_PRIMITIVE, 0));

			RDP_PipeSync();
			
			// Texture Rectangle
			{
				uint32_t w = 100;
				uint32_t h = 100;
				// u10.2 format
				uint32_t x0 = (75) << 2;
				uint32_t y0 = (150) << 2;
				uint32_t x1 = (75+w) << 2;
				uint32_t y1 = (150+h) << 2;
				RDP_Write(&cmdlist, (0x24<<24) | (x1<<12) | (y1<<0));
				RDP_Write(&cmdlist, (0<<24) | (x0<<12) | (y0<<0));
				
				int32_t u = ((0)<<5) & 0xFFFF;
				int32_t v = ((0)<<5) & 0xFFFF;
				int32_t du = ((tileWidth)<<10) / 100;
				int32_t dv = ((tileHeight)<<10) / 100;
				RDP_Write(&cmdlist, (u<<16) | (v<<0));
				RDP_Write(&cmdlist, (du<<16) | (dv<<0));
			}
			
			RDP_PipeSync();
			
			// Set Combine Mode
			RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_SHADE, 0));
			
			// Shaded triangle
			rdp_vertex_t verts[] = {
				{{200, 50}, {255, 255, 255, 255}, {0.0f, 0.0f}},
				{{260, 100}, {0, 0, 0, 255}, {32.0f, 0.0f}},
				{{230, 150}, {255, 255, 255, 255}, {32.0f, 32.0f}},
			};
			RDP_FillTriangleWithShade(verts);


			RDP_FullSync();
			RDP_ExecuteAndWait();
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

		sprint(str, 64, "texture addr: %32x -> %32x", (uintptr_t)texture_test_start, (uintptr_t)texture_test_end);
		DrawFontStringWithBG(N64Font, str, 10, 80);
		sprint(str, 64, "heap: %32x -> %32x", (uintptr_t)__heap_start, (uintptr_t)__heap_end);
		DrawFontStringWithBG(N64Font, str, 10, 90);

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

		BlitTexture(texture_test_start, 100, 100, 0, 0, 32, 32);
		
		UpdateLogs();

		WaitForVideoSync();
	}
}

uint32_t GetDeltaTime()
{
	return __deltaTime;
}
