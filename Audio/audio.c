#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "RDB1768.h"
#include "LPC17xx.h"
#include "uart0.h"
#include "ff.h"
#include "filesystem.h"
#include "audio.h"
/**
*	@file audio.c
*	@brief This file manages the audio subsystem
*	This file manages the audio subsystem.  It uses the FatFS filesystem drivers to open an audio file and play the file.  At the time of writing this, it always plays the first file sent to it.  If the same file is sent multiple times, it will fail and block until a system reset occurs
*	@author Kyle Swenson
*	@date 26 FEB 2013
*/
#define IDLE 0
#define BUSY 1
xQueueHandle audioControlQueue = NULL;	/*!< Queue for Audio Control*/
xQueueHandle audioRXQueue = NULL;	/*!< Queue for Audio Rx*/		
xSemaphoreHandle audioBusy = NULL;	/*!< Semaphore indicating if the audio subsystem is currently busy*/
xSemaphoreHandle dmaRequest = NULL;	/*!< Mutex for deferred ISR processing*/	 
FIL audioFile, fp2;			/*!< File pointers for file handles*/  
uint32_t receiveAudio[8]; 
uint32_t audio1[8], audio2[8];		/*!< DMA audio sample memory locations */ 
uint8_t audioState = IDLE;
/**
*	This function initalized pin functions and directions for the UDA1345TS
*	NOTES:
*	Initalizes the pins connected to UDA1345TS. 	
*	UDA1345TS Pin	LPC1768 Pin	Pin Function	Pin Type
*	MP1, 9		P0.4, 81	MP1		GPIO	
*	MP5, 20		P0.5, 80	MP5		GPIO, input, OVERFLOW output
*	DIN, 19		P0.6, 76	I2STX_SDA	I2S Data output
*	BCK, 16		P0.7, 78	I2STX_CLK	Bit Clock Output
*	WS, 17		P0.8, 77	I2STX_WS	Word Select Output
*	DOUT, 18	P0.6, 79	I2SRX_SDA	I2S Data input
*	SYSCLK, 12	P4.28, 82	I2SRX_MCLK	I2S System Clock
*	MP2, 13		P1.22, 36	L3MODE		GPIO, L3 CS signal
*	MP3, 14		P1.20, 34	SSCL		SSP0 SCK
*	MP4, 15		P1.24, 38	MOSI0		SSP0 MOSI
*
*	@author Kyle Swenson
*	@date	26 FEB 2013
*/
void audio_init()
{	
	LPC_PINCON->PINSEL0 &= ~((0x03 << 8) | (0x03 <<10) | (0x03 << 12) | (0x03 << 14) | (0x03 << 16) | (0x03 << 18));
	LPC_PINCON->PINSEL0 |= ( 0x00 << 8) | //GPIO for MP1
				(0x00 << 10) | //GPIO for MP5
				(0x01 << 12) | //I2S_RXSDA for DIN
				(0x01 << 14) | //I2STX_CLK for BCK
				(0x01 << 16) | //I2STX_WS for WS
				(0x01 << 18); //I2STX_SDA for DOUT
	LPC_PINCON->PINSEL9 &= ~((0x03 << 24)|(0x03 << 26));
	LPC_PINCON->PINSEL9 |= (0x01 << 24)|(0x01 << 26); //RX_MCLK for SYSCLK
	LPC_PINCON->PINSEL3 &= ~((0x03 << 12)|  //GPIO for L3MODE 
				(0x03 << 8) | //GPIO for SSCL
				(0x03 << 16) ); //GPIO for MOSI0;
	LPC_PINCON->PINSEL3 |= ( 0x03 << 16)|(0x03 << 8) ; //MOSI0 for MOSI0, SCL0;

	LPC_GPIO0->FIODIR |= (1 << 4) | (1 << 5);	//Set Pins P0.4 and P0.5 for input 
	LPC_GPIO1->FIODIR |= (1 << 22);


}
/**
*	This function initializes the mode control pins for the UDA1345TS.  
*	
*	NOTES:	
*	MC1 is on P2.2, MC1 is on P2.1
*	MC1|MC2|Function
*	 0 | 0 | L3MODE
*	 0 | 1 | TEST
*	 1 | 0 | TEST
*	 1 | 1 | Static Pin mode
*	@author Kyle Swenson
*	@date 26 FEB 2013
*/
void mcp_init(uint8_t static_mode)
{
	LPC_GPIO2->FIODIR |= ( 1 << 2)|(1 << 1);
	LPC_GPIO2->FIOCLR |= (1 << 2)|(1 << 1);	//SET l3mode
	if(static_mode)
		LPC_GPIO2->FIOSET |= (1 << 2)|(1 << 1);	//SET static pin mode

}
/**START FUNCTION DESCRIPTION
static_pin_init			<main.c>
SYNTAX:				void static_pin_init();
KEYWORDS:			static pin init, initalization
DESCRIPTION:			Sets static pin modes for all static pins
RETURN VALUE:			None.
NOTES:	
	MP1 is on P0.4
	MP2 is on P1.22
	MP3 is on P1.20
	MP4 is on P1.24
	MP5 is on P0.5
	
	MP1 and MP5 control data input format
		MP1 | MP5 | Function
		0	0	MSB-Justified
		0	1	I2S-Bus
		1	0	MSB output LSB 20 bit input
		1	1	MSB output LSB 16 bit input
	MP2 controls Demphasis and Mute
		0	No de-emphasis and mute
		.5VDD	De-emphasis at 44.1kHz
		1	Mutes
	MP3 controls system clock frequency
		0	256Fs
		1	384Fs
	MP4 Controls ADC mode
		0	ADC Power-Down mode
		.5VDD	6dB gain mode
		1	0dB gain mode
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
void l3SendCmd(uint8_t address, uint8_t data)
{
	#define SETL3MODE {LPC_GPIO1->FIOPIN |= (1 << 22);}
	#define CLRL3MODE {LPC_GPIO1->FIOPIN &= ~(1 << 22);}
	uint8_t i;
	LPC_GPIO1->FIOPIN |= (1 << 21);
	SETL3MODE
	for(i=0;i<0xFF;i++);
	CLRL3MODE
	LPC_SSP0->DR = address;
	while(LPC_SSP0->SR & (1 << 4));	
	SETL3MODE
	CLRL3MODE
	CLRL3MODE
	SETL3MODE
	LPC_SSP0->DR = data;
	while(LPC_SSP0->SR & (1 << 4));
	for(i=0;i<0x07;i++);
	CLRL3MODE
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
	l3SendCmd(0b00101000, 0b00000000); //Set volume to max
	l3SendCmd(0b00101000, 0b00000001); //De-Emphasis and MuTe off
	l3SendCmd(0b00101000, 0b11000011);	//Power control (Turn everything on)
	l3SendCmd(0b01101000, 0b00001000);	//384*fs, I2S bus, DC filter
	l3SendCmd(0b00000000, 0b00000000);	//Send an invalid address	
}

/* START FUNCTION DESCRIPTION
I2Sinit					<main.c>
SYNTAX:		void I2Sinit();
KEYWORDS:	Init I2S Bus
DESCRIPTION:	Initializes I2S bus for UDA1345TS communication
RETURN VALUE:	None
NOTES:	
	Sets the I2S Bus for 8 bit samples at 16kHz.  
	The SYSCLK is operating at 384Fs.
	The BCLK is operating at 16kHz*8bits/sample*2 channels = 256 kHz
*/
void I2Sinit()
{
	LPC_SC->PCONP |= (1 << 27);		//Power up
	LPC_SC->PCLKSEL1 &= ~(0x03 << 22);
	LPC_SC->PCLKSEL1 |= ( 0x02 << 22); 	//100 MHz clock into I2S subsystem
	LPC_I2S->I2SDAO = (0x00 << 0)  |	//8 Bit data
			(0x00 << 2) |		//Stereo data
			(0x01 << 3) |		//Stop
			(0x01 << 4) | 		//Reset
			(0x00 << 5) |		//LPC1768 as master
			(0x07 << 6) |		//WS period 8
			(0x01 << 15);		//MUTE

	LPC_I2S->I2SDAI = (0x00 << 0)  | 	// 8 Bit word width
			(0x00 << 1) | 		//Sereo Data
			(0x01 << 3) | 		//Stop
			(0x01 << 4) | 		//Reset
			(0x00 << 5) | 		//LPC1768 as master
			(0x07 << 6) ; 		//WS half period 8
	LPC_I2S->I2STXRATE = (118 << 0) | 	//Y_divider
				(29 << 8); 	//X_divider

	LPC_I2S->I2SRXRATE = (118 << 0) | 	//Y
				(29 << 8) ; 	//X
	LPC_I2S->I2STXBITRATE = 23;
	LPC_I2S->I2SRXBITRATE = 23;
	LPC_I2S->I2STXMODE = (0x01 << 0) | 	// Fractional Rate divider clock output as source
			(0x00 << 2) | 		//Transmit in 4 pin mode = false
			(0x01 << 3); 		//Enable tx_mclk output
	LPC_I2S->I2SRXMODE = (0x00 << 0) | 	//Frctional rate divier clk output as source
			(0x00 << 2) | 		//Transmit in 3 pin mode
			(0x01 << 3); 		//Enable rx_mclk output
	

}
/* START FUNCTION DESCRIPTION
enableI2S					<main.c>
SYNTAX:		void enableI2S();
KEYWORDS:	enable I2S
DESCRIPTION:	Enables the I2S bus (takes it out of mute mode and enables output)
RETURN VALUE:	None
NOTES:		None	
*/
void enableI2SOutput()
{
	LPC_I2S->I2SDAO &= ~((1 << 15) | 	//Disable Mute
				(1 << 3) | 	//Disable reset
				(1 << 4));	//Disable stop
}
void disableI2SOutput()
{
	LPC_I2S->I2SDAO |= ((1 << 15) | 	//Enable Mute
				(1 << 4));	//Enable stop

}
void enableI2SInput()
{
	LPC_I2S->I2SDAI &= ~((1 << 3)|(1 << 4));//Disable reset and stop modes


}
void disableI2SInput()
{
	LPC_I2S->I2SDAI |= (1 << 4); //enable reset and stop modes

}
/* START FUNCTION DESCRIPTION
unsignedtosigned					<main.c>
SYNTAX:		uint8_t unsignedtosigned(uint8_t us)	
PARAMETER1:	unsigned 8 bit integer
KEYWORDS:	convert unsigned 2sC
DESCRIPTION:	Converts an unsigned 8 bit integer to a 2sC signed 8 bit integer
RETURN VALUE:	2sC representation of unsigned value  
NOTES:		None	
*/
uint8_t unsignedtosigned(uint8_t us)
{
	us = us >> 2;	
	if(us & 0x80)
		us &= 0x7F;
	else
		us += 128;
	return us;

}

void SetCh5DMA(uint32_t * array)
{
	LPC_GPDMACH5->DMACCConfig = 0; //Stop Ch5 DMA
	LPC_GPDMA->DMACIntTCClear |= ( 1 << 5); //Clear Ch5 TC interrupt

	LPC_GPDMACH5->DMACCDestAddr = &(LPC_I2S->I2STXFIFO);	//Set destination Address of I2S TX fifo
	LPC_GPDMACH5->DMACCSrcAddr = array;		//Set source address as array 1 for audio
	LPC_GPDMACH5->DMACCControl = (8 << 0) | //Set the transfer size to eight
				(0x02 << 12) | //Source burst size (0)
				(0x02 << 15) | //Destination burst size (0)
				(0x02 << 18) | //Source transfer width 32 bits
				(0x02 << 21) | //Destination transfer width 32 bits
				(0x01 << 26) | //Source address is incremented after each transfer
				(0x00 << 27) | //Destination address is not incremented after each transfer
				(0x01 << 31); //Enable terminal count interrupt
	LPC_GPDMACH5->DMACCConfig = ( 1 << 0) | //Enable the channel (Shold be enabled by I2S configuration)
				(0x05 << 6) | //Destination peripheral I2SCh0
				(0x01 << 11)|//Tranfer type is memory to peripherial
				(0x03 << 14);
	LPC_GPDMA->DMACConfig = (1 << 0) ; //Enable DMA controller	

}
void DMA_IRQHandler(void)
{
	static uint8_t arraynumber=1;
	static int rx_index = 0;
	uint32_t fretval;
	uint32_t bytes_read;
	uint8_t i;
	uint8_t audioSamples[32];
	portBASE_TYPE xHigherPriorityTaskWoken;
	LPC_GPIO2->FIOPIN |= ( 1 << 12);	
	if(LPC_GPDMA->DMACIntStat & (1 << 5)) //If we have an I2SCh0 interrupt
	{
		if(LPC_GPDMA->DMACIntTCStat & (1 << 5)) //We have an active terminal count request
		{
			LPC_GPDMA->DMACIntTCClear |= ( 1 << 5); //Clear Ch5 TC interrupt
			xSemaphoreGiveFromISR(dmaRequest, &xHigherPriorityTaskWoken);	
		}
		else
		{
			printf("We had a DMA error occur on the transmit channel\n\r");
			while(1);
		}
	
	}
	LPC_GPIO2->FIOPIN &= ~( 1 << 12);	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}
void DMAinit()
{
	LPC_SC->PCONP |= (1 << 29); //Power on GPDMA
	LPC_GPDMA->DMACIntTCClear = 0xFF; //Clear pending terminal count interrupt requests
	LPC_GPDMA->DMACIntErrClr = 0x0FF; //Clear pending error interrupt requests
	LPC_GPDMA->DMACSync = 0x0000; //Enable hardware synchronization
	LPC_GPDMACH5->DMACCDestAddr = &(LPC_I2S->I2STXFIFO);	//Set destination Address of I2S TX fifo
	LPC_GPDMACH5->DMACCSrcAddr = &audio1[0];		//Set source address as array 1 for audio
	LPC_GPDMACH5->DMACCControl = (8 << 0) | //Set the transfer size to 8
					(0x02 << 12) | //Source burst size (8)
					(0x02 << 15) | //Destination burst size (8)
					(0x02 << 18) | //Source transfer width 32 bits
					(0x02 << 21) | //Destination transfer width 32 bits
					(0x01 << 26) | //Source address is incremented after each transfer
					(0x00 << 27) | //Destination address is not incremented after each transfer
					(0x01 << 31); //Enable terminal count interrupt
	LPC_GPDMACH5->DMACCConfig = ( 1 << 0) | //Enable the channel (Should be enabled by I2S configuration)
					(0x05 << 6) | //Destination peripheral I2SCh0
					(0x01 << 11)| //Tranfer type is memory to peripherial
					(0x03 << 14); //Enable interrupt masks
	
	LPC_GPDMACH6->DMACCDestAddr = receiveAudio;
	LPC_GPDMACH6->DMACCSrcAddr = &(LPC_I2S->I2SRXFIFO);
	LPC_GPDMACH6->DMACCControl = (8 << 0)	| //Transfer size to 8
				(0x02 << 12)	| //Source burst size = 8
				(0x02 << 15)	| //Destination burst size = 8
				(0x02 << 18)	| //Source transfer width 32 bits
				(0x02 << 21)	| //Destination transfer width 32 bits
				(0x00 << 26)	| //Source is not incremented after transfer
				(0x01 << 27)	| //Destination is incremented after transfer
				(0x01 << 31);	  //Terminal count interrupt enabled
	LPC_GPDMACH6->DMACCConfig = (1 << 0)	| //Enable the channel
				(0x06 << 1)	| //Source peripherial I2SCh1
				(0x00 << 6)	| //Dest peripherial memory
				(0x02 << 11)	| //Transfer type is peripherial to memory
				(0x03 << 14);	//Enable interrupt masks (enables interrupt)
	LPC_GPDMA->DMACConfig = (1 << 0) ; //Enable DMA controller	
	
	NVIC_EnableIRQ(DMA_IRQn);
}
void enableI2SRXDMA()
{
	LPC_I2S->I2SDMA2 |= (1 << 0) | (8 << 8); //Enable dma2 for receive, trigger on 8 samples.
}
void enableI2STXDMA()
{
	LPC_I2S->I2SDMA1 |= (1 << 1) | //Enable DMA1 for transmit
				(1 << 16); //Trigger on TX FIFO = 1
}
void disableI2STXDMA()
{
	LPC_I2S->I2SDMA1 &= ~((1 << 1) | (1 << 16));

}
uint32_t samplesToWord(uint8_t* samples)
{
	return (samples[3] << 24) | (samples[2] << 16) | (samples[1] << 8) | ( samples[0]);
}
void preserve_config(uint8_t load)
{
	static uint32_t CR0=0,CR1=0, DR=0, SR=0,CPSR=0,IMSC=0, RIS=0, MIS=0, ICR=0, DMACR=0, PCLK=0;
	
	if(load)
	{
		
		LPC_SC->PCLKSEL1 = PCLK;
		LPC_SSP0->CR0 = CR0;
		LPC_SSP0->CR1 = CR1;
		LPC_SSP0->CPSR = CPSR;

	}
	else
	{
		CR0 = LPC_SSP0->CR0;
		CR1 = LPC_SSP0->CR1;
		CPSR = LPC_SSP0->CPSR;
		PCLK = LPC_SC->PCLKSEL1;
	}
}
void vAudioInit()
{
	audio_init();
	mcp_init(0);
	l3init();
	I2Sinit();
	uda1345TS_init();
	static_pin_init();
	DMAinit();

}
void vAudioTask(void * pvParam)
{
	uint32_t return_value;
	uint8_t audioSamples[32];
	uint32_t wave_header[44];
	uint32_t bytes_read;
	uint32_t audioIndex = 0;
	const char filename[21];
	uint32_t i;
	uint8_t audioCh = 1;
	xSemaphoreTake(dmaRequest, portMAX_DELAY);
	
	xQueueReceive(audioControlQueue, (const char*) filename, portMAX_DELAY); //Block until there is a file ready

	
	return_value = f_open(&audioFile, filename, FA_READ|FA_OPEN_EXISTING); //Open the file passed from the queue
//	if(audioFile == NULL)
//	{
//		printf("f_open in vAudioTask failed, return code: ");
//		put_rc(return_value);
//		printf("\n\r");
//		f_close(&audioFile);
//		f_open(&audioFile, filename, FA_READ|FA_OPEN_EXISTING);
//	}

	return_value = f_read(&audioFile, wave_header, 44, &bytes_read);
	if(return_value != FR_OK || bytes_read != 44)
	{
		printf("f_read in vAudioTask failed, return code: ");
		put_rc(return_value);
		printf(" bytes read: %u\n\r", bytes_read);
	}	
	return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);
	if(return_value != FR_OK || bytes_read != 32)
	{
		printf("f_read of audio samples failed, return code: ");
		put_rc(return_value);
		printf(" bytes read: %u\n\r", bytes_read);
	}
	for(i = 0; i < 32; i++)
	{
		audioSamples[i] = unsignedtosigned(audioSamples[i]);
	}
	for(i = 0; i <  8; i++)
	{
		audio1[i] = samplesToWord(&audioSamples[i*4]);
	}
	audioCh = 1;
	SetCh5DMA(audio1);	
	while(1)
	{
		
		enableI2STXDMA();
		enableI2SOutput();
		xSemaphoreTake(dmaRequest, portMAX_DELAY); //Wait until DMA request occurs
		if(audioCh == 2)
		{
			audioCh = 1;		//Start filling array 2
			SetCh5DMA(audio1);	//Set DMA to use audio1 as data source
			
			if(!f_eof(&audioFile))	//If we aren't at the end of the file
			{
				
				return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);	//Read some samples
				if(return_value != FR_OK || bytes_read != 32)
				{
					printf("f_read failed, return code: ");
					put_rc(return_value);
					printf("bytes read : %u\n\r", bytes_read);
				} 
				for(i=0; i < 32; i++)
				{
					audioSamples[i] = unsignedtosigned(audioSamples[i]);
				}	
				for(i=0; i < 8; i++)
				{
					audio2[i] = samplesToWord(&audioSamples[i*4]);
				}
		
			}
			else	//If we are at the end of the file
			{
				disableI2SOutput();
				disableI2STXDMA();
				printf("End of file\n\r");
				f_close(&audioFile);
				xQueueReceive(audioControlQueue, (const char*) filename, portMAX_DELAY); //Block until there is a file ready
				return_value = f_open(&audioFile, filename, FA_READ|FA_OPEN_EXISTING); //Open the file passed from the queue
				if(return_value != FR_OK)
				{
					printf("f_open in vAudioTask failed, return code: ");
					put_rc(return_value);
					printf("\n\r");
				}

				return_value = f_read(&audioFile, wave_header, 44, &bytes_read);
				if(return_value != FR_OK || bytes_read != 44)
				{
					printf("f_read in vAudioTask failed, return code: ");
					put_rc(return_value);
					printf(" bytes read: %u\n\r", bytes_read);
				}	
				return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);
				if(return_value != FR_OK || bytes_read != 32)
				{
					printf("f_read of audio samples failed, return code: ");
					put_rc(return_value);
					printf(" bytes read: %u\n\r", bytes_read);
				}
				for(i = 0; i < 32; i++)
				{
					audioSamples[i] = unsignedtosigned(audioSamples[i]);
				}
				for(i = 0; i <  8; i++)
				{
					audio2[i] = samplesToWord(&audioSamples[i*4]);
				}
			}
		}
		else	//audioCh == 1
		{
			audioCh = 2;		//Set to array2, fill array 1
			SetCh5DMA(audio2);	//Set DMA to use audio2 as data source
			if(!f_eof(&audioFile))	//If we aren't at the end of the file
			{
				return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);	//Read some samples
				if(return_value != FR_OK || bytes_read != 32)
				{
					printf("f_read failed, return code: ");
					put_rc(return_value);
					printf("bytes read : %u\n\r", bytes_read);
				} 
				for(i=0; i < 32; i++)
				{
					audioSamples[i] = unsignedtosigned(audioSamples[i]);
				}	
				for(i=0; i < 8; i++)
				{
					audio1[i] = samplesToWord(&audioSamples[i*4]);
				}
			}
			else	//If we are at the end of the file
			{
				disableI2SOutput();
				disableI2STXDMA();
				f_close(&audioFile);
				printf("File closed\n\r");
				xQueueReceive(audioControlQueue, (const char*) filename, portMAX_DELAY); //Block until there is a file ready
				return_value = f_open(&audioFile, filename, FA_READ|FA_OPEN_EXISTING); //Open the file passed from the queue
				if(return_value != FR_OK)
				{
					printf("f_open in vAudioTask failed, return code: ");
					put_rc(return_value);
					printf("\n\r");
				}

				return_value = f_read(&audioFile, wave_header, 44, &bytes_read);
				if(return_value != FR_OK || bytes_read != 44)
				{
					printf("f_read in vAudioTask failed, return code: ");
					put_rc(return_value);
					printf(" bytes read: %u\n\r", bytes_read);
				}	
				return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);
				if(return_value != FR_OK || bytes_read != 32)
				{
					printf("f_read of audio samples failed, return code: ");
					put_rc(return_value);
					printf(" bytes read: %u\n\r", bytes_read);
				}
				for(i = 0; i < 32; i++)
				{
					audioSamples[i] = unsignedtosigned(audioSamples[i]);
				}
				for(i = 0; i <  8; i++)
				{
					audio1[i] = samplesToWord(&audioSamples[i*4]);
				}
			}

		}
	}
}
void vAudioTask2(void * pvParam)
{
	uint32_t return_value;
	uint32_t wave_header[44];
	uint32_t bytes_read;
	uint8_t audioSamples[32];
	const char filename[21];
	uint32_t i;
//	if(xSemaphoreGive(audioBusy) != pdTRUE)
//	{
//		printf("Failed to give audioBusy Semaphore\n\r");
//	}
	xSemaphoreTake(audioBusy, portMAX_DELAY);
	while(1)
	{
		xQueueReceive(audioControlQueue, (const char*) filename, portMAX_DELAY); //Block until there is a file ready
		{
			return_value = f_open(&audioFile, filename, FA_READ|FA_OPEN_EXISTING);
	
			if(return_value != FR_OK)
			{
				printf("f_open in vAudioTask failed, return code: ");
				put_rc(return_value);
				printf("\n\r");
			}
	
			return_value = f_read(&audioFile, wave_header, 44, &bytes_read);
			if(return_value != FR_OK || bytes_read != 44)
			{
				printf("f_read in vAudioTask failed, return code: ");
				put_rc(return_value);
				printf(" bytes read: %u\n\r", bytes_read);
			}	
			return_value = f_read(&audioFile, audioSamples, 32, &bytes_read);
			if(return_value != FR_OK || bytes_read != 32)
			{
				printf("f_read of audio samples failed, return code: ");
				put_rc(return_value);
				printf(" bytes read: %u\n\r", bytes_read);
			}
			for(i = 0; i < 32; i++)
			{
				audioSamples[i] = unsignedtosigned(audioSamples[i]);
			}
			for(i = 0; i < 8; i++)
			{
				audio1[i] = samplesToWord(&audioSamples[i*4]);
			}
			enableI2STXDMA();
			enableI2SOutput();
			xSemaphoreTake(audioBusy, portMAX_DELAY);
			if(f_close(&audioFile) != FR_OK)
				printf("error closing the file\n\r");
			printf("closed the file\n\r");	
		}
		
	}
}
void wordToSamples(uint32_t word, uint8_t * samples)
{
	samples[0] = (word & 0x000000FF) >> 0;
	samples[1] = (word & 0x0000FF00) >> 8;
	samples[2] = (word & 0x00FF0000) >> 16;
	samples[3] = (word & 0xFF000000) >> 24;

}
void vAudioRXTask(void * pvParam)
{
	uint32_t rx_array[8];
	uint32_t trx[8];
	uint8_t tbrx[32];
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		receiveAudio[i] = 0x0;		
		trx[i] = 0x0;
		tbrx[i] = 0;
	}
	enableI2SInput();
	enableI2SRXDMA();
	while(1)
	{
		xQueueReceive(audioRXQueue, rx_array, portMAX_DELAY);
		for(i = 0; i < 8; i++)
			trx[i] = rx_array[i];
		for(i = 0; i < 8; i++)
		{
			wordToSamples(trx[i], &tbrx[i*4]);
			printf("trx[%u] = 0x%x\n\r", i, trx[i]);
		}
		
	}
}	
