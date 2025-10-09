/*
	Created by Matt Hartley on 09/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "rdp.h"


void GFX_Draw(rdp_vertex_t* vertices, uint16_t* indices, int num)
{
	assert(num % 3 == 0);

	for (int idx=0; idx<num; idx+=3) {
		RDP_Triangle(
			vertices[indices[idx+0]],
			vertices[indices[idx+1]],
			vertices[indices[idx+2]]
		);
	}
}
