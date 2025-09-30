/*
	Created by Matt Hartley on 29/09/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#define CORE_IMPL
#include <core/core.h>
#include <core/bmp.h>

int main()
{
	char* bmpFilename = "./resources/test_texture.bmp";
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

	file_t outputFile = sys_create("./resources/test_texture.bin");
	sys_write(outputFile, 0, outputMemory, size);
	sys_close(outputFile);

	// output file
}
