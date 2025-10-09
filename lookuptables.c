/*
	Created by Matt Hartley on 08/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#define CORE_IMPL
#include <core/core.h>
#include <core/sys.h>
#include <core/math.h>

// #include "n64/math.h"


allocator_t allocator;

void Write(char* str)
{
	print(str);
	int len = strsize(str);
	sys_copy_memory(push_memory(&allocator, len), str, len);
}

int main()
{
	allocator = virtual_bump_allocator(MB(1), MB(1));

	// NOTE: Must be power of 2
#define TABLE_SIZE 256

	float sine[TABLE_SIZE];
	float cosine[TABLE_SIZE];
	float tangent[TABLE_SIZE];

	for (int idx=0; idx<TABLE_SIZE; ++idx) {
		float t = (float)idx / ((float)TABLE_SIZE);
		sine[idx] = sinf(t * PI2);
		cosine[idx] = cosf(t * PI2);
		t = ((float)idx + 0.5f) / ((float)TABLE_SIZE);
		tangent[idx] = tanf((-PI/2) + t * PI);
	}
	
	Write("\n");
	Write("#ifndef __LOOKUP_TABLES_HEADER__\n");
	Write("#define __LOOKUP_TABLES_HEADER__\n\n");

	Write("#include <stdint.h>\n\n");

	char str[64];
	sprint(str, 64, "const int __table_size = %i;\n\n", TABLE_SIZE);
	Write(str);

	Write("float __table_sine_float[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			sprint(str, 16, "%ff,", sine[idx+i2]);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("float __table_cosine_float[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			sprint(str, 16, "%ff,", cosine[idx+i2]);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("float __table_tangent_float[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			sprint(str, 16, "%ff,", tangent[idx+i2]);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("int32_t __table_sine_fixed[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			int32_t x = sine[idx+i2] * (float)0x10000;
			sprint(str, 16, "%i,", x);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("int32_t __table_cosine_fixed[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			int32_t x = cosine[idx+i2] * (float)0x10000;
			sprint(str, 16, "%i,", x);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("int32_t __table_tangent_fixed[] = {\n");
	for (int idx=0; idx<TABLE_SIZE; idx+=8) {
		Write("\t");
		for (int i2=0; i2<8; ++i2) {
			char str[16];
			int32_t x = tangent[idx+i2] * (float)0x10000;
			sprint(str, 16, "%i,", x);
			Write(str);
		}
		Write("\n");
	}
	Write("};\n\n");

	Write("#endif\n");

	file_t outputFile = sys_create("./resources/tables.h");
	sys_write(outputFile, 0, allocator.address, allocator.stackptr);
	sys_truncate(outputFile, allocator.stackptr);
	sys_close(outputFile);
}