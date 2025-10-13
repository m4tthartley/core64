/*
	Created by Matt Hartley on 03/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__

#include <stdint.h>
#include <stddef.h>

#include "interrupts.h"


unsigned long strlen(const char* str);
void* memset(void* dest, int value, size_t size);
void* CopyMemory(uint8_t* dest, const uint8_t* src, size_t num);

void DrawFontStringWithBG(uint32_t* font, char* str, int x, int y);
#define _assert_ex(exp, func, file, line) if (!(exp)) { AssertionScreen(func, file, line, #exp); /*DrawFontStringWithBG(N64Font, "AssertionFired", 320/2-7, 10); (*(volatile int*)0)=0;*/ }
// #define assert1(exp) assert2(exp)
#define assert(exp) _assert_ex(exp, __func__, __FILE__, __LINE__)
#define swap(a, b) { typeof(a) __tmp=a; a=b; b=__tmp; }
// #define swap(a, b) a=b-a; b=b-a; a=b+a;

#define max(a, b) (a>b ? a : b)

#define arraysize(a) (sizeof(a)/sizeof(a[0]))


#endif
