//
//  Created by Matt Hartley on 24/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>
#include <stdarg.h>

#include "core64.h"
#include "util.h"


volatile uint32_t __memorySize;
volatile uint32_t __tvType;
volatile uint32_t __resetType;
volatile uint32_t __consoleType;


// MEMORY
extern uint8_t __heap_start[];
extern uint8_t __heap_end[];
uint32_t __heapCursor = 0;

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

void* AllocMemory(uint32_t size, uint32_t alignment)
{
	assert((alignment & (alignment-1)) == 0);

	uintptr_t addr = (uintptr_t)__heap_start + __heapCursor;

	uint32_t mask = alignment - 1;
	if (addr & mask) {
		addr &= ~mask;
		addr += alignment;
	}

	__heapCursor = (addr - (uintptr_t)__heap_start) + size;

	return (void*)addr;
}


// TIME
uint32_t __lastFrameTime = 0;
uint32_t __deltaTime = 0;

void PollTime()
{
	if (!__lastFrameTime) {
		__lastFrameTime = _GetClock();
	}

	uint32_t clockHz = 46875000;
	uint32_t clocksPerMs = clockHz / 1000;
	uint32_t time = _GetClock();
	uint32_t deltaClocks = time - __lastFrameTime;
	__lastFrameTime = time;

	__deltaTime = deltaClocks / clocksPerMs;
}

uint32_t GetDeltaTime()
{
	return __deltaTime;
}
