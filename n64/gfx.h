/*
	Created by Matt Hartley on 09/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __GFX_HEADER__
#define __GFX_HEADER__

#include "util.h"
#include "rdp.h"


typedef int16_t fx_t; // fixed point s8.8
typedef int16_t fxlo_t; // fixed point s12.4
typedef int32_t fxhi_t; // fixed point s16.16
typedef int64_t fxdhi_t; // fixed point s32.32
typedef union {
	struct {
		fx_t x, y;
	};
	struct {
		fx_t s, t;
	};
} fx2_t;
typedef union {
	struct {
		fxlo_t x, y;
	};
	struct {
		fxlo_t s, t;
	};
} fxlo2_t;
typedef union {
	struct {
		fxhi_t x, y;
	};
	struct {
		fxhi_t s, t;
	};
} fxhi2_t;

typedef struct {
	// fxlo2_t v0;
	// fxlo2_t v1;
	// fxlo2_t v2;
	vec2_t v0;
	vec2_t v1;
	vec2_t v2;
} debugtriangle_t;

#define FxMultiplier 0x100
#define FxloMultiplier 0x10
#define FxhiMultiplier 0x10000
#define FloatToFx(f) ((fx_t)(f * (float)0x100))
#define FloatToFxlo(f) ((fxlo_t)(f * (float)0x10))
#define FloatToFxhi(f) ((fxhi_t)(f * (float)0x10000))
#define FloatToFxdhi(f) ((fxdhi_t)(f * (float)0x100000000))
#define FxloToFxhi(fx) ((fxhi_t)(fx * 0x1000))

#define QUAD_INDICES(offset) (offset*4+0), (offset*4+1), (offset*4+2), (offset*4+0), (offset*4+2), (offset*4+3)

void GFX_SetPosition(float x, float y, float z);
void GFX_DrawIndices(rdp_vertex_t* vertices, uint16_t* indices, int num);
void GFX_DrawVertices(rdp_vertex_t* vertices, int num);


#endif