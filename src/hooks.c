#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
uint32_t sw_timer_array[NUM_TIMERS]; //Num timers defined in FreeRTOSConfig.h

void vApplicationMallocFailedHook(void)
{
	MESSAGE("Malloc failed\n\r");
	while(1);	
}
void vApplicationStackOverflowHook( void)
{	
	MESSAGE("stack overflow\n\r");
	while(1);
}
void vApplicationIdleHook( void)
{
	
}
void vApplicationTickHook(void)
{
	static uint32_t int_timer = 0;
	int i;
	if(int_timer == configCPU_CLOCK_HZ/(configTICK_RATE_HZ*1000) )
	{
		//GLOBAL TIMER inc (1 inc per ms)
		i = 0;
		while(i < NUM_TIMERS)
		{
			if(sw_timer_array[i])
				sw_timer_array[i]--;
			
			i++;
		}
		int_timer = 0;

	}
	int_timer++;
		
}
