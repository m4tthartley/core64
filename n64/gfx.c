/*
	Created by Matt Hartley on 09/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include <stdbool.h>
#include "math.h"
#include "rdp.h"
#include "draw.h"
#include "gfx.h"


float __gfxPositionOffsetX = 0;
float __gfxPositionOffsetY = 0;
float __gfxPositionOffsetZ = 0;

debugtriangle_t __debugTriangles[1024];
uint32_t __debugTriangleCount = 0;


void GFX_SetPosition(float x, float y, float z)
{
	__gfxPositionOffsetX = x;
	__gfxPositionOffsetY = y;
	__gfxPositionOffsetZ = z;
}

bool GFX_ClipTriangle(vec4_t clip0, vec4_t clip1, vec4_t clip2)
{
	if (
		(clip0.x < -1.0f && clip1.x < -1.0f && clip2.x < -1.0f) ||
		(clip0.x > +1.0f && clip1.x > +1.0f && clip2.x > +1.0f) ||
		(clip0.y < -1.0f && clip1.y < -1.0f && clip2.y < -1.0f) ||
		(clip0.y > +1.0f && clip1.y > +1.0f && clip2.y > +1.0f) ||
		(clip0.z < +0.0f && clip1.z < +0.0f && clip2.z < +0.0f) ||
		(clip0.z > +1.0f && clip1.z > +1.0f && clip2.z > +1.0f)
	) {
		return 0;
	}

	return 1;
}

void GFX_DrawTriangle(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
{
	// v0.pos.x += __gfxPositionOffsetX;
	// v0.pos.y += __gfxPositionOffsetY;
	// v1.pos.x += __gfxPositionOffsetX;
	// v1.pos.y += __gfxPositionOffsetY;
	// v2.pos.x += __gfxPositionOffsetX;
	// v2.pos.y += __gfxPositionOffsetY;

	vec3_t offset = {__gfxPositionOffsetX, __gfxPositionOffsetY, __gfxPositionOffsetZ};
	v0.pos.xyz = add3(v0.pos.xyz, offset);
	v1.pos.xyz = add3(v1.pos.xyz, offset);
	v2.pos.xyz = add3(v2.pos.xyz, offset);

	mat4_t mat = PerspectiveMatrix(90, 320.0f/288.0f, 0.1f, 100.0f);
	vec4_t ndc0 = Vec4MulMat4(vec4(v0.pos.x, v0.pos.y, v0.pos.z, 1), mat);
	vec4_t ndc1 = Vec4MulMat4(vec4(v1.pos.x, v1.pos.y, v1.pos.z, 1), mat);
	vec4_t ndc2 = Vec4MulMat4(vec4(v2.pos.x, v2.pos.y, v2.pos.z, 1), mat);

	if (ndc0.w <= 0 || ndc1.w <= 0 || ndc2.w <= 0) {
		return;
	}

	v0.pos = vec4(ndc0.x/ndc0.w, ndc0.y/ndc0.w, ndc0.z/ndc0.w, ndc0.w);
	v1.pos = vec4(ndc1.x/ndc1.w, ndc1.y/ndc1.w, ndc1.z/ndc1.w, ndc1.w);
	v2.pos = vec4(ndc2.x/ndc2.w, ndc2.y/ndc2.w, ndc2.z/ndc2.w, ndc2.w);

	if (!GFX_ClipTriangle(v0.pos, v1.pos, v2.pos)) {
		return;
	}

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
	
	v0.invw = divsafe(1.0f, ndc0.w);
	v1.invw = divsafe(1.0f, ndc1.w);
	v2.invw = divsafe(1.0f, ndc2.w);

	v0.texcoord.u *= v0.invw;
	v1.texcoord.u *= v1.invw;
	v2.texcoord.u *= v2.invw;
	v0.texcoord.v *= v0.invw;
	v1.texcoord.v *= v1.invw;
	v2.texcoord.v *= v2.invw;
	
	v0.invw *= (float)0x7FFF;
	v1.invw *= (float)0x7FFF;
	v2.invw *= (float)0x7FFF;

	// vec3_t highEdge = sub3(v1.pos.xyz, v0.pos.xyz);
	// vec3_t longEdge = sub3(v2.pos.xyz, v0.pos.xyz);
	// vec3_t normal = normalize3(cross3(highEdge, longEdge));

	vec3_t sunDir = normalize3(vec3(1, 1, 1));
	float ambient = 0.1f;
	float light = ambient + (max(dot3(v0.normal, sunDir), 0.0f) * (1.0f-ambient));

	v0.color.xyz = vec3f(light * 255.0f);
	v1.color.xyz = vec3f(light * 255.0f);
	v2.color.xyz = vec3f(light * 255.0f);

	RDP_Triangle(v0, v1, v2);

	// DrawStrBG(8, 8, "clip z: %f, clip w: %f", v0.pos.z, ndc0.w);
	// DrawStrBG(8, 8*3, "u %f, v %f", v2.texcoord.u, v2.texcoord.v);
	// DrawStrBG(8, 8*2, "invw %f", v0.invw);

	__debugTriangles[__debugTriangleCount++] = (debugtriangle_t){
		{(v0.pos.x), (v0.pos.y)},
		{(v1.pos.x), (v1.pos.y)},
		{(v2.pos.x), (v2.pos.y)},
	};
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
