##
##  Created by Matt Hartley on 09/09/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

.extern ExceptionHandler

	.section .bss
	.align 8
__exception_stack:
	.space 1024
__exception_stack_top:

	.section .exception_vectors, "ax"
	.align 4

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
	# .balign 0x180
__vector_general:
	j interupt_handler
	# lui $k0, %hi(ExceptionHandler)
	# ori $k0, $k0, %lo(ExceptionHandler)
	# jr $k0
	# nop

	# jal ExceptionHandler
	# nop
	# j __vector_general
	# nop

.section .text

	.p2align 5
interupt_handler:
	# lui   $t0, 0x0440       # VI_BASE = 0x04400000 mapped at 0xA4400000 (KSEG1)
    # ori   $t0, $t0, 0x0000  # Adjust if needed
    # li    $t1, 0x00000000   # Some color value
    # sw    $t1, 0($t0)       # Write to VI_CONTROL or background color

	move $k0, $sp
	# la $sp, __exception_stack_top

	addiu $sp, -4
	move $k1, $sp

	move $a0, $k1
	jal ExceptionHandler
	nop

	# lui $t0, 0xA020
	# addiu $t0, 1024*2
	# li $t1, 31<<6
	# sw $t1, 0($t0)

1:  j     1b                 # Infinite loop
    nop
