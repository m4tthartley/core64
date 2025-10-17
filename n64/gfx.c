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

vec3_t __lights[] = {
	{2, 0, -5},
};

debugtriangle_t __debugTriangles[1024];
uint32_t __debugTriangleCount = 0;


void GFX_SetPosition(float x, float y, float z)
{
	__gfxPositionOffsetX = x;
	__gfxPositionOffsetY = y;
	__gfxPositionOffsetZ = z;
}

// bool GFX_ClipTriangle(vec4_t clip0, vec4_t clip1, vec4_t clip2)
// {
// 	if (
// 		(clip0.x < -1.0f && clip1.x < -1.0f && clip2.x < -1.0f) ||
// 		(clip0.x > +1.0f && clip1.x > +1.0f && clip2.x > +1.0f) ||
// 		(clip0.y < -1.0f && clip1.y < -1.0f && clip2.y < -1.0f) ||
// 		(clip0.y > +1.0f && clip1.y > +1.0f && clip2.y > +1.0f) ||
// 		(clip0.z < +0.0f && clip1.z < +0.0f && clip2.z < +0.0f) ||
// 		(clip0.z > +1.0f && clip1.z > +1.0f && clip2.z > +1.0f)
// 	) {
// 		return 0;
// 	}

// 	return 1;
// }

typedef struct {
	rdp_vertex_t outVertices[16];
	rdp_vertex_t inVertices[16];
	int outCount;
	int inCount;
} clipstate_t;

typedef vec3_t plane_t;
plane_t planeX0 = {+1, 0, 0};
plane_t planeX1 = {-1, 0, 0};
plane_t planeY0 = {0, +1, 0};
plane_t planeY1 = {0, -1, 0};
plane_t planeZ0 = {0, 0, +1};
plane_t planeZ1 = {0, 0, -1};

float GFX_ClipPlaneDist(vec4_t pos, plane_t plane)
{
	return pos.x*plane.x + pos.y*plane.y + pos.z*plane.z + pos.w;
}

void GFX_ClipWithPlane(clipstate_t* clip, plane_t plane)
{
	int previ = clip->inCount-1;
	for (int vi=0; vi<clip->inCount; ++vi) {
		// test x min clip
		rdp_vertex_t v0 = clip->inVertices[vi];
		rdp_vertex_t v1 = clip->inVertices[previ];
		// bool swapped = 0;
		// if (v0.pos.x > v1.pos.x) {
		// 	swap(v0, v1);
		// 	swapped = 1;
		// }
		float d0 = GFX_ClipPlaneDist(v0.pos, plane);
		float d1 = GFX_ClipPlaneDist(v1.pos, plane);
		// bool inclip0 = v0.pos.x + v0.pos.w >= 0;
		// bool inclip1 = v1.pos.x + v1.pos.w >= 0;
		bool inclip0 = d0 >= 0;
		bool inclip1 = d1 >= 0;
		// DrawStrBG(10, 20, "clip %i %i", inclip0, inclip1);
		if (inclip0 != inclip1) {
			// interpolate
			// float t = (v1.pos.x-v0.pos.x) / ()
			float t = divsafe(d0, (d0 - d1));
			rdp_vertex_t interp;
			interp.pos.x = v0.pos.x + t*(v1.pos.x-v0.pos.x);
			interp.pos.y = v0.pos.y + t*(v1.pos.y-v0.pos.y);
			interp.pos.z = v0.pos.z + t*(v1.pos.z-v0.pos.z);
			interp.pos.w = v0.pos.w + t*(v1.pos.w-v0.pos.w);

			interp.normal.x = v0.normal.x + t*(v1.normal.x-v0.normal.x);
			interp.normal.y = v0.normal.y + t*(v1.normal.y-v0.normal.y);
			interp.normal.z = v0.normal.z + t*(v1.normal.z-v0.normal.z);

			interp.color.r = v0.color.r + t*(v1.color.r-v0.color.r);
			interp.color.g = v0.color.g + t*(v1.color.g-v0.color.g);
			interp.color.b = v0.color.b + t*(v1.color.b-v0.color.b);
			interp.color.a = v0.color.a + t*(v1.color.a-v0.color.a);

			interp.texcoord.u = v0.texcoord.u + t*(v1.texcoord.u-v0.texcoord.u);
			interp.texcoord.v = v0.texcoord.v + t*(v1.texcoord.v-v0.texcoord.v);

			clip->outVertices[clip->outCount++] = interp;
		}
		// else if (inclip0 && inclip1) {
		// 	clip->outVertices[clip->outCount++] = clip->inVertices[vi];
		// }

		if (inclip0) {
			clip->outVertices[clip->outCount++] = clip->inVertices[vi];
		}

		// clip->outVertices[clip->outCount++] = clip->inVertices[vi];

		// outVerts[(*outNum)++] = inVerts[vi];

		previ = vi;
	}

	for (int idx=0; idx<clip->outCount; ++idx) {
		clip->inVertices[idx] = clip->outVertices[idx];
	}
	clip->inCount = clip->outCount;
	clip->outCount = 0;
}

void GFX_DrawClippedTriangle(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
{
	vec4_t clip0 = v0.pos;
	vec4_t clip1 = v1.pos;
	vec4_t clip2 = v2.pos;
	v0.pos = vec4(divsafe(clip0.x, clip0.w), divsafe(clip0.y, clip0.w), divsafe(clip0.z, clip0.w), clip0.w);
	v1.pos = vec4(divsafe(clip1.x, clip1.w), divsafe(clip1.y, clip1.w), divsafe(clip1.z, clip1.w), clip1.w);
	v2.pos = vec4(divsafe(clip2.x, clip2.w), divsafe(clip2.y, clip2.w), divsafe(clip2.z, clip2.w), clip2.w);

	// if (!GFX_ClipTriangle(v0.pos, v1.pos, v2.pos)) {
	// 	return;
	// }

	v0.pos.x = (v0.pos.x*0.5f + 0.5f) * 320.0f;
	v0.pos.y = (-v0.pos.y*0.5f + 0.5f) * 288.0f;
	v1.pos.x = (v1.pos.x*0.5f + 0.5f) * 320.0f;
	v1.pos.y = (-v1.pos.y*0.5f + 0.5f) * 288.0f;
	v2.pos.x = (v2.pos.x*0.5f + 0.5f) * 320.0f;
	v2.pos.y = (-v2.pos.y*0.5f + 0.5f) * 288.0f;
	// v0.pos.x = (v0.pos.x*0.5f + 0.5f) * (320.0f*0.75f) + (320.0f*0.125f);
	// v0.pos.y = (-v0.pos.y*0.5f + 0.5f) * (288.0f*0.75f) + (288.0f*0.125f);
	// v1.pos.x = (v1.pos.x*0.5f + 0.5f) * (320.0f*0.75f) + (320.0f*0.125f);
	// v1.pos.y = (-v1.pos.y*0.5f + 0.5f) * (288.0f*0.75f) + (288.0f*0.125f);
	// v2.pos.x = (v2.pos.x*0.5f + 0.5f) * (320.0f*0.75f) + (320.0f*0.125f);
	// v2.pos.y = (-v2.pos.y*0.5f + 0.5f) * (288.0f*0.75f) + (288.0f*0.125f);
	
	v0.texcoord.u *= 32;
	v0.texcoord.v *= 32;
	v1.texcoord.u *= 32;
	v1.texcoord.v *= 32;
	v2.texcoord.u *= 32;
	v2.texcoord.v *= 32;
	
	v0.invw = divsafe(1.0f, clip0.w);
	v1.invw = divsafe(1.0f, clip1.w);
	v2.invw = divsafe(1.0f, clip2.w);

	float minw = 1.0f / max(max(v0.invw, v1.invw), v2.invw);
	v0.invw *= minw;
	v1.invw *= minw;
	v2.invw *= minw;

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

	__debugTriangles[__debugTriangleCount++] = (debugtriangle_t){
		{(v0.pos.x), (v0.pos.y)},
		{(v1.pos.x), (v1.pos.y)},
		{(v2.pos.x), (v2.pos.y)},
	};
}

void GFX_DrawTriangleClip(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
{
	// Clipping
	// rdp_vertex_t vertices[16];
	// int numVertices = 0;
	// rdp_vertex_t inVerts[] = {v0, v1, v2};

	clipstate_t clip = {0};
	clip.inCount = 3;
	clip.inVertices[0] = v0;
	clip.inVertices[1] = v1;
	clip.inVertices[2] = v2;

	GFX_ClipWithPlane(&clip, planeX0);
	GFX_ClipWithPlane(&clip, planeX1);
	GFX_ClipWithPlane(&clip, planeY0);
	GFX_ClipWithPlane(&clip, planeY1);
	GFX_ClipWithPlane(&clip, planeZ0);
	GFX_ClipWithPlane(&clip, planeZ1);

	for (int idx=0; idx<(clip.inCount-2); ++idx) {
		GFX_DrawClippedTriangle(clip.inVertices[0], clip.inVertices[idx+1], clip.inVertices[idx+2]);
	}

	// DrawStrBG(10, 10, "num vertices %i", numVertices);
}

float __debugSmallestW;

vec3_t GFX_VertexLight(rdp_vertex_t v)
{
	// vec3_t highEdge = sub3(v1.pos.xyz, v0.pos.xyz);
	// vec3_t longEdge = sub3(v2.pos.xyz, v0.pos.xyz);
	// vec3_t normal = normalize3(cross3(highEdge, longEdge));

	// vec3_t sunDir = normalize3(vec3(1, 1, 1));
	float ambient = 0.1f;
	// float light = ambient + (max(dot3(v0.normal, sunDir), 0.0f) * (1.0f-ambient));

	float light = ambient;
	for (int idx=0; idx<arraysize(__lights); ++idx) {
		vec3_t diff = sub3(__lights[idx], v.pos.xyz);
		vec3_t dir = normalize3(diff);
		float diffuse = max(dot3(v.normal, dir), 0.0f);// * (1.0f-ambient);

		float dist = len3(sub3(__lights[idx], v.pos.xyz));
		float attenuation = 1.0f / (1.0f + (dist));

		light += diffuse * attenuation;
	}

	return vec3f(light);
}

void GFX_DrawTriangle3D(rdp_vertex_t v0, rdp_vertex_t v1, rdp_vertex_t v2)
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

	{
		

		v0.color.xyz = mul3(GFX_VertexLight(v0), vec3f(255.0f));
		v1.color.xyz = mul3(GFX_VertexLight(v1), vec3f(255.0f));
		v2.color.xyz = mul3(GFX_VertexLight(v2), vec3f(255.0f));
	}

	mat4_t mat = PerspectiveMatrix(90, 320.0f/288.0f, 0.1f, 100.0f);
	vec4_t clip0 = Vec4MulMat4(vec4(v0.pos.x, v0.pos.y, v0.pos.z, 1), mat);
	vec4_t clip1 = Vec4MulMat4(vec4(v1.pos.x, v1.pos.y, v1.pos.z, 1), mat);
	vec4_t clip2 = Vec4MulMat4(vec4(v2.pos.x, v2.pos.y, v2.pos.z, 1), mat);

	if (clip0.w < __debugSmallestW) __debugSmallestW = clip0.w;
	if (clip1.w < __debugSmallestW) __debugSmallestW = clip1.w;
	if (clip2.w < __debugSmallestW) __debugSmallestW = clip2.w;

	// if (clip0.w <= 0 || clip1.w <= 0 || clip2.w <= 0) {
	// 	return;
	// }

	v0.pos = clip0;
	v1.pos = clip1;
	v2.pos = clip2;
	GFX_DrawTriangleClip(v0, v1, v2);
}

void GFX_DrawIndices(rdp_vertex_t* vertices, uint16_t* indices, int num)
{
	assert(num % 3 == 0);

	for (int idx=0; idx<num; idx+=3) {
		rdp_vertex_t v0 = vertices[indices[idx+0]];
		rdp_vertex_t v1 = vertices[indices[idx+1]];
		rdp_vertex_t v2 = vertices[indices[idx+2]];

		GFX_DrawTriangle3D(v0, v1, v2);
	}
}

void GFX_DrawVertices(rdp_vertex_t* vertices, int num)
{
	assert(num % 3 == 0);

	for (int idx=0; idx<num; idx+=3) {
		GFX_DrawTriangle3D(vertices[idx+0], vertices[idx+1], vertices[idx+2]);
	}
}

void GFX_DrawQuadBuffer(rdp_vertex_t* vertices, int num)
{
	assert(!(num & 0b11));

	for (int idx=0; idx<num; idx+=4) {
		GFX_DrawTriangle3D(vertices[idx+0], vertices[idx+1], vertices[idx+2]);
		GFX_DrawTriangle3D(vertices[idx+0], vertices[idx+2], vertices[idx+3]);
	}
}
