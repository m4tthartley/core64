/*
	Created by Matt Hartley on 04/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __VIDEO_HEADER__
#define __VIDEO_HEADER__

#include "util.h"


typedef struct {
	int16_t x, y;
	int16_t width, height;
} video_rect_t;


video_rect_t GetResolution();


#endif
