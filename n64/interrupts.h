/*
	Created by Matt Hartley on 03/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#ifndef __INTERRUPT_HEADER__
#define __INTERRUPT_HEADER__

#include <stdint.h>


typedef struct {
	struct {
		uint64_t zero;
		uint64_t at;
		uint64_t v0, v1;
		uint64_t a0, a1, a2, a3;
		uint64_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
		uint64_t s0, s1, s2, s3, s4, s5, s6, s7, s8;
		uint64_t k0, k1;
		uint64_t gp;
		uint64_t sp;
		uint64_t ra;
	};

	uint32_t status;
	uint32_t cause;
	uint32_t epc;
	uint32_t badaddr;

	// uint32_t sp;
} interruptframe_t;


char* GetExcCodeName(uint8_t code);
char* GetInterruptName(uint8_t code);
void ExceptionHandler(interruptframe_t* frame);
void UnhandledInterrupt(uint32_t type);
void HandleInterrupt_Reset();
void HandleInterrupt_Timer();
void HandleInterrupt_Cart();
void HandleInterrupt_MI();

void AssertionScreen(const char* function, const char* filename, int line, const char* expr);


#endif
