//
//  Created by Matt Hartley on 24/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>
#include <stdarg.h>

#include "core64.h"


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

// TODO: Should this be volatile or not?
void DataCacheWritebackInvalidate(void* addr, uint32_t size)
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

// uintptr_t PhysicalAddress(uintptr_t addr)
// {
// 	return addr & 0x1FFFFFFF;
// }
