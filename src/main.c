#include <stdlib.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "queue.h"
#include "RDB1768.h"
#include "LPC17xx.h"
#include "uart0.h"
#include "tinysh.h"
#include "sd.h"
#include "ff.h"
#include "apbio.h"
#include "apbconfig.h"
#include "audio.h"
#include "pwm.h"
#include "filesystem.h"
#include "rtc.h"
#define _VOLUMES 1
uint32_t SystemCoreClock;
extern FATFS Fatfs[_VOLUMES];
DIR dir;
FIL File[2];
char file_name[] = "apb_conf.txt";

extern FIL fp;
extern uint32_t audio1[8], audio2[8];
extern xQueueHandle audioControlQueue;
extern xQueueHandle audioRXQueue;
extern xSemaphoreHandle audioBusy;
extern xSemaphoreHandle I2STX_Request;
extern xSemaphoreHandle I2SRX_Request;
uint32_t CCLK_FREQ;
volatile int ITM_RxBuffer;
xTimerHandle xTimers[NUM_TIMERS];
long lExpireCounters[NUM_TIMERS] = {0};
void vTimerCallback(xTimerHandle pxTimer)
{
		MESSAGE("In timer callback\n\r");
		


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
//	if(setRTC(0, 15, 14, 26, 2, 57, 2, 2013))
//		printf("RTC was set\n\r");
//	else
//		printf("RTC was not set\n\r");	
	printf("initalization routine completed, calling main\n\r");
}
void vAudioStim(void * pvParam)
{
	const char filename[] = "sin440.wav\0"; //This is actually 880 Hz :-P
	while(1)
	{
		xQueueSend(audioControlQueue, filename, portMAX_DELAY); 
		printf("Sent file to audio task\n\r");
	}
}
int main(void) 
{
	int retval = 0;
	RTC_time t1;		
	apb_station_config_t sc;
	audioControlQueue = xQueueCreate(3, 20); //Create a queue of length 1, 20 bytes wide
	if(audioControlQueue == NULL)
	{
		printf("Failed to create audio command queue\n\r");
	}
	else
	{
		printf("Succesfully created audio command queue\n\r");
	}
	audioRXQueue = xQueueCreate(4, 16);  //Create a queue of length 4, each 4 bytes wide (1 word)
	if(audioRXQueue == NULL)
	{	
		printf("Failed to create audio receive queue\n\r");
	}
	else
	{
		printf("Succesfully created audio receive queue\n\r");
	}
	audioBusy = xSemaphoreCreateMutex();
	if(audioBusy == NULL)
	{
		printf("Failed to create audioBusy Semaphore\n\r");
	}
	else
	{
		printf("Sucessfully created audioBusy Semaphore\n\r");
	}
	vSemaphoreCreateBinary(I2STX_Request);
	if(I2STX_Request == NULL)
	{
		printf("Failed to create I2STX_Request Semaphore\n\r");
	}
	else
	{
		printf("Sucessfully created T2STX_Request Semaphore\n\r");
	}
	vSemaphoreCreateBinary(I2SRX_Request);
	if( I2SRX_Request == NULL)
	{
		printf("Failed to create I2SRX_Request Semaphore\n\r");
	}	
	else
	{
		printf("Sucessfully created I2SRX_Request Semaphore\n\r");
	}
	LPC_GPIO1->FIODIR |= (1 << 21);
	LPC_GPIO1->FIOSET |= (1 << 21);
	vAudioInit();
	if(vInitFilesystem())
		printf("Filesystem failed to init\n\r");

	retval = f_mount(0, &Fatfs[0]);
	printf("filesystem f_mount return value: ");
	put_rc(retval);
	printf("\n\r");
	

//	apbconfig_load(&sc, "config.cfg");
//	t1 = readRTC();
//	printf("RTC: %u:%u:%u %u/%u/%u\n\r", t1.hours, t1.minutes, t1.seconds, t1.month, t1.DOM, t1.year);

	xTaskCreate(vAudioStim, "a2", 230, NULL,0,NULL);
	xTaskCreate(vAudioTask, "audio", 240, NULL, 0, NULL);	
//	xTaskCreate(vAudioRXTask, "rxau", 240, NULL, 0, NULL);
	vTaskStartScheduler();
	MESSAGE("Scheduler didn't start\n\r");
	for(;;);
}




