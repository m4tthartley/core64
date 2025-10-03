/*
	Created by Matt Hartley on 02/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __MATH_HEADER__
#define __MATH_HEADER__

#include <stdint.h>


typedef int16_t fixed16_t; // fixed point s8.8
typedef int32_t fixed32_t; // fixed point s16.16
typedef int16_t fixed12_4_t; // fixed point s12.4

typedef union {
	struct {
		float x, y;
	};
	struct {
		float u, v;
	};
} vec2_t;

typedef union {
	struct {
		float x, y, z;
	};
	struct {
		float r, g, b;
	};
	struct {
		float u, v, w;
	};
} vec3_t;

typedef union {
	struct {
		fixed32_t x, y, z;
	};
	struct {
		fixed32_t r, g, b;
	};
	struct {
		fixed32_t u, v, w;
	};
} vec3fx32_t;

typedef union {
	struct {
		float x, y, z, w;
	};
	struct {
		float r, g, b, a;
	};
} vec4_t;

typedef union {
	struct {
		fixed32_t x, y, z, w;
	};
	struct {
		fixed32_t r, g, b, a;
	};
} vec4fixed32_t;

typedef struct {
	struct {
		uint8_t x, y, z, w;
	};
	struct {
		uint8_t r, g, b, a;
	};
} vec4u8_t;

inline fixed32_t tofixed32(float x)
{
	x *= 65536.0f;
	return (fixed32_t)x;
}

inline vec3_t vec3(float x, float y, float z)
{
	return (vec3_t){x, y, z};
}
inline vec3_t vec3f(float f)
{
	return (vec3_t){f, f, f};
}
inline vec3_t add3(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x+b.x, a.y+b.y, a.z+b.z};
}
inline vec3_t sub3(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x-b.x, a.y-b.y, a.z-b.z};
}
inline vec3_t mul3(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x*b.x, a.y*b.y, a.z*b.z};
}
inline vec3_t mul3f(vec3_t a, float f)
{
	return (vec3_t){a.x*f, a.y*f, a.z*f};
}
inline vec3_t div3(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x/b.x, a.y/b.y, a.z/b.z};
}
inline vec3_t div3f(vec3_t a, float f)
{
	return (vec3_t){a.x/f, a.y/f, a.z/f};
}

inline vec3fx32_t vec3tofixed32(vec3_t v)
{
	v = mul3f(v, 65536.0f);
	return (vec3fx32_t){ v.x, v.y, v.z };
}

inline vec4_t vec4(float x, float y, float z, float w)
{
	return (vec4_t){x, y, z, w};
}
inline vec4_t vec4f(float f)
{
	return (vec4_t){f, f, f, f};
}
inline vec4_t add4(vec4_t a, vec4_t b)
{
	return (vec4_t){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
}
inline vec4_t sub4(vec4_t a, vec4_t b)
{
	return (vec4_t){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
}
inline vec4_t mul4(vec4_t a, vec4_t b)
{
	return (vec4_t){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
}
inline vec4_t mul4f(vec4_t a, float f)
{
	return (vec4_t){a.x*f, a.y*f, a.z*f, a.w*f};
}
inline vec4_t div4(vec4_t a, vec4_t b)
{
	return (vec4_t){a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};
}
inline vec4_t div4f(vec4_t a, float f)
{
	return (vec4_t){a.x/f, a.y/f, a.z/f, a.w/f};
}

inline vec4fixed32_t vec4tofixed32(vec4_t v)
{
	v = mul4f(v, 65536.0f);
	return (vec4fixed32_t){ v.x, v.y, v.z, v.w };
}


#endif
