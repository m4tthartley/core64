/*
	Created by Matt Hartley on 29/09/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#define CORE_IMPL
#include <core/core.h>
#include <core/bmp.h>

int main()
{
	char* bmpFilename = "./resources/tiles2.bmp";
	print("Packing texture (%s) \n", bmpFilename);

	file_t file = sys_open(bmpFilename);
	stat_t info = sys_stat(file);
	void* bmpData = malloc(info.size);
	// sys_copy_memory(bmpData, void *src, size_t size)
	sys_read(file, 0, bmpData, info.size);
	sys_close(file);

	bmp_info_t bmpInfo = bmp_get_info(bmpData);
	print("bmp %ux%ux%ubits \n", bmpInfo.width, bmpInfo.height, bmpInfo.format);
	uint32_t size = bmpInfo.width*bmpInfo.height*sizeof(uint32_t);//bmp_get_size(bmpData);
	void* outputMemory = malloc(size);
	bmp_load_rgba32(bmpData, outputMemory);

	int numPixels = bmpInfo.width*bmpInfo.height;

	uint32_t* src = outputMemory;
	uint16_t* tex = malloc(bmpInfo.width*bmpInfo.height*sizeof(uint16_t));
	for (int pixel=0; pixel<bmpInfo.width*bmpInfo.height; ++pixel) {
		// uint16_t r = (src[pixel]>>24) & 31;
		// uint16_t g = (src[pixel]>>16) & 31;
		// uint16_t b = (src[pixel]>>8) & 31;
		uint16_t r = (((src[pixel]>>16) & 0xFF) / 8) & 31;
		uint16_t g = (((src[pixel]>>8) & 0xFF) / 8) & 31;
		uint16_t b = (((src[pixel]>>0) & 0xFF) / 8) & 31;
		tex[pixel] = (r<<11) | (g<<6) | (b<<1) | 1;
		// tex[pixel] =  (31<<1) ;
	}

	// Swap bytes to big endian
	for (int pi=0; pi<numPixels; ++pi) {
		uint8_t* bytes = (uint8_t*)(tex + pi);
		swap(bytes[0], bytes[1]);
		// uint8_t t = bytes[0];
		// bytes[0]
		tex[pi] = *((uint16_t*)bytes);
	}

	file_t outputFile = sys_create("./resources/test_texture.bin");
	sys_write(outputFile, 0, tex, bmpInfo.width*bmpInfo.height*sizeof(uint16_t));
	sys_close(outputFile);

	// output file
}
