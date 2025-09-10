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
	la gp, _gp

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

# NOTE: This is not needed for the way my memory layout is setup
# 		all code and data starts at 0x80000400
# 		which gets mapped into RAM by the bootloader
## COPY .data
# 	la $t0, __data_rom_start
# 	la $t1, __data_start
# 	la $t2, __data_end
# CopyData:
# 	beq $t1, $t2, CopyDataDone
# 	lw $t3, 0($t0)
# 	sw $t3, 0($t1)
# 	addiu $t0, $t0, 4
# 	addiu $t1, $t1, 4
# 	j CopyData
# 	nop
# CopyDataDone:

#define C0_SR	$12

#define SR_CU1 	0x20000000
#define SR_FR 	0x04000000
#define SR_BEV 	0x00400000
#define SR_PE 	0x00100000

#define SR_KX 	0x00000080
#define SR_SX 	0x00000040
#define SR_UX 	0x00000020

#define SR_KSU 	0x00000018
#define SR_ERL 	0x00000004
#define SR_EXL 	0x00000002
#define SR_IE 	0x00000001

# Enable Exceptions
	# mfc0 $t0, $12
	# li $t1, ~(1<<22)
	# and $t0, $t0, $t1
	# li $t1, ~0xFF00
	# and $t0, $t0, $t1
	# mtc0 $t0, $12
	# nop
	li $2, SR_CU1 | SR_PE | SR_FR | SR_KX | SR_SX | SR_UX
	mtc0 $2, C0_SR

# setup the stack
	la $29, _stack_top
# call main
	jal main
	nop
# 	lui   $t0, 0xA440        # VI_BASE in KSEG1
#     ori   $t0, $t0, 0x0014   # VI_BKCOL register
#     li    $t1, 0x00FF00FF    # bright magenta
#     sw    $t1, 0($t0)

# 1:  j     1b                 # Infinite loop
#     nop

_loop:
	j _loop
	nop

.section .stack
.space 0x10000
