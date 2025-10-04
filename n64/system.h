/*
	Created by Matt Hartley on 04/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __SYSTEM_HEADER__
#define __SYSTEM_HEADER__

#include "util.h"


#define PhysicalAddress(addr) (addr & 0x1FFFFFFF)
#define UncachedAddress(addr) (PhysicalAddress(addr) | 0xA0000000)

void DataCacheWritebackInvalidate(void* addr, uint32_t size);
void MemoryBarrier();


#endif
