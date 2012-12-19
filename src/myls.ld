

MEMORY
{
	FLASH (RX) : ORIGIN = 0x00000000, LENGTH = 512K
	SRAM0 (RWX) : ORIGIN = 0x10000000, LENGTH = 32K
	SRAM1 (RWX) : ORIGIN = 0x2007C000, LENGTH = 32K
}
ENTRY(ResetISR)
/*
Sections:
	.text
	.data
	.bss
	.isr_vector
	.rodata
	.debug_info
	.debug_abbrev
	.debug_loc
	.debug_aranges
	.debug_line
	.debug_str
	.comment
	.ARM.attributes
	.debug_frame


*/

SECTIONS
{
	.text :
	{
		

	} >FLASH


}
