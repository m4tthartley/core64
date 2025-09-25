##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#include "n64def.h"

#include "exception.s"

	.set noreorder
	.global _start
	.extern _stack_top

	.section .boot

	.extern __bss_start
	.extern __bss_end
_start:
	la $gp, _gp

	# ZERO BSS
	la $a0, __bss_start
	la $a1, __bss_end
ZeroLoop:
	beq $a0, $a1, ZeroLoopDone
	sw $zero, 0($a0)
	addiu $a0, $a0, 4
	j ZeroLoop
	nop
ZeroLoopDone:

	# NOTE: This is apparently not needed for the way my memory layout is setup
	# 		all code and data starts at 0x80000400
	# 		which gets mapped into RAM by the bootloader
	# COPY .data
	# la $t0, __data_rom_start
	# la $t1, __data_start
	# la $t2, __data_end
	# CopyData:
	# beq $t1, $t2, CopyDataDone
	# lw $t3, 0($t0)
	# sw $t3, 0($t1)
	# addiu $t0, $t0, 4
	# addiu $t1, $t1, 4
	# j CopyData
	# nop
	# CopyDataDone:

	# Setup status register with some defaults and enable interrupts and RCP interrupts
	li $2, SR_CU1 | SR_PE | SR_FR | SR_KX | SR_SX | SR_UX | SR_IE | C0_INTERRUPT_RCP
	mtc0 $2, C0_SR

	# Get bootcode flags
	lui $v0, 0xA400

	la $v1, __memorySize
	lw $t0, 0($v0)
	sw $t0, 0($v1)

	la $v1, __tvType
	lbu $t0, 9($v0)
	sw $t0, 0($v1)

# setup the stack pointer
	la $sp, _stack_top
# call main
	jal main
	nop

_loop:
	j _loop
	nop


_deliberate_error:
	.globl _deliberate_error
	# syscall

	# break

	# li $t0, 5
	# li $t1, 5
	# teq $t0, $t1

	# li $t0, 0x7FFFFFFF
	# li $t1, 1
	# add $t2, $t0, $t1

	jr $ra
	nop


_GetClock:
	.globl _GetClock
	mfc0 $v0, C0_COUNT
	jr $ra
	nop


	.section .stack
	.space 0x10000
