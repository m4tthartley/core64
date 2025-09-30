##
##  Created by Matt Hartley on 20/07/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

EXTERN(_start)
ENTRY(_start)

#MEMORY {
#	RDRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 4M
#}

# Loadable segments
PHDRS {
	excepthdr PT_LOAD AT ( 0x80000000 );
	main PT_LOAD AT ( 0x80000400 );
}

SECTIONS {
	# exception vectors in the first 0x400 of ram
	. = 0x80000000;
	.exception_vectors  : {
		. = ALIGN(32);
		KEEP(*(.exception_vectors))
	} :excepthdr

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

	.rodata : {
		*(.rdata)
		*(.rodata)
		*(.rodata*)
		KEEP(*(keep.rodata.*))
		*(.gnu.linkonce.r.*)
		. = ALIGN(8);
	} :main

	. = ALIGN(16);
	__data_rom_start = LOADADDR(.data);

	__data_start = .;
	.data ALIGN(16) : {
		*(.data*)
		. = ALIGN(8);
	} :main

	.sdata : {
		_gp = . + 0x8000;
		*(.sdata)
		*(.sdata.*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(8);
	} :main
	__data_end = .;

	. = ALIGN(16);

	.bss (NOLOAD) : {
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(COMMON)
		PROVIDE(__bss_end = .);
	} :main

	. = ALIGN(16);

	.stack (NOLOAD) : {
		_stack_bottom = .;
		*(.stack*)
		_stack_top = .;
	} :main

	. = ALIGN(16);
	__heap_start = .;
	. = 0x803FFFFF;
	__heap_end = .;
	#__heap_end = ORIGIN(RDRAM) + LENGTH(RDRAM);

	/DISCARD/ : {
		*(.MIPS.abiflags)
		*(.reginfo)
		*(.gnu.*)
		*(.comment)
		*(.mdebug*)
	}
}
