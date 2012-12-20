#include <stdlib.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "RDB1768.h"
#include "LPC17xx.h"
#include "uart0.h"
#include "tinysh.h"
#include "sd.h"
#include "ff.h"
//#include "diskio.h"
#include "apbio.h"
#include "apbconfig.h"
#include "audio.h"

#define mainCHECK_DELAY	( ( portTickType ) 5000 / portTICK_RATE_MS )
/* Task priorities. */
#define mainUIP_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainCMDPRO_TASK_PRIORITY			( tskIDLE_PRIORITY + 1)
/* The WEB server has a larger stack as it utilises stack hungry string
handling library calls. */
#define mainBASIC_WEB_STACK_SIZE            		( configMINIMAL_STACK_SIZE * 4 )
/* The message displayed by the WEB server when all tasks are executing
without an error being reported. */
#define mainPASS_STATUS_MESSAGE				"All tasks are executing without error."
#define _VOLUMES 1
uint32_t SystemCoreClock;
FATFS Fatfs[_VOLUMES];
DIR dir;
FIL File[2];
//BYTE Buff[4096] __attribute__ ((aligned (4)));
	char file_name[] = "apb_conf.txt";

/*
#include "pwm.h"
#include "ssp.h"
#include "uart0.h"
#include "pin_ops.h"
#include "timer.h"
#include "apbio.h"
#include "apbconfig.h"
#include "apbtypes.h"
#include "efs.h"
#include "ls.h"
#include "mkfs.h"
#include "tinysh.h"*/
uint32_t CCLK_FREQ;
volatile int ITM_RxBuffer;
static char *pcStatusMessage = mainPASS_STATUS_MESSAGE;
char *pcGetTaskStatusMessage( void );
xTimerHandle xTimers[NUM_TIMERS];
long lExpireCounters[NUM_TIMERS] = {0};
void vTimerCallback(xTimerHandle pxTimer)
{
		MESSAGE("In timer callback\n\r");
//		inv_led3();
		


}

/*START FUNCTION DESCRIPTION
**_init				<main.c>
**SYNTAX:			_init(void)
**KEYWORDS:			initialization routine startup configuration
**DESCRIPTION:			Called from startup_LPC17xx.s before main() is called.
				Sets up CPU Clock, PLL, Direction and any other
				peripherials
**RETURN VALUE:			void
**NOTES:			Executed before main(). 
*/
extern uint32_t  sw_timer_array[NUM_TIMERS]; //Defined in src/hooks.c

void init_sw_timers()
{
	int i = 0;
	while(i < NUM_TIMERS)
		sw_timer_array[i++] = 0;	

}
void timerTestTask(void * pvParam)
{
	//initilize timer(1)
	while(1)
	{
		sw_timer_array[0] == 1000;
		if(sw_timer_array[0] == 0)
		{
			LPC_GPIO1->FIOPIN ^= LED3;
			sw_timer_array[0] = 1000;

		}
	}

}
void _init()
{
	char res;
	//Settings for 100MHz
	CCLK_FREQ = SystemInit(0x01, 74, 5, 2);
	SystemCoreClock = CCLK_FREQ;
	LPC_GPIO1->FIODIR |= LED2|LED3|LED4|LED5;	
	LPC_GPIO1->FIODIR |= OP1|OP2|OP4;
	LPC_GPIO0->FIODIR &= ~(1 << 0);
	LPC_PINCON->PINSEL3 |= (1 << 22) ;  // CLKOUT pin selection	
	UART0_Init(115200);
	MESSAGE("Initalizing timers\n\r");
	init_sw_timers();
	MESSAGE("Timers initaliezed\n\r");

}
typedef struct{
	uint32_t * BaseReg;
	uint32_t Pin;
} IO_t;
void APB_SetIO(IO_t io_obj)
{
	uint32_t * reg;
	reg = (uint32_t *) (io_obj.BaseReg + 6);
	*reg = io_obj.Pin;
}
void APB_ClrIO(IO_t io_obj)
{
	uint32_t * reg;
	reg = (uint32_t *) (io_obj.BaseReg + 7);
	*reg = io_obj.Pin;	
}
void APB_ToggleIO(IO_t io_obj)
{
	uint32_t * reg;
	reg = (uint32_t *) (io_obj.BaseReg + 5);
	*reg ^= io_obj.Pin;
}
int main(void) 
{
	int retval = 0;
	apb_station_config_t sc;

	
	while(1); //Struct-type IO identification test

	retval = f_mount(0, &Fatfs[0]);
	MESSAGE("filesystem f_mount return value: %d\n\r", retval);
	//Create some timers
	apbconfig_load(&sc, "config.cfg");

	




//	xTaskCreate(timerTestTask, "temp", 100, NULL, 0, NULL);
	xTaskCreate(vAudioTask, "audio", 1400, NULL, 0, NULL);	
	vTaskStartScheduler();
	MESSAGE("Scheduler didn't start\n\r");
	for(;;);
}












char *pcGetTaskStatusMessage( void )
{
	/* Not bothered about a critical section here. */
	return pcStatusMessage;
}
void vConfigureTimerForRunTimeStats( void )
{
	const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;
	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */
	/* Power up and feed the timer. */
	LPC_SC->PCONP |= 0x02UL;
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3<<2))) | (0x01 << 2);
	/* Reset Timer 0 */
	LPC_TIM0->TCR = TCR_COUNT_RESET;
	/* Just count up. */
	LPC_TIM0->CTCR = CTCR_CTM_TIMER;
	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	LPC_TIM0->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;
	/* Start the counter. */
	LPC_TIM0->TCR = TCR_COUNT_ENABLE;

}
