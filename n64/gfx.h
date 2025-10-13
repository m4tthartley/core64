/*
	Created by Matt Hartley on 09/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __GFX_HEADER__
#define __GFX_HEADER__

#include "util.h"
#include "rdp.h"


void GFX_SetPosition(float x, float y);
void GFX_DrawIndices(rdp_vertex_t* vertices, uint16_t* indices, int num);
void GFX_DrawVertices(rdp_vertex_t* vertices, int num);


#endif