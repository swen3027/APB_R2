#include "LPC17xx.h"

#define SCS_Val               0x00000020	//OSCRANGE B/t 1-20MHz, OSCEN = 1
#define CLKSRCSEL_Val         0x00000001	//Osc clock

#define PCONP_Val             0x042887DE
#define CLKOUTCFG_Val         (9 << 4)|(1 << 8)	//Divide cclk by 10 and output to P1.27



#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */

/*START FUNCTION DESCRIPTION: SystemInit	<system_LPC17xx.c>
**SYNTAX:		void SystemInit(uint32_t, uint32_t, uint32_t, uint32_t
**PARAMETER1:		Clock source selection:
**				0x00 = Internal RC Osciliator (4 MHz)
**				0x01 = Main Osciliator (12 MHz)
**				0x02 = RTC Osciliator (32 kHz)
**PARAMETER2:		PLL0 Multiplier value: 0 - 511
**PARAMETER3:		PLL0 Divider value: 0 - 31			
**PARAMETER4:		CCLKDIV value: 0-255
**KEYWORDS:		CPU clock config PLL0 CCLKCFG CCLK CPUCLK
**DESCRIPTION:		Configures PLL0 and CCLKCFG to generate a desired clock frequency.
**RETURN VALUE:		Clock speed in Hz
**NOTES:		The values passed into this function should come from a look up table/ generator.  If this is improperly configured, then nothing will work on the board including programming functions.  User must enter ISP mode to reprogram with a valid binary file.
**END FUNCTION DESCRIPTION*/

uint32_t SystemInit (uint32_t clksrc, uint32_t m, uint32_t n, uint32_t cclkdiv)
{
	uint32_t clk_val = 0;

	LPC_SC->SCS = SCS_Val;
	if (SCS_Val & (1 << 5)) {             /* If Main Oscillator is enabled      */
   		while ((LPC_SC->SCS & (1<<6)) == 0);/* Wait for Oscillator to be ready    */
  	}

	LPC_SC->CCLKCFG = cclkdiv;
	LPC_SC->CLKSRCSEL = clksrc;
  	LPC_SC->PLL0CFG = (m | (n << 16)); 
	LPC_SC->PLL0FEED  = 0xAA;
  	LPC_SC->PLL0FEED  = 0x55;
	LPC_SC->PLL0CON   = 0x01;             /* PLL0 Enable                        */
	LPC_SC->PLL0FEED  = 0xAA;
	LPC_SC->PLL0FEED  = 0x55;
	while (!(LPC_SC->PLL0STAT & (1<<26)));/* Wait for PLOCK0                    */
	LPC_SC->PLL0CON   = 0x03;             /* PLL0 Enable & Connect              */
	LPC_SC->PLL0FEED  = 0xAA;
	LPC_SC->PLL0FEED  = 0x55;
	while (!(LPC_SC->PLL0STAT & ((1<<25) | (1<<24))));/* Wait for PLLC0_STAT & PLLE0_STAT */
	LPC_SC->PCLKSEL0  = 0x0;     /* Peripheral Clock Selection         */
	LPC_SC->PCLKSEL1  = 0x0;
	LPC_SC->CLKOUTCFG = CLKOUTCFG_Val;    /* Clock Output Configuration         */
	switch(clksrc)
	{
		case 0x00:
			clk_val = 4000000;
			break;
		case 0x01:
			clk_val = 12000000;
			break;
		case 0x02:
			clk_val = 32000;
			break;
	}	
		return ((2 * clk_val * (m+1))/((n+1) * (cclkdiv+1)));
}
