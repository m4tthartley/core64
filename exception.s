##
##  Created by Matt Hartley on 09/09/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#include "registers.h"


.extern ExceptionHandler
.extern ResetVideoCurrentLine

	.section .bss
	.align 8
__exception_stack:
	.space 1024
__exception_stack_top:

	.section .exception_vectors, "ax"

	.org 0x0
__vector_tlb_refill:
	j interupt_handler
	nop

	.org 0x80
__vector_something:
	j interupt_handler
	nop

	.org 0x100
__vector_cache_error:
	j interupt_handler
	nop

	.org 0x180
__vector_general:
	j interupt_handler
	nop
	

	.section .text

	.p2align 5
interupt_handler:
	.set noreorder
	# .set noat

	# save original stack pointer
	move $k0, $sp

	# not sure if a separate exception stack is really needed
	# la $sp, __exception_stack_top
	# push stack to make room for exception frame structure
	addiu $sp, $sp, -24 # stack must be 8byte aligned
	# move $k1, $sp

	# read in status, cause and epc registers from cop0
#define status $t4
#define cause $t5
#define epc $t6
#define badaddr $t7
	mfc0 status, C0_SR
	mfc0 cause, C0_CAUSE
	mfc0 epc, C0_EPC
	mfc0 badaddr, C0_BADVADDR

	# store info in exception frame structure
	sw status, 0($sp)
	sw cause, 4($sp)
	sw epc, 8($sp)
	sw badaddr, 12($sp)
	sw $k0, 16($sp)

	# disable interrupts, set exl=0 and force kernel mode
	move $t0, status
	and $t0, ~(SR_IE | SR_EXL | SR_KSU)
	mtc0 $t0, C0_SR
	nop

	# check if this is an interrupt
	andi $t2, cause, 0xFF
	beqz $t2, interrupt
	nop
	
	move $a0, $sp
	jal ExceptionHandler
	nop

infloop:
	j infloop
	nop

interrupt:
	li $t1, VI_FRAMEBUFFERBASE
	li $t2, (31 << 11)
	sw $t2, (1024*2)($t1)

	# clear Vi interrupt
	# I read that you need to do this but I'm not sure
	li $t2, 0xA4300000
	li $t3, 0x08
	sw $t3, 0x08($t2)

	# lui $t2, 0xA440
	# lw $t3, 0x10($t2)
	# sw $t3, 0x10($t2)
	jal ResetVideoCurrentLine
	nop

	# restore status register
	lw $t0, 0($sp)
	lw $t1, 8($sp)
	move $sp, $k0
	mtc0 $t0, C0_SR
	mtc0 $t1, C0_EPC
	nop

	# restore stack pointer return
	eret
