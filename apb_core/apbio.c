/*********************************************************************
*
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : BSP_APB_rev2.c
Purpose : BSP interface for University of Idaho APB V2 board
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "apbio.h"
#include "pin_ops.h"
#include "apbconfig.h"
#include "apbfsm.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MII 1

/* Port 0 indices */
#define APB_REMBUTT_INDEX	17
#define APB_RESETOUT_INDEX	18
#define APB_BUTTON_INDEX	19
#define APB_VIB_INDEX		20
#define APB_LED_INDEX		21
#define APB_MUTE_INDEX		22

/* Port 2 indicies */
#define APB_LED0_INDEX		0
#define APB_LED1_INDEX		1
#define APB_LED2_INDEX		2
#define APB_LED3_INDEX		3
#define APB_LED4_INDEX		4
#define APB_LED5_INDEX		5
#define APB_LED6_INDEX		6
#define APB_LED7_INDEX		7

#define APB_BUTTON_MASK		( 0x1 << APB_BUTTON_INDEX )

#define TIMER2_PRESCALE 11999

///TODO: Replace with button stuff static void timer2_init( void );
//static void button_isr( void );
//static void timer2_isr( void );
//static void button_init( void );

//Board Support Package
void BSP_Init( void ) {

	/* Set MUTE high */
//TODO	FIOSET0 = FIOSET_SET( APB_MUTE_INDEX );

	/* Set RESETOUT high */
//TODO	FIOSET0 = FIOSET_SET( APB_RESETOUT_INDEX );

	/* LED high (off ) */
//TODO	FIOSET0 = FIOSET_SET( APB_LED_INDEX );

//TODO	button_init();
}

void CommReset( void ){
//	FIO0CLR2 = NOTRESET;
//	OS_Delay( 100 );
//	FIO0SET2 = ( FIO0PIN2 | NOTRESET );
//	OS_Delay( 500 );	
}


static void button_init( void ){

	// Enable falling edge interrupt
//	IO0IntEnF |= APB_BUTTON_MASK;

	/* Clear out any old interrupts */
//	IO0IntClr = APB_BUTTON_MASK;

	// Initialize button press timer
//	timer2_init();

//	OS_ARM_InstallISRHandler( VIC_INDEX_EINT3 , &button_isr );
//	OS_ARM_ISRSetPrio( VIC_INDEX_EINT3 , INT_EINT3_PRIO );
//	OS_ARM_EnableISR( VIC_INDEX_EINT3 );

}

void disablebutton( void ){

   // Disable the interrupt.
//   IO0IntEnF &= ~APB_BUTTON_MASK;

   // Clear any pending interruupts.
  // IO0IntClr |= APB_BUTTON_MASK;
}

void enablebutton( void ){

   // Clear any pending interruupts.
 //  IO0IntClr |= APB_BUTTON_MASK;

   // Enable the interrupt.
 //  IO0IntEnF |= APB_BUTTON_MASK;
}

static void button_isr( void ){
/*	OS_EnterInterrupt();

	if( IO0IntEnStatF & APB_BUTTON_MASK ){
		// Falling edge of button

		// Enable rising edge interrupt
		IO0IntEnR |= APB_BUTTON_MASK;

		// Reset timer2 for interrupt
		// Clear timer2 count
		timer_reset( TIMER2 );
		// Enable timer2
		timer_enable( TIMER2 );

	}
	else if( IO0IntEnStatR & APB_BUTTON_MASK ){
		// Rising edge of button
		
		// Disable rising edge interrupt
		IO0IntEnR &= ~APB_BUTTON_MASK;

		// Stop timer2
		// Disable timer2
		timer_disable( TIMER2 );

		// Clear timer2 interupt
   	TIMER_IR( TIMER2 ) = TIMER_IR_MR0;
		
		// Signal a button press
      ButtonState = B_REGPRESS;

	}
	else {
		// Pin changed that we don't care about
	}

   // Clear interrupt
	IO0IntClr |= APB_BUTTON_MASK;

	OS_LeaveInterrupt();
*/
}
static void timer2_init( void ){
/*	// Protect code from being interrupted
   OS_IncDI();

   // Power up timer2
   PCONP |= PCONP_TIMER2;

   // Configure timer2
   // Disable timer2
	timer_disable( TIMER2 );

   // Clear interrupt
   TIMER_IR( TIMER2 ) = TIMER_IR_MR0;
   // Load prescaler
   // F_timer2 = PCLK / ( PR + 1 )
   // shooting for about 1ms prescale
   TIMER_PR( TIMER2 ) = TIMER2_PRESCALE;
   // Match register for extended press time
   TIMER_MR0( TIMER2 ) = StationConfig.EXP_Time;
   // Reset after match, fire interrupt
   TIMER_MCR( TIMER2 ) = ( TIMER_MCR_MR0I | TIMER_MCR_MR0R );
   // Clear counter
   timer_reset( TIMER2 );

	OS_ARM_InstallISRHandler( VIC_INDEX_TIMER2 , &timer2_isr );
	OS_ARM_ISRSetPrio( VIC_INDEX_TIMER2 , INT_TIMER2_PRIO );
	OS_ARM_EnableISR( VIC_INDEX_TIMER2 );

   OS_DecRI();
*/
}

void apbio_reloadexptime( void ){
//   TIMER_MR0( TIMER2 ) = StationConfig.EXP_Time;
}
static void timer2_isr( void ){
 //  OS_EnterInterrupt();

   // Clear interrupt
 //  TIMER_IR( TIMER2 ) = TIMER_IR_MR0;
   // Diasble timer2
//   timer_disable( TIMER2 );
   // Disable risinge edge button interrupt
 //  IO0IntEnR &= ~APB_BUTTON_MASK;

	// Signal that APS call was placed
  // ButtonState = B_LONGPRESS;

  // OS_LeaveInterrupt();
}
