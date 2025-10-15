/*
	Created by Matt Hartley on 02/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __MATH_HEADER__
#define __MATH_HEADER__

#include <stdint.h>

#include "util.h"
#include "../resources/tables.h"


float _Sqrt(float x);


#define PI 3.14159265359f
#define PI2 (PI*2.0f)

// PI*65536 = 205887.4161456742
#define PIFX 205887
#define PI2FX (PIFX * 2)

#define EPSILON 1e-10f

typedef int16_t fixed16_t; // fixed point s8.8
typedef int32_t fixed32_t; // fixed point s16.16
typedef fixed32_t fx32;
typedef int16_t fixed12_4_t; // fixed point s12.4

typedef union {
	struct {
		float x, y;
	};
	struct {
		float u, v;
	};
	float f[2];
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
	float f[3];
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
	vec3_t xyz;
	float f[4];
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

inline fixed32_t itofx32(int32_t x)
{
	return x * 0x10000;
}

inline float fx32tof(fixed32_t x)
{
	float result = x;
	result /= 65536.0f;
	return result;
}

inline float divsafe(float x, float div)
{
	if (div < EPSILON && div > -EPSILON) {
		return 0;
	}

	return x / div;
}

inline vec2_t vec2(float x, float y)
{
	return (vec2_t){x, y};
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
inline vec3_t div3f(vec3_t a, float b)
{
	return div3(a, vec3f(b));
}
inline vec3_t div3safe(vec3_t a, vec3_t b)
{
	return (vec3_t){divsafe(a.x, b.x), divsafe(a.y, b.y), divsafe(a.z, b.z)};
}
inline vec3_t div3fsafe(vec3_t a, float b)
{
	return div3safe(a, vec3f(b));
}

float len3(vec3_t a) {
	return (float)_Sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
vec3_t normalize3(vec3_t v) {
	float len = len3(v);
	return vec3(v.x/len, v.y/len, v.z/len);
}
float dot3(vec3_t a, vec3_t b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
vec3_t cross3(vec3_t a, vec3_t b) {
	vec3_t result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return normalize3(result);
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
	uint32_t dSignBit = (1<<31) & *(uint32_t*)&f;
	// float dSignBit = (1<<31) & f;
	if (f < EPSILON && f > -EPSILON) {
		// f = EPSILON;
		// *(uint32_t*)&f &= dSignBit;
		return vec4f(0);
	}
	return (vec4_t){
		a.x/f,
		a.y/f,
		a.z/f,
		a.w/f
	};
}

inline vec4fixed32_t vec4tofixed32(vec4_t v)
{
	v = mul4f(v, 65536.0f);
	return (vec4fixed32_t){ v.x, v.y, v.z, v.w };
}

inline float floor(float x)
{
	int32_t result = x;
	if (x < 0.0f) {
		--result;
	}
	return (float)result;
}

inline float fract(float x)
{
	return x - floor(x);
}

inline fixed32_t floorfx32(fixed32_t x)
{
	uint32_t fracMask = 0xFFFF;
	if (x < 0 && (x & fracMask)) {
		x -= 0x10000;
	}
	return x & ~fracMask;
}


// TRIG
// inline uint32_t GetTableIndex()
// {

// }
static inline float SineFloat(float t)
{
	t *= __table_size / PI2;
	float f = fract(t);
	int32_t i = floor(t);
	uint32_t u = i;
	uint32_t idx0 = u & (__table_size-1);
	uint32_t idx1 = (u+1) & (__table_size-1);

	float value0 = __table_sine_float[idx0];
	float value1 = __table_sine_float[idx1];

	return value0 + (value1-value0)*f;
}

static inline fixed32_t SineFx(fixed32_t t)
{
	fixed32_t offset = (t) * (itofx32(__table_size) / PI2FX);
	// NOTE: This is not the correct way to get the fractional part
	// but for this case it is what we want
	fixed32_t f = offset & 0xFFFF;
	uint32_t u = floorfx32(offset) >> 16;
	uint32_t idx0 = u & (__table_size-1);
	uint32_t idx1 = (u+1) & (__table_size-1);

	fixed32_t value0 = __table_sine_fixed[idx0];
	fixed32_t value1 = __table_sine_fixed[idx1];

	return value0 + ((value1-value0)*f >> 16);
}

static inline float CosineFloat(float t)
{
	t *= __table_size / PI2;
	float f = fract(t);
	int32_t i = floor(t);
	uint32_t u = i;
	uint32_t idx0 = u & (__table_size-1);
	uint32_t idx1 = (u+1) & (__table_size-1);

	assert(idx0 < arraysize(__table_cosine_float));
	assert(idx1 < arraysize(__table_cosine_float));
	float value0 = __table_cosine_float[idx0];
	float value1 = __table_cosine_float[idx1];

	return value0 + (value1-value0)*f;
}

static inline fixed32_t CosineFx(fixed32_t t)
{
	fixed32_t offset = (t) * (itofx32(__table_size) / PI2FX);
	fixed32_t f = offset & 0xFFFF;
	uint32_t u = floorfx32(offset) >> 16;
	uint32_t idx0 = u & (__table_size-1);
	uint32_t idx1 = (u+1) & (__table_size-1);

	fixed32_t value0 = __table_cosine_fixed[idx0];
	fixed32_t value1 = __table_cosine_fixed[idx1];

	return value0 + ((value1-value0)*f >> 16);
}

inline float TangentFloat(float t)
{
	// not sure if (__table_size-1) is important
	t = (t + PI/2) * ((float)(__table_size-1) / PI);
	assert(t >= 0.0f && t < __table_size);
	float f = fract(t);
	int32_t i = floor(t);
	uint32_t u = i;
	uint32_t idx0 = u & (__table_size-1);
	uint32_t idx1 = (u+1) & (__table_size-1);

	float value0 = __table_tangent_float[idx0];
	float value1 = __table_tangent_float[idx1];

	return value0 + (value1-value0)*f;
}


// MATRIX
typedef union {
	struct {
		float m00;
		float m01;
		float m02;
		float m03;
		float m10;
		float m11;
		float m12;
		float m13;
		float m20;
		float m21;
		float m22;
		float m23;
		float m30;
		float m31;
		float m32;
		float m33;
	};
	float f[16];
} mat4_t;

// mat4_t perspective_matrix(float fov, float aspect, float near, float far) {
// 	float f = 1.0f / TangentFloat((fov/180.0f*PI) / 2.0f);
// 	mat4_t mat = {
// 		f / aspect, 0, 0, 0,
// 		0, f, 0, 0,
// 		0, 0, (far + near) / (near - far), -1,
// 		0, 0, (2.0f * far * near) / (near - far), 0,
// 	};
// 	return mat;
// }

// vec4_t vec4_mul_mat4(vec4_t in, mat4_t mat) {
// 	vec4_t result = {0};
// 	for (int i = 0; i < 4; ++i) {
// 		for (int j = 0; j < 4; ++j) {
// 			result.f[i] += in.f[j] * mat.f[i*4 + j];
// 		}
// 	}
// 	return result;
// }

mat4_t PerspectiveMatrix(float fov, float aspect, float near, float far)
{
	float a = aspect;
	float s = 1.0f / TangentFloat((fov/180.0f*PI) / 2.0f);
	float n = near;
	float f = far;

	// (2*f*n)/(n-f) scales Z
	// -1 sets W to -Z
	mat4_t mat = {
		s/a, 0,  0,             0,
		0,   s,  0,             0,
		0,   0, (f+n)/(n-f),   -1,
		0,   0, (2*f*n)/(n-f),  0,
	};
	return mat;
}

vec4_t Vec4MulMat4(vec4_t in, mat4_t mat)
{
	vec4_t result = {0};
	for (int v = 0; v < 4; ++v) {
		for (int i = 0; i < 4; ++i) {
			result.f[v] += in.f[i] * mat.f[i*4+v];
		}
	}
	return result;
}

vec4_t Mat4MulVec4(mat4_t mat, vec4_t in)
{
	vec4_t result = {0};
	for (int v = 0; v < 4; ++v) {
		for (int i = 0; i < 4; ++i) {
			result.f[v] += mat.f[v*4+i] * in.f[i];
		}
	}
	return result;
}

mat4_t Mat4RotationZ(float rads)
{
	float s = SineFloat(rads);
	float c = CosineFloat(rads);
	mat4_t result = {
		c, s, 0, 0,
		-s, c,  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	return result;
}


#endif
