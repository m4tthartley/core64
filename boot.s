##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

#define sp $29

.section .text._start
.global _start

_start:
	la $29, _stack_top # setup the stack
	jal main
	nop

_loop:
	j _loop
	nop

.section .bss
.space 0x1000
_stack_top:
