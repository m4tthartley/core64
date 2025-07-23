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
		#KEEP(*(.text._start))
		#KEEP(*(.text.entry))
		#KEEP(*(.text))

		_text_start = .;
		*(.boot)
		. = ALIGN(16);
		*(.text)
		*(.text*)
		_text_end = .;
	} :main

	. = ALIGN(16);
	. = . + 0x100;

	#.rodata : {
	#	*(.rodata*)
	#}

	. = ALIGN(16);
	__data_rom_start = LOADADDR(.data);

	.data ALIGN(16) : {
		__data_start = .;
		*(.data*)
		. = ALIGN(8);
		__data_end = .;
	}

	. = ALIGN(16);

	.bss (NOLOAD) : {
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(COMMON)
		PROVIDE(__bss_end = .);
	}

	. = ALIGN(16);
	. = . + 0x400;
	#. = ALIGN(0x100);

	.stack : {
		_stack_bottom = .;
		*(.stack*)
		_stack_top = .;
	}

	/DISCARD/ : {
		*(.MIPS.abiflags)
		*(.reginfo)
		*(.gnu.*)
		*(.comment)
		*(.mdebug*)
	}
}

PHDRS {
	#text PT_LOAD FLAGS(0x5);
	#data PT_LOAD FLAGS(0x6);
	main PT_LOAD AT ( 0x80000400 );
}
