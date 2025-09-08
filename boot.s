##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#define sp $29

##	# clear bss
##	#addiu t0, t0, %lo(__bss_start)
##	la $t0, __bss_start
##	la $t1, __bss_end
##ClearBSS:
##	beq $t0, $t1, BSSDone
##	sw 0, 0(t0)
##BSSDone:

#define gp $28
#define sp $29

.set noreorder
.global _start
.extern _stack_top

.section .boot

.extern __bss_start
.extern __bss_end
_start:
	la $gp, _gp

## ZERO BSS
	la $a0, __bss_start
	la $a1, __bss_end
ZeroLoop:
	beq $a0, $a1, ZeroLoopDone
	sw $zero, 0($a0)
	addiu $a0, $a0, 4
	j ZeroLoop
	nop
ZeroLoopDone:

## COPY .data
	la $t0, __data_rom_start
	la $t1, __data_start
	la $t2, __data_end
CopyData:
	beq $t1, $t2, CopyDataDone
	lw $t3, 0($t0)
	sw $t3, 0($t1)
	addiu $t0, $t0, 4
	addiu $t1, $t1, 4
	j CopyData
	nop
CopyDataDone:

# setup the stack
	la $29, _stack_top
# call main
	jal main
	nop

_loop:
	j _loop
	nop

.section .stack
.space 0x10000
