##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

EXTERN(_start)
ENTRY(_start)

SECTIONS {
	# n64 code starts after exception vectors

	. = 0x80000400;

	.text : {
		_text_start = .;
		KEEP(*(.text._start))
		KEEP(*(.text.entry))
		KEEP(*(.text))
		*(.text*)
		_text_end = .;
	} :main

	#. = 0x80001000;

	.rodata : {
		*(.rodata*)
	}

	.data : { *(.data*) }

	.bss : {
		*(.bss*)
		*(COMMON)
	}

	/DISCARD/ : { *(MIPS.abiflags) *(.pdr) }
}

PHDRS {
	#text PT_LOAD FLAGS(0x5);
	#data PT_LOAD FLAGS(0x6);
	main PT_LOAD AT ( 0x80000400 );
}
