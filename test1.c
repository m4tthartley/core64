//
//  Created by Matt Hartley on 20/07/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

/*
	TODO
	-	Separate ROM and RAM memory
		data shouldn't be copied over to RAM automatically
		as apparently that won't work on real hardware
		research this

	- Flush cache after copying data to RAM
*/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "n64/print.c"
#include "n64/util.c"
#include "n64/video.c"
#include "n64/interrupts.c"
#include "n64/draw.c"
#include "n64/core64.h"



#if 0
void DrawRandomIntoFramebuffer32()
{
	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	for (int y=0; y<240; ++y) {
		for (int x=0; x<320; ++x) {
			uint32_t r = Rand32();
			fb[y*320+x] = r;
		}
	}
}
#endif



int main()
{
	InitDefaultVI();

	volatile uint16_t* fb = (void*)VI_FRAMEBUFFERBASE;

	uint16_t color = 0;
	uint32_t fbIndex = 0;
	for (;;) {
		// clear
		uint64_t* fbData = (uint64_t*)fb;
		for (int idx=0; idx<(320*240/4); ++idx) {
			fbData[idx] = 0;
		}

		// draw pattern
		for (int idx=0; idx<320*2; ++idx) {
			fb[fbIndex] = (color) ;
			++fbIndex;
			fbIndex %= (320*240);
			++color;
		}
		++color;

		fb[10*320+10] = 0xFFFF;
		fb[10*320+11] = 0xF0F0;

		char str[64];
		sprint(str, 64, "Interrupt Count: %u", __viInterruptCounter);
		DrawFontString(N64Font, str, 20, 20);
		DrawFontString(N64Font, "HELLO WORLD 256.4096", 20, 30);

		if (fbIndex > 1024) {
			// Trigger error to test exceptions
			// int* asd = 0;
			// *asd = 5;
		}

		fb[10*320+20] = 0xFFFF;
		WaitForVideoSync();
	}


	return 0;
}
