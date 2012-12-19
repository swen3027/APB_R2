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

#include <RTOS.h>
#include "apbio.h"
#include "pinsel.h"
#include "gpio.h"
#include "scs.h"
#include "apbconfig.h"
#include "timer.h"
#include "vic.h"
#include "pconp.h"
#include "tasks.h"
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

static void timer2_init( void );
static void button_isr( void );
static void timer2_isr( void );
static void button_init( void );

void BSP_Init( void ) {
	/* Set GPIO mode to use the fast GPIO registers */
	SCS |= SCS_GPIO_FAST;

	/* Set P2.0-7 as outputs for the LEDs*/
	FIODIR2 |= FIODIR_OUTPUT( APB_LED0_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED1_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED2_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED3_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED4_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED5_INDEX );
	FIODIR2 |= FIODIR_OUTPUT( APB_LED6_INDEX );

	/* Set up GPIO for the APB things. */
	FIODIR0 &= FIODIR_INPUT( APB_BUTTON_INDEX );

	FIODIR0 |= FIODIR_OUTPUT( APB_LED_INDEX );
	FIODIR0 |= FIODIR_OUTPUT( APB_VIB_INDEX );
	FIODIR0 |= FIODIR_OUTPUT( APB_MUTE_INDEX );
	FIODIR0 |= FIODIR_OUTPUT( APB_REMBUTT_INDEX );
	FIODIR0 |= FIODIR_OUTPUT( APB_RESETOUT_INDEX );

	/* Remove pullup/downs from output pins*/
	PINMODE1 &= PINMODE1_P0_17_CLEAR;
	PINMODE1 &= PINMODE1_P0_18_CLEAR;
	PINMODE1 &= PINMODE1_P0_20_CLEAR;
	PINMODE1 &= PINMODE1_P0_21_CLEAR;
	PINMODE1 &= PINMODE1_P0_22_CLEAR;

	PINMODE1 |= PINMODE1_P0_17_FLOAT;
	PINMODE1 |= PINMODE1_P0_18_FLOAT;
	PINMODE1 |= PINMODE1_P0_20_FLOAT;
	PINMODE1 |= PINMODE1_P0_21_FLOAT;
	PINMODE1 |= PINMODE1_P0_22_FLOAT;

	/* Set MUTE high */
	FIOSET0 = FIOSET_SET( APB_MUTE_INDEX );

	/* Set RESETOUT high */
	FIOSET0 = FIOSET_SET( APB_RESETOUT_INDEX );

	/* LED high (off ) */
	FIOSET0 = FIOSET_SET( APB_LED_INDEX );

	button_init();
}

void APB_SetIO(int Value) {
  FIO0SET2 =  ( Value);

}

void APB_ClrIO(int Value) {
  FIO0CLR2 = ( Value);

}
void APB_ToggleIO(int Index) {
  if ((FIO0PIN2) & (Index)) {
    APB_ClrIO(Index);
  } else {
    APB_SetIO(Index);
  }
}

void CommReset( void ){
	FIO0CLR2 = NOTRESET;
	OS_Delay( 100 );
	FIO0SET2 = ( FIO0PIN2 | NOTRESET );
	OS_Delay( 500 );	
}


static void button_init( void ){

	// Enable falling edge interrupt
	IO0IntEnF |= APB_BUTTON_MASK;

	/* Clear out any old interrupts */
	IO0IntClr = APB_BUTTON_MASK;

	// Initialize button press timer
	timer2_init();

	OS_ARM_InstallISRHandler( VIC_INDEX_EINT3 , &button_isr );
	OS_ARM_ISRSetPrio( VIC_INDEX_EINT3 , INT_EINT3_PRIO );
	OS_ARM_EnableISR( VIC_INDEX_EINT3 );

}

void disablebutton( void ){

   // Disable the interrupt.
   IO0IntEnF &= ~APB_BUTTON_MASK;

   // Clear any pending interruupts.
   IO0IntClr |= APB_BUTTON_MASK;
}

void enablebutton( void ){

   // Clear any pending interruupts.
   IO0IntClr |= APB_BUTTON_MASK;

   // Enable the interrupt.
   IO0IntEnF |= APB_BUTTON_MASK;
}

static void button_isr( void ){
	OS_EnterInterrupt();

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
}
static void timer2_init( void ){
	// Protect code from being interrupted
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
}

void apbio_reloadexptime( void ){
   TIMER_MR0( TIMER2 ) = StationConfig.EXP_Time;
}
static void timer2_isr( void ){
   OS_EnterInterrupt();

   // Clear interrupt
   TIMER_IR( TIMER2 ) = TIMER_IR_MR0;
   // Diasble timer2
   timer_disable( TIMER2 );
   // Disable risinge edge button interrupt
   IO0IntEnR &= ~APB_BUTTON_MASK;

	// Signal that APS call was placed
   ButtonState = B_LONGPRESS;

   OS_LeaveInterrupt();
}
