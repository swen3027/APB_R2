#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "LPC17xx.h"
#include "pwm.h"

/*START FUNCTION DESCRIPTION
**PWM_init			<pwm.c>
**SYNTAX:			PWM_init(uint32_t, uint32_t, uint32_t, uint32_t)
**PARAMETER1:			PR1 prescaler value.  TC is incremented every PR+1 ticks
**PARAMETER2:			Cycle time; MR0 value.  Restarts when TC == MR0
**PARAMETER3:			Cycle High time: MR1 value.  Signal drops low when TC == MR1
**PARAMETER4:			Peripherial clock divider (1, 2, 4, 8).  PR increments every Pclk tick
**KEYWORDS:			PWM1 init
**DESCRIPTION:			initalizes PWM1, channel 3 to a duty cycle of cycle_high/cycle_time.  
**RETURN VALUE:			void
**NOTES:			Only initalizes PWM1, channel 3 (only supports single edge)
*/
void PWM_Init(uint32_t prescale, uint32_t cycle_time, uint32_t cycle_high, unsigned char pclk_div)
{
	uint32_t rmw;
	LPC_SC->PCONP |= PWM_PWR; //Turn on power (Should be enabled by default)
	
	LPC_SC->PCLKSEL0 |= (1 << 12); //CCLK
	LPC_GPIO3->FIODIR |= (1 << 26);
	LPC_PINCON->PINSEL7 |= (3 << 20); //Enable pin 3.29 as PWM1 Ch 3 output	
	LPC_PWM1->PR = prescale;
	LPC_PWM1->MR0 = cycle_time;
	LPC_PWM1->MR3 = cycle_high; 
//	LPC_PWM1->PCR = PWMENA3; //Enable PWM outputs
	LPC_PWM1->MCR = PWMMR0R;
	LPC_PWM1->TCR = COUNTER_ENABLE | //Counter enable
			PWM_ENABLE ; //PWM enable
	LPC_PWM1->LER = ENB_PWM_MATCH0;
}
/*START FUNCTION DESCRIPTION
**PWM_set			<pwm.c>
**SYNTAX:			PWM_set(uint32_t, uint32_t, uint32_t)
**PARAMETER1:			PR1 prescaler value.  TC is incremented every PR+1 ticks
**PARAMETER2:			Cycle time; MR0 value.  Restarts when TC == MR0
**PARAMETER3:			Cycle High time: MR1 value.  Signal drops low when TC == MR1
**KEYWORDS:			PWM1 set duty cycle
**DESCRIPTION:			sets PWM1, channel 1 to a duty cycle of cycle_high/cycle_time.  
**RETURN VALUE:			void
**NOTES:			Only sets PWM1, channel 1 (only supports single edge)
*/
void PWM_set(uint32_t prescale, uint32_t cycle_time, uint32_t cycle_high)
{
	LPC_PWM1->PR = prescale;
	LPC_PWM1->MR0 = cycle_time;
	LPC_PWM1->MR3 = cycle_high;
	LPC_PWM1->TCR = COUNTER_ENABLE | PWM_ENABLE;
	LPC_PWM1->LER = ENB_PWM_MATCH0;

}

void disablePWM()
{
	LPC_PWM1->PCR = 0;
}

void enablePWM()
{
	LPC_PWM1->PCR = PWMENA3; //Enable PWM output (Ch3)

}
