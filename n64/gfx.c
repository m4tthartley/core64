/*
	Created by Matt Hartley on 09/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "math.h"
#include "rdp.h"
#include "gfx.h"
#include "draw.h"


float __gfxPositionOffsetX = 0;
float __gfxPositionOffsetY = 0;


void GFX_SetPosition(float x, float y)
{
	__gfxPositionOffsetX = x;
	__gfxPositionOffsetY = y;
}

void GFX_DrawTriangle(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
{
	// v0.pos.x += __gfxPositionOffsetX;
	// v0.pos.y += __gfxPositionOffsetY;
	// v1.pos.x += __gfxPositionOffsetX;
	// v1.pos.y += __gfxPositionOffsetY;
	// v2.pos.x += __gfxPositionOffsetX;
	// v2.pos.y += __gfxPositionOffsetY;
	v0.pos.z -= 3;
	v1.pos.z -= 3;
	v2.pos.z -= 3;

	mat4_t mat = PerspectiveMatrix(90, 320.0f/288.0f, 0.1f, 100.0f);
	vec4_t ndc0 = Vec4MulMat4(vec4(v0.pos.x, v0.pos.y, v0.pos.z, 1), mat);
	vec4_t ndc1 = Vec4MulMat4(vec4(v1.pos.x, v1.pos.y, v1.pos.z, 1), mat);
	vec4_t ndc2 = Vec4MulMat4(vec4(v2.pos.x, v2.pos.y, v2.pos.z, 1), mat);

	v0.pos = vec4(ndc0.x/ndc0.w, ndc0.y/ndc0.w, ndc0.z/ndc0.w, ndc0.w);
	v1.pos = vec4(ndc1.x/ndc1.w, ndc1.y/ndc1.w, ndc1.z/ndc1.w, ndc1.w);
	v2.pos = vec4(ndc2.x/ndc2.w, ndc2.y/ndc2.w, ndc2.z/ndc2.w, ndc2.w);

	v0.pos.x = (v0.pos.x*0.5f + 0.5f) * 320.0f;
	v0.pos.y = (-v0.pos.y*0.5f + 0.5f) * 288.0f;
	v1.pos.x = (v1.pos.x*0.5f + 0.5f) * 320.0f;
	v1.pos.y = (-v1.pos.y*0.5f + 0.5f) * 288.0f;
	v2.pos.x = (v2.pos.x*0.5f + 0.5f) * 320.0f;
	v2.pos.y = (-v2.pos.y*0.5f + 0.5f) * 288.0f;
	
	v0.texcoord.u *= 32;
	v0.texcoord.v *= 32;
	v1.texcoord.u *= 32;
	v1.texcoord.v *= 32;
	v2.texcoord.u *= 32;
	v2.texcoord.v *= 32;
	
	v0.invw = (1.0f / ndc0.w);
	v1.invw = (1.0f / ndc1.w);
	v2.invw = (1.0f / ndc2.w);

	v0.texcoord.u *= v0.invw;
	v1.texcoord.u *= v1.invw;
	v2.texcoord.u *= v2.invw;
	v0.texcoord.v *= v0.invw;
	v1.texcoord.v *= v1.invw;
	v2.texcoord.v *= v2.invw;
	
	v0.invw *= (float)0x7FFF;
	v1.invw *= (float)0x7FFF;
	v2.invw *= (float)0x7FFF;
	RDP_Triangle(v0, v1, v2);

	// DrawStrBG(8, 8, "clip z: %f, clip w: %f", v0.pos.z, ndc0.w);
	// DrawStrBG(8, 8*3, "u %f, v %f", v2.texcoord.u, v2.texcoord.v);
	// DrawStrBG(8, 8*2, "invw %f", v0.invw);
}

void GFX_DrawIndices(rdp_vertex_t* vertices, uint16_t* indices, int num)
{
	assert(num % 3 == 0);

	for (int idx=0; idx<num; idx+=3) {
		rdp_vertex_t v0 = vertices[indices[idx+0]];
		rdp_vertex_t v1 = vertices[indices[idx+1]];
		rdp_vertex_t v2 = vertices[indices[idx+2]];

		GFX_DrawTriangle(v0, v1, v2);
	}
}

void GFX_DrawVertices(rdp_vertex_t* vertices, int num)
{
	assert(num % 3 == 0);

	for (int idx=0; idx<num; idx+=3) {
		GFX_DrawTriangle(vertices[idx+0], vertices[idx+1], vertices[idx+2]);
	}
}
