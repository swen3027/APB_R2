#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "LPC17xx.h"
#include "l3.h"



/**START FUNCTION DESCRIPTION
*	mcp_init					<Audio/l3.c>
*	SYNTAX:			void mcp_init(uint8_t)
*	PARAMETER1:		1 or a 0 enabling or disabling static pin mode
*	DESCRIPTION:		This function initializes the mode control pins for the UDA1345TS.  
*	RETURN VALUE:		Void
*	NOTES:	
*		MC1 is on P2.2, MC1 is on P2.1
*		MC1 | MC2 | Function
*		 0  |  0  | L3MODE
*		 0  |  1  | TEST  
*		 1  |  0  | TEST
*		 1  |  1  | Static Pin Mode
*	
*/
void mcp_init(uint8_t static_mode)
{
	LPC_GPIO2->FIODIR |= ( 1 << 2)|(1 << 1);
	LPC_GPIO2->FIOCLR |= (1 << 2)|(1 << 1);	//SET l3mode
	if(static_mode)
		LPC_GPIO2->FIOSET |= (1 << 2)|(1 << 1);	//SET static pin mode

}
/**START FUNCTION DESCRIPTION
*	static_pin_init					<Audio/l3.c>
*	SYNTAX:		void static_pin_init(void)
*	DESCRIPTION:	Configures the UDA1345TS in static pin mode	
*	NOTES:	
*		Requires that mcp_init(1) be called first
*		MP1 is on P0.4 
*		MP2 is on P1.22  
*		MP3 is on P1.20  
*		MP4 is on P1.24  
*		MP5 is on P0.5  
*	MP1 and MP5 control data input format
*		MP1 | MP5 | Function
*		 0  |  0  | MSB-Justified
*		 0  |  1  | I2S-Bus
*		 1  |  0  | MSB output LSB 20 bit input
*		 1  |  1  | MSB output LSB 16 bit input
*	MP2 controls Demphasis and Mute
*		0	No de-emphasis and mute
*		.5VDD	De-emphasis at 44.1kHz
*		1	Mutes
*	MP3 controls system clock frequency
*		0	256Fs
*		1	384Fs
*	MP4 Controls ADC mode
*		0	ADC Power-Down mode
*		.5VDD	6dB gain mode
*		1	0dB gain mode
*/
void static_pin_init()
{
	LPC_PINCON->PINSEL0 &= ~((3 << 8)|(3 << 10));
	LPC_PINCON->PINSEL3 &= ~((3 << 8)|(3 << 12)|(3 << 16));
	LPC_GPIO0->FIODIR = (1 << 4) | (1 << 5);
	LPC_GPIO1->FIODIR = (1 << 22) | ( 1 << 20) | (1 << 24);
	LPC_GPIO0->FIOCLR = ( 1 << 4);		//Set I2S Bus as input
	LPC_GPIO0->FIOSET = (1 << 5);		//Set I2S Bus as input
	LPC_GPIO1->FIOCLR = (1 << 20) | (1 << 22);
	LPC_GPIO1->FIOSET = ( 1 << 24)|(1 << 20);// | (1 << 22); //384Fs, ADC in 0dB gain
}
/**START FUNCTION DESCRIPTION
l3init				<main.c>
SYNTAX:				void l3init()
KEYWORDS:			L3 bus initalization
DESCRIPTION:			Initalizes pins and SSP0 for L3 Bus function 
RETURN VALUE:			None.
NOTES:	
	Sets the L3Bus for a 2 MhZ clock, 8 bit data transfer.  
	The L3Bus has the clock polarity high between frames.
	The SD card is on the SSP0 bus as well, and uses different parameters.  
	The SSP0 is first configured here, then reconfigured when the filesystem is mounted.	
*/
void l3init()
{
	
	LPC_SC->PCLKSEL1 &= ~(0x03 << 10);
	LPC_SC->PCLKSEL1 |= ( 0x02 << 10); 	//cclk/1
	LPC_SSP0->CPSR = 50; 			//2 MHz clock
	LPC_SSP0->CR0 = (0x07) | 		//8 bit data transfer
			(0x00 << 4) | 		//SPI format
			(1 << 6) | 		//Clock polarity high between frames
			(1 << 7) ; 		//
	LPC_SSP0->CR1 = 0x02;			//Enable SSP

}	
/**START FUNCITON DESCRIPTION
l3SendCmd						<Audio/l3.c>
SYNTAX:				void l3SendCmd(uint8_t, uint8_t)
PARAMETER1:			Address of the L3 Device.  Varies depending on data or status command
PARAMETER2:			Data value to be clocked into the L3 Register
RETURN VALUE:			Void
DESCRIPTION:			This function sends out the address of provided in the first argument
				and sends it out the SSP0 bus.  Then, it sends the data in compliance with
				the L3 data format
NOTES:				Uses the same bus as the file system
*/
void l3SendCmd(uint8_t address, uint8_t data)
{
	uint8_t i;
	LPC_GPIO1->FIOPIN |= (1 << 21);		//Set SD Card CS high
	SETL3MODE				//Set L3 Mode high (data mode)
	for(i=0;i<0xFF;i++);			//Delay
	CLRL3MODE				//Set L3 Mode low ( address mode)
	LPC_SSP0->DR = address;			//Send out the address
	while(LPC_SSP0->SR & (1 << 4));		//Wait until SSP0 is complete
	SETL3MODE				//Set L3 Mode high (data mode)
	CLRL3MODE				//Drop L3 Mode low for a Thalt (See UDA1345TS datasheet, pg 14 and 22) 
	CLRL3MODE
	SETL3MODE				//Set L3 Mode high for data mode
	LPC_SSP0->DR = data;			//Send out the data
	while(LPC_SSP0->SR & (1 << 4));		//Wait until SSP0 is complete
	for(i=0;i<0x07;i++);			//Delay for a Thold
	CLRL3MODE				//Go back to address mode to avoid interferrence with SD card
}	
/**START FUNCTION DESCRIPTION
uda1345TS_init			<main.c>
SYNTAX:				void usa1345TS_init()
KEYWORDS:			L3 bus, UDA1345 L3Bus initalization
DESCRIPTION:			Uses the L3 Bus to initalize the UDA1345TS
RETURN VALUE:			None.
NOTES:				Doesn't work, using static pin mode instead	
*/
void uda1345TS_init()
{
	l3SendCmd(0b00101000, 0b00000000); 	//Set volume to max
	l3SendCmd(0b00101000, 0b00000001); 	//De-Emphasis and MuTe off
	l3SendCmd(0b00101000, 0b11000011);	//Power control (Turn everything on)
	l3SendCmd(0b01101000, 0b00001000);	//384*fs, I2S bus, DC filter
	l3SendCmd(0b00000000, 0b00000000);	//Send an invalid address	
}
