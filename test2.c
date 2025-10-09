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


// uint32_t lastFrameTime;
// uint32_t __deltaTime;


extern uint8_t texture_test_start[];
extern uint8_t texture_test_end[];


// LOGS
typedef struct {
	char str[64];
	int32_t timer;
} log_t;

log_t logs[64];
uint32_t logIndex = 0;

void Log(char* str, ...)
{
	va_list args;
	va_start(args, str);


	vsprint(logs[logIndex].str, 64, str, args);
	logs[logIndex].timer = 10000;
	++logIndex;
	logIndex &= 63;

	va_end(args);
}

void UpdateLogs()
{
	int y = 0;
	for (int idx=0; idx<64; ++idx) {
		if (logs[idx].timer > 0) {
			logs[idx].timer -= GetDeltaTime();
			DrawFontStringWithBG(N64Font, logs[idx].str, 8, 8 + y);
			y += 8;
		}
	}
}


int main()
{
	// lastFrameTime = _GetClock();

	Log("System Started");

	CopyMemory(__heap_start, texture_test_start, 32*32*2);
	DataCacheWritebackInvalidate(__heap_start, 32*32*2);

	// BOOT TESTING
// #if 0
// 	volatile bootinfo_t* bootInfo = (volatile bootinfo_t*)0x80000300;
	
// 	#define RDRAM_BASE 0x80000000
// 	#define RDRAM_SIZE_4MB 0x00400000
// 	#define RDRAM_SIZE_8MB 0x00800000
	
// 	__memorySize = RDRAM_SIZE_4MB;
// 	// #define RDRAM_BASE 0x80000000

// 	volatile uint32_t* probeAddr = (volatile uint32_t*)(RDRAM_BASE + RDRAM_SIZE_4MB);
// 	uint32_t tmp = *probeAddr;
// 	*probeAddr = 0xCAFEBABE;
// 	__asm__ volatile("sync");
// 	if (*probeAddr == 0xCAFEBABE) {
// 		__memorySize = RDRAM_SIZE_8MB;
// 	}
// 	*probeAddr = tmp;

// 	#define NTSC 0
// 	#define PAL 1
// 	#define MPAL 2

// 	// tv type
// 	#define PIF_RAM ((volatile uint8_t*)0xBFC00700);
// 	volatile uint8_t* pif = PIF_RAM;
// 	__tvType = pif[0x24];
// #endif
	////////

	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	InitDefaultVI();

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
		PollTime();

		RDP_StartCmdList(0, 0);

		RDP_SetColorImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 320);
		RDP_SetScissor(0, 0, 320, 288);
		
		RDP_SetOtherModes(RDP_MODE_FILL);
		RDP_SetFillColor16((8<<1) | 1);
		// RDP_SetFillColor32(0xFF00FFFF);

		int ox = 1;
		int oy = 1;
		int w = 320-2;
		int h = 288-2;
		RDP_FillRect(ox, oy, w, h);

		// Sync between different draw operations
		RDP_PipeSync();

		RDP_SetFillColor16((31<<6) | (12<<1) | 1);
		vecscreen_t verts[] = {
			{50, 50},
			{200, 150},
			{150, 200},
		};
		RDP_FillTriangle(verts);
		RDP_FillTriangle((vecscreen_t[]){
			{140, 40},
			{100, 60},
			{120, 30},
		});
		RDP_PipeSync();
		
		{
			RDP_SetFillColor16((31<<11) | (12<<1) | 1);

			static int x = 100;
			static int y = 100;
			static int speedx = 2;
			static int speedy = 2;

			RDP_FillRect(x, y, 50, 50);

			x += speedx;
			y += speedy;
			if (x+50 > 320) speedx = -2;
			if (x < 0) speedx = 2;
			if (y+50 > 288) speedy = -2;
			if (y < 0) speedy = 2;

			RDP_PipeSync();
		}

		RDP_SetOtherModes(RDP_MODE_1CYCLE | RDP_MODE_TEX_RGB);
		RDP_SetPrimitiveColor(Color32(0, 255, 255, 255));
		RDP_SetEnvironmentColor(0x00FFFFFF);
		
		uint32_t tileWidth = 32;
		uint32_t tileHeight = 32;
		RDP_SetTextureImage(RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, __heap_start);
		RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, 0);
		RDP_LoadTile(0, 0, 0, tileWidth, tileHeight);
		RDP_LoadSync();
		// RDP_SetTile(0, RDP_TEXTURE_IMAGE_FORMAT_RGBA16, 32, 0);
		RDP_SetTileSize(0, 32, 32);
		// NOTE: These 2 ^ actually don't seem to be needed

		RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_PRIMITIVE, 0));
		RDP_TextureRectangle(75, 150, 100, 100, 0, 0, 32, 32);
		RDP_PipeSync();
		
		RDP_SetCombineMode(RDP_CombinerRGB(RDP_COMB_TEX0, 0, RDP_COMB_SHADE, 0));
		rdp_vertex_t verts2[] = {
			{{200, 50}, {255, 255, 255, 255}, {0.0f, 0.0f}},
			{{260, 100}, {0, 0, 0, 255}, {32.0f, 0.0f}},
			{{230, 150}, {255, 255, 255, 255}, {32.0f, 32.0f}},
		};
		RDP_FillTriangleWithShade(verts2);

		RDP_FullSync();
		RDP_ExecuteAndWait();


		for (int x=0; x<16; ++x) {
			fb[pixel] = 0xFFFF;
			++pixel;
			pixel %= (320*240);
		}

		char str[64];
		sprint(str, 64, "%32x", __tvType);
		DrawFontString(N64Font, str, 10, 10);
		sprint(str, 64, "reset type: %32x", __resetType);
		DrawFontString(N64Font, str, 10, 20);
		sprint(str, 64, "memory size: %32x", __memorySize);
		DrawFontString(N64Font, str, 10, 30);
		sprint(str, 64, "console type: %32x", __consoleType);
		DrawFontString(N64Font, str, 10, 40);

		uint32_t delta = GetDeltaTime();
		sprint(str, 64, "clock: %u", delta);
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
