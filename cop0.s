##
##  Created by Matt Hartley on 12/09/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##


	.section .text
	.align 8

SetStatusRegisterBit:
	mfc0 $t0, $12
	li $t1, $a0
	mtc0 $2, C0_SR
