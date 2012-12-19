OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_startup)


MEMORY
{
	flash (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
	sram (rwx)  : ORIGIN = 0x10000000, LENGTH =  32K
	ahb0        : ORIGIN = 0x2007C000, LENGTH =  16K
	ahb1        : ORIGIN = 0x20080000, LENGTH =  16K
}

/* Define the top our stack at the end of SRAM */
_end_stack = 0x10008000;

EXTERN(__interrupt_vector_table);

SECTIONS
{
	.text :
	{
		/* Insert the interrupt vector table first */
		__interrupt_vector_table = .;
		*(.interrupt_vector_table)
		/* Startup assembly */
		*(.startup)

		/* Rest of the code (C) */
		*(.text )
		*(.glue_7)
		*(.glue_7t)

/* Added following section for holding initializers for variables   */
/* found in RAM.
/*
  The _data_size value will be used in the startup code to step through
  the image of data in flash and copy it to RAM.
*/
		. = ALIGN(4);
		_start_data_flash = .;
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
		_end_data_flash = .;
	} >flash

  /*  From generic.ld, supplied by CodeSourcery  */
  /* .ARM.exidx is sorted, so has to go in its own output section.  */
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} >sram
	PROVIDE_HIDDEN (__exidx_end = .);

	
	.data : AT (_end_data_flash)
	{
		_start_data = .;
		*(.data)
		_end_data = .;
	} >sram
	. = ALIGN(4);
	_data_size = _end_data - _start_data;

	_start_bss = .;
	.bss :
	{
		*(.bss)
		*(COMMON)
	} >sram

	. = ALIGN(4);
	_end_bss = .;
	bss_size = _end_bss - _start_bss;

	. = ALIGN(256);

	/* Stack can grow down to here, right after data and bss sections in 
	 * SRAM */
	_start_stack = .;

	/* Linker wants .eh_frame section defined because of gcc 4.4.X bug,
	 * just discard it here. */
/*	/DISCARD/ :
	{
		*(.eh_*)
	}
	*/
	_end = .;
	PROVIDE(end = .);

	.ethram :
	{
	} > ahb0
}



