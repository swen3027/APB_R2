#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timer3.h"
void initTimer3()
{
	LPC_SC->PCONP |= (1 << 23); //Power on Timer3
	LPC_SC->PCLKSEL1 |= (3 << 14); //Timer 3 initally clocked at 12 MHz
	LPC_TIM3->TCR = 0x02; //Reset and disable timer3 while making changes
	LPC_TIM3->PR = 1249; //Prescale register must get to this value before the actual timer counter increments.
	//Settings for a tick every 100 us, gives a range of 0-429497 seconds.
	return;

}
void startTimer3()
{
	LPC_TIM3->TCR = 0x01;

}
void stopTimer3()
{	
	LPC_TIM3->TCR = 0x00;
}
void resetTimer3()
{
	LPC_TIM3->TCR = 0x02;
}
uint32_t timer3Value()
{
	return LPC_TIM3->TC;
}

