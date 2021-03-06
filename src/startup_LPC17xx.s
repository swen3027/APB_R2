/*
 *  startup_LPC17xx.s
 *
 *  This is a generic LPC17xx startup script, suitable for use with
 *  the CodeSourcery Lite gcc toolset.
 *
 *  This code is based on several examples I found on the web, mashed
 *  together to do what I want it to do.  My thanks to the various
 *  authors for their contributions.
 */

	.syntax unified
	.thumb

	.section ".interrupt_vector_table"
	.global __interrupt_vector_table
/*
 *  This interrupt vector table gets pulled in by the linker script
 *  into flash right before the startup code
 *
 *  The following set of vectors are required by all ARM devices.
 */
__interrupt_vector_table:
	.long _end_stack			/* marks the top of stack */
	.long Reset_Handler
	.long NMI_Handler
	.long HardFault_Handler
	.long MemManage_Handler
	.long BusFault_Handler
	.long UsageFault_Handler
	.long 0
	.long 0
	.long 0
	.long 0
/*	.long SVC_Handler*/ .long vPortSVCHandler 
	.long DebugMon_Handler
	.long 0
/*	.long PendSV_Handler*/ .long xPortPendSVHandler 
/*	.long SysTick_Handler*/  .long xPortSysTickHandler 

/*
 *  The folloiwng set of vectors are specific to NXP's LPC devices.
 *  If you use this startup file for other ARM devices, modify this
 *  table accordingly.
 */
	.long WDT_IRQHandler
	.long TIMER0_IRQHandler
	.long TIMER1_IRQHandler
	.long TIMER2_IRQHandler
	.long TIMER3_IRQHandler
	.long UART0_IRQHandler
	.long UART1_IRQHandler
	.long UART2_IRQHandler
	.long UART3_IRQHandler
	.long PWM1_IRQHandler
	.long I2C0_IRQHandler
	.long I2C1_IRQHandler
	.long I2C2_IRQHandler
	.long SPI_IRQHandler
	.long SSP0_IRQHandler
	.long SSP1_IRQHandler
	.long PLL0_IRQHandler
	.long RTC_IRQHandler
	.long EINT0_IRQHandler
	.long EINT1_IRQHandler
	.long EINT2_IRQHandler
	.long EINT3_IRQHandler
	.long ADC_IRQHandler
	.long BOD_IRQHandler
	.long USB_IRQHandler
	.long CAN_IRQHandler
	.long DMA_IRQHandler
	.long I2S_IRQHandler
	.long ENET_IRQHandler
	.long RIT_IRQHandler
	.long MCPWM_IRQHandler
	.long QEI_IRQHandler
	.long PLL1_IRQHandler


/*
 *  The following stub routines serve as default handlers for the
 *  above vectors (except for the reset handler, of course).
 *
 *  You can recode this to have all dummy handlers use the same
 *  function, if you choose.  However, if you have a debugging
 *  tool, you should use a separate function for each default handler
 *  to make it easier to determine what caused any stray interrupt.
 */

/*
 *  Stub routines for the main vectors.
 */
NMI_Handler:
	b	.
	    
HardFault_Handler:
	b	.
	    
MemManage_Handler:
	b	.
	    
BusFault_Handler:
	b	.
	    
UsageFault_Handler:
	b	.
	    
SVC_Handler:
	b	.
	    
DebugMon_Handler:
	b	.
	    
PendSV_Handler:
	b	.
	    
SysTick_Handler:
	b	.
	    


/*
 *  Stub routines for the external vectors.
 */

WDT_IRQHandler:           
	b	.
	    
TIMER0_IRQHandler:
	b	.
	    
TIMER1_IRQHandler:
	b	.
	    
TIMER2_IRQHandler:
	b	.
	    
TIMER3_IRQHandler:
	b	.
	    
UART0_IRQHandler:
	b	.
	    
UART1_IRQHandler:
	b	.
	    
UART2_IRQHandler:
	b	.
	    
UART3_IRQHandler:
	b	.
	    
PWM1_IRQHandler:
	b	.
	    
I2C0_IRQHandler:
	b	.
	    
I2C1_IRQHandler:
	b	.
	    
I2C2_IRQHandler:
	b	.
	    
SPI_IRQHandler:
	b	.
	    
SSP0_IRQHandler:
	b	.
	    
SSP1_IRQHandler:
	b	.
	    
PLL0_IRQHandler:
	b	.
	    
RTC_IRQHandler:
	b	.
	    
EINT0_IRQHandler:
	b	.
	    
EINT1_IRQHandler:
	b	.
	    
EINT2_IRQHandler:
	b	.
	    
EINT3_IRQHandler:
	b	.
	    
ADC_IRQHandler:
	b	.
	    
BOD_IRQHandler:
	b	.
	    
USB_IRQHandler:
	b	.
	    
CAN_IRQHandler:
	b	.
	    
DMA_IRQHandler:
	b	.
	    
I2S_IRQHandler:
	b	.
	    
ENET_IRQHandler:
	b	.
	    
RIT_IRQHandler:
	b	.
	    
MCPWM_IRQHandler:
	b	.
	    
QEI_IRQHandler:
	b	.
	    
PLL1_IRQHandler:
	b	.
	    
	.align



/*
 *  Now declare all interrupts handlers except the reset handler
 *  to be weak.  This allows you to create a replacement handler
 *  in C with the same name and install it in the vector table
 *  above at link time.
 */

	.weak	NMI_Handler
	.weak	HardFault_Handler
	.weak	MemManage_Handler
	.weak	BusFault_Handler
	.weak	UsageFault_Handler
	.weak	SVC_Handler
	.weak	DebugMon_Handler
	.weak	PendSV_Handler
	.weak	SysTick_Handler


	.weak	WDT_IRQHandler           
	.weak	TIMER0_IRQHandler         
	.weak	TIMER1_IRQHandler         
	.weak	TIMER2_IRQHandler         
	.weak	TIMER3_IRQHandler         
	.weak	UART0_IRQHandler          
	.weak	UART1_IRQHandler          
	.weak	UART2_IRQHandler          
	.weak	UART3_IRQHandler          
	.weak	PWM1_IRQHandler           
	.weak	I2C0_IRQHandler           
	.weak	I2C1_IRQHandler           
	.weak	I2C2_IRQHandler           
	.weak	SPI_IRQHandler            
	.weak	SSP0_IRQHandler           
	.weak	SSP1_IRQHandler           
	.weak	PLL0_IRQHandler           
	.weak	RTC_IRQHandler            
	.weak	EINT0_IRQHandler          
	.weak	EINT1_IRQHandler          
	.weak	EINT2_IRQHandler          
	.weak	EINT3_IRQHandler          
	.weak	ADC_IRQHandler            
	.weak	BOD_IRQHandler            
	.weak	USB_IRQHandler            
	.weak	CAN_IRQHandler            
	.weak	DMA_IRQHandler          
	.weak	I2S_IRQHandler            
	.weak	ENET_IRQHandler       
	.weak	RIT_IRQHandler          
	.weak	MCPWM_IRQHandler             
	.weak	QEI_IRQHandler            
	.weak	PLL1_IRQHandler           


/*
 *  Actual code.
 */
	.section ".startup","x",%progbits
	.thumb_func
	.global _startup
	.global Reset_Handler

Reset_Handler:
_startup:
/* 
 *  Copy data from flash initialization area to RAM
 *
 *  The three values seen here are supplied by the linker script

 */
    ldr   r0, =_start_data_flash	/* initial values, found in flash */
    ldr   r1, =_start_data			/* target locations in RAM to write */
    ldr   r2, =_data_size			/* number of bytes to write */

/*
 *  Perform the copy.
 *  Handle the special case where _data_size == 0
 */
    cmp   r2, #0
    beq   done_copy
copy:
    ldrb   r4, [r0], #1
    strb   r4, [r1], #1
    subs   r2, r2, #1
    bne    copy
done_copy:
/*
 *  Clear the BSS section
 */ 
	mov r0, #0
	ldr r1, = _start_bss
	ldr r2, = _end_bss
	cmp	r1, r2
	beq	_done_clear

	sub r2, #1
_clear:
	cmp r1, r2
	str r0, [r1, #0]
	add r1, #4
	blo _clear
_done_clear:





/*
 *  Removed the call to init() at the assembly level.  The top-level C function
 *  (usually main) is now responsible for performing the necessary low-level
 *  initialization, like setting up the PLL and configuring the peripheral
 *  clocks..
 */
	/* Jump to init() */
	/* ldr r0, =init */
	/* blx r0 */

	/* Jump to main() */
	ldr r0, =main
	bx r0

	b	.					/* just in case control ever leaves main()! */
	
.end
