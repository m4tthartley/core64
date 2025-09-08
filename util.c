//
//  Created by Matt Hartley on 08/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>


void* memset(void* dest, int value, size_t size)
{
	void* result = dest;
	while (size) {
		*(uint8_t*)dest = (uint8_t)value;
		++dest;
		--size;
	}

	return result;
}

void* CopyMemory(uint8_t* dest, const uint8_t* src, size_t num)
{
	// for (int i=0; i<num; ++i) {
	// 	((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
	// }

	while (num) {
		*dest++ = *src++;
		--num;
	}

	return dest;
}

float* CopyFloats(float* out, float* in, size_t num)
{
	for (int i=0; i<num; ++i) {
		// float a = out[i] * in[i];
		// out[i] = a;
		out[i] = in[i];
	}

	return out;
}
