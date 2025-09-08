//
//  Created by Matt Hartley on 07/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <stdint.h>


void DrawRandomNoise(volatile uint32_t* fb)
{
	// for (int y=0; y<240; ++y) {
	// 	uint16_t c = 31;//(y & 0x1F) << 11;
	// 	for (int x=0; x<320; x += 1) {
	// 		fb[y*320+x] = color | (green<<5) | (y%31 << 11);
	// 		if (!color) {
	// 			color = 31;
	// 		} else {
	// 			color = 0;
	// 		}
	// 		// color += 1;
	// 		++green;
	// 		green = green % 63;
	// 	}

	// }

	// fb[0] = 0xFFFF;
	// fb[320 * 100] = 0xF800;
	// fb[320 * 200] = 0xF800;
	// fb[320 * 239] = 0xF800;
	// fb[320 * 1] = 0xF800;
	// fb[320 * 3] = 0xF800;

	// for (int y=0; y<2; ++y) {
	// 	fb[y*320+0] = 31;
	// }

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	for (;;) {
		// uint16_t c = (y & 0x1F) << 11;
		for (int y=0; y<240; ++y) {
			for (int x=0; x<320; ++x) {
				// fb[y*320+x] = (b+=1)<<8 | (g+=2)<<16 | (r+=3)<<24;
				// fb[y*320+x] = ((b+=1)%32) | ((g+=2)%32)<<5 | ((r+=4)%32)<<10;

				uint32_t r = Rand32();

				fb[y*320+x] = r;
				// fb[y*320+x] = clearColor;
				// ++clearColor;
			}
		}

		fb[0*320+10] = 0xFFFF;

		// for (int i=0; i<256; ++i) {
		// 	fa[i] *= fb[i];
		// }

		WaitForVideoSync();
	}
}