//
//  Created by Matt Hartley on 13/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>


uint32_t randState32 = 0;
uint64_t pcgState = 0x853C49E6748FEA9BULL;
uint64_t pcgInc = 0xDA3E39CB94B95BDBULL;

uint32_t RandXorShift32()
{
	if (!randState32) {
		randState32 = 2463534242;
	}

	randState32 ^= randState32 << 13;
	randState32 ^= randState32 << 17;
	randState32 ^= randState32 << 5;
	return randState32;
}

uint32_t RandLCG()
{
	if (!randState32) {
		randState32 = 1;
	}
	randState32 = 1664525 * randState32 + 1013904223;
	return randState32;
}

uint32_t RandPCG()
{
	uint64_t oldState = pcgState;
	pcgState = oldState * 6364136223846793005ULL + (pcgInc | 1);
	uint32_t xor = ((oldState >> 18u) ^ oldState) >> 27u;
	uint32_t rot = oldState >> 59u;
	return (xor >> rot) | (xor << ((-rot) & 31));
}

uint32_t Rand32()
{
	return RandPCG();
}

uint32_t Rand32Range(uint32_t min, uint32_t max)
{
	uint32_t range = max - min;
	return min + (Rand32() % range);
}
