##
##  Created by Matt Hartley on 09/09/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#include "registers.h"


.extern ExceptionHandler
.extern ResetVideoCurrentLine
.extern __EXCEPTIONFRAME_SIZE
.extern __viInterruptCounter

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
	# move $k0, $sp

	# not sure if a separate exception stack is really needed
	# la $sp, __exception_stack_top
	# push stack to make room for exception frame structure
	la $k0, __EXCEPTIONFRAME_SIZE
	lw $k1, 0($k0)
	move $k0, $sp
	# addiu $sp, $sp, -$k1 # stack must be 8byte aligned
	sub $sp, $sp, $k1
	# move $k1, $sp

	# save registers
	sd $at, __EF_AT($sp)
	sd $v0, __EF_V0($sp)
	sd $v1, __EF_V1($sp)
	sd $a0, __EF_A0($sp)
	sd $a1, __EF_A1($sp)
	sd $a2, __EF_A2($sp)
	sd $a3, __EF_A3($sp)
	sd $t0, __EF_T0($sp)
	sd $t1, __EF_T1($sp)
	sd $t2, __EF_T2($sp)
	sd $t3, __EF_T3($sp)
	sd $t4, __EF_T4($sp)
	sd $t5, __EF_T5($sp)
	sd $t6, __EF_T6($sp)
	sd $t7, __EF_T7($sp)
	sd $t8, __EF_T8($sp)
	sd $t9, __EF_T9($sp)
	sd $k0, __EF_SP($sp)
	sd $ra, __EF_RA($sp)

	# read in status, cause and epc registers from cop0
#define status $t4
#define cause $t5
#define epc $t6
#define badaddr $t7
	mfc0 $t4, C0_SR
	mfc0 $t5, C0_CAUSE
	mfc0 $t6, C0_EPC
	mfc0 $t7, C0_BADVADDR

	# store info in exception frame structure
	sw $t4, __EF_STATUS($sp)
	sw $t5, __EF_CAUSE($sp)
	sw $t6, __EF_EPC($sp)
	sw $t7, __EF_BADADDR($sp)
	# sw $k0, 16($sp)

	# disable interrupts, set exl=0 and force kernel mode
	lw $t0, __EF_STATUS($sp)
	and $t0, ~(SR_IE | SR_EXL | SR_KSU)
	mtc0 $t0, C0_SR
	nop

	# check if this is an interrupt
	andi $t0, $t5, 0xFF
	beqz $t0, interrupt
	nop
	
	move $a0, $sp
	jal ExceptionHandler
	nop

infloop:
	j infloop
	nop

interrupt:
	la $t0, __viInterruptCounter
	lw $t1, 0($t0)
	addiu $t1, $t1, 1
	sw $t1, 0($t0)

	li $t1, VI_FRAMEBUFFERBASE
	li $t2, (31 << 11)
	sw $t2, (1024*3)($t1)

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
	# lw $sp, __EF_SP($sp)
	lw $t0, __EF_STATUS($sp)
	lw $t1, __EF_EPC($sp)
	mtc0 $t0, C0_SR
	# NOTE: This must have at least 4 hazard cycles between it and the eret
	mtc0 $t1, C0_EPC
	nop

	# restore registers
	ld $at, __EF_AT($sp)
	ld $v0, __EF_V0($sp)
	ld $v1, __EF_V1($sp)
	ld $a0, __EF_A0($sp)
	ld $a1, __EF_A1($sp)
	ld $a2, __EF_A2($sp)
	ld $a3, __EF_A3($sp)
	ld $t0, __EF_T0($sp)
	ld $t1, __EF_T1($sp)
	ld $t2, __EF_T2($sp)
	ld $t3, __EF_T3($sp)
	ld $t4, __EF_T4($sp)
	ld $t5, __EF_T5($sp)
	ld $t6, __EF_T6($sp)
	ld $t7, __EF_T7($sp)
	ld $t8, __EF_T8($sp)
	ld $t9, __EF_T9($sp)
	ld $ra, __EF_RA($sp)
	ld $sp, __EF_SP($sp)

	# restore stack pointer return
	eret
