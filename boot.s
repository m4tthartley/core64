##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#define sp $29

.set noreorder
.global _start
.global _stack_top

.section .boot

_start:
	la $29, _stack_top # setup the stack
	jal main
	nop

_loop:
	j _loop
	nop

.section .stack
.space 0x4000
_stack_top:
