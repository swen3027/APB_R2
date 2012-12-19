//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd. 
//
// UART example project for RDB1768 development board
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 
//
//*****************************************************************************

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "LPC17xx.h"
#include "RDB1768.h"
#include "uart0.h"
#include <string.h>
//#include "audio.h"
// PCUART0
#define PCUART0_POWERON (1 << 3)

#define PCLK_UART0 6
#define PCLK_UART0_MASK (3 << 6)

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

// ***********************
// Function to set up UART
extern xQueueHandle audioControl;
xQueueHandle commandProcessorQueue;	//Queue Handle for the cmdProcessor task
xSemaphoreHandle semUart0;
extern int volume;
char stack[STACK_LIMIT];		//Declares a stack for holding characters
int stack_pointer = 0;			//Initialize pointer to zero
int stack_empty = 1;			//Flag to determine empty/not empty
extern uint32_t CCLK_FREQ;
/*START FUNCTION DESCRIPTION
**UART0_Init			<uart0.c>
**SYNTAX:			UART0_Init(int baudrate)
**KEYWORDS:			UART0 initalization setup configuration
**PARAMETER1:			Integer value specifing the baud rate
**RETURN VALUE:			void
**DESCRIPTION:			Sets up the UART0 for communication, in addition to UART interrupts
**NOTES:			
**END FUNCTION DESCRIPTION
*/
char recievedChar;
//extern void vVolumeIncrease();
//extern void vVolumeDecrease();
//extern void vBassIncrease();
//extern void vBassDecrease();
void UART0_Init(int baudrate)
{
	int pclk;
	unsigned long int Fdiv;
//	portENTER_CRITICAL();
	// PCLK_UART0 is being set to 1/4 of SystemCoreClock
	pclk = CCLK_FREQ / 4;	
	
	// Turn on power to UART0
	LPC_SC->PCONP |=  PCUART0_POWERON;
		
	// Turn on UART0 peripheral clock
	LPC_SC->PCLKSEL0 &= ~(PCLK_UART0_MASK);
	LPC_SC->PCLKSEL0 |=  (0 << PCLK_UART0);		// PCLK_periph = CCLK/4
	
	// Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	LPC_PINCON->PINSEL0 &= ~0xf0;
	LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));
	
	LPC_UART0->LCR = 0x83;		// 8 bits, no Parity, 1 Stop bit, DLAB=1
	Fdiv = ( pclk / 16 ) / baudrate ;	// Set baud rate
	LPC_UART0->DLM = Fdiv / 256;							
	LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		// 8 bits, no Parity, 1 Stop bit DLAB = 0
	LPC_UART0->FCR = 0x07;		// Enable and reset TX and RX FIF0
//	portEXIT_CRITICAL();
	UART0_PrintString("UART0 Ready to accept commands\n\r");	//Print out a ready message
}
/*START FUNCTION DESCRIPTION
**push				<uart0.c>					
**SYNTAX:			void push(char c)
**KEYWORDS:			stack push uart0 
**PARAMETER1:			character to push onto the stack
**RETURN VALUE:			void
**DESCRIPTION:			Pushes a character onto the stack
**NOTES:			Has underflow and overflow protection
**END FUNCTION DESCRIPTION
*/
void push(char c)
{
	if(stack_pointer < STACK_LIMIT) //We still have room on the stack
	{
		stack[stack_pointer] = c;
		stack_pointer++;
		stack_empty = 0;
	}
	else //Stack pointer == STACK_LIMIT
	{
		UART0_PrintString("\n\rERROR: STRING LENGTH EXCEEDED MAXIMUM, COMMAND WILL BE DISCARDED.\n\r");
		memset(stack, 0x00, STACK_LIMIT);
		stack_pointer = 0;
		stack_empty = 1;
		
	}

}
/*START FUNCTION DESCRIPTION
**pop				<uart0.c>
**SYNTAX:			char pop()
**KEYWORDS:			stack pop uart0
**RETURN VALUE:			char at the top of the stack
**DESCRIPTION:			Pops a char off the top of the stack and decrements the stack pointer
**NOTES:			Has underflow and overflow protection
**END FUNCTION DESCRIPTION
*/
char pop()
{
	char c = stack[stack_pointer];
	if(stack_pointer == 0)
	{
		memset(stack, 0x00, STACK_LIMIT);

	}
	else
		stack_pointer--;
	return c;
}
/*START FUNCTION DESCRIPTION
**sendStack			<uart0.c>
**SYNTAX:			void sendStack(int fromISR)
**KEYWORDS:			send queue stack cmdProcessor 
**PARAMETER1:			integer telling the function if it is being called from an ISR.
				0: Not from ISR
				1: From ISR
**RETURN VALUE:			void
**DESCRIPTION:			Makes a copy of the stack, and then wraps the pointer up with the length and sends the address of the struct into the queue, which is read by cmdProcessor
**NOTES:			Resets the stack pointer and clears out the stack with a call to memset
**END FUNCTION DESCRIPTION
*/
void sendStack()
{
	commandStruct CommandData;	//Declare the struct
	portBASE_TYPE higherwoken;	
	char stackToSend[STACK_LIMIT];	//Declare memory for the stack copy
	int i = 0;
	memcpy(stackToSend, stack, STACK_LIMIT);	//Copy the stack over
	stackToSend[stack_pointer] = 0x0;
	CommandData.string = stackToSend;		//Copy the pointer to the struct
	CommandData.length = stack_pointer;		//Grab the length too
	i =0;
	while(i < stack_pointer)
	{
		printf("stack[%d] = %c\n\r", i, stackToSend[i]);
		i++;
	}
	xQueueSend(commandProcessorQueue, &CommandData, 10);
	stack_pointer = 0;	//Reset Stack Pointer
	memset(stack, 0x00, STACK_LIMIT);	//Clear out the stack

}
// ***********************
// Function to send character over UART
void UART0_Sendchar(char c)
{
	LPC_GPIO1->FIOSET = LED2;
	while( (LPC_UART0->LSR & LSR_THRE) == 0 );	// Block until tx empty
	LPC_UART0->THR = c;
	LPC_GPIO1->FIOCLR = LED2;
}

// ***********************
// Function to get character from UART
char UART0_Getchar()
{
	char c;
	while( (LPC_UART0->LSR & LSR_RDR) == 0 )
	{
	//	UART0_PrintString("Waiting for char\n\r");
	}  // Nothing received so just block 	
	c = LPC_UART0->RBR; // Read Receiver buffer register
	return c;
}

void UART0_GetLine(char * buffer, uint32_t length)
{
	int i = 0;
	char c;
	c = UART0_Getchar();
	if(c == NULL /*|| c == '\n' || c == '\r'*/)
	{
		buffer = NULL;
		return 0;
	}
	while(i < length/* && c != '\n' && c != '\r'*/)
	{
	//	UART0_Sendchar(c);
		buffer[i] = c;
		i++;
		c = UART0_Getchar();	

	}
	return i;



}
void UART0_PrintStringL(char *pcString, uint32_t length)
{
	int i = 0;
	// loop through until reach string's zero terminator
//	taskENTER_CRITICAL();
	while (pcString[i] != 0 && i < length) {	
		UART0_Sendchar(pcString[i]); // print each character
		i++;
	}
//	taskEXIT_CRITICAL();
}
// ***********************
// Function to prints the string out over the UART
void UART0_PrintString(char *pcString)
{
	int i = 0;
	// loop through until reach string's zero terminator
//	taskENTER_CRITICAL();
	while (pcString[i] != 0) {	
		UART0_Sendchar(pcString[i]); // print each character
		i++;
	}
//	taskEXIT_CRITICAL();
}
void newline()
{
	UART0_PrintString("\n\r");
}
char hex2char(uint8_t hex)
{
	hex &= 0x0F;
	if(hex < 10)
		hex += 48;
	else
		hex += 55;
	return hex;
}
void UART0_PrintHex(uint32_t datain)
{
	char dataout[9];
	dataout[0] = hex2char((datain & 0xF0000000) >> 28);
	dataout[1] = hex2char((datain & 0x0F000000) >> 24);
	dataout[2] = hex2char((datain & 0x00F00000) >> 20);
	dataout[3] = hex2char((datain & 0x000F0000) >> 16);
	dataout[4] = hex2char((datain & 0x0000F000) >> 12);
	dataout[5] = hex2char((datain & 0x00000F00) >> 8);
	dataout[6] = hex2char((datain & 0x000000F0) >> 4);
	dataout[7] = hex2char((datain & 0x0000000F) >> 0);
	dataout[8] = 0x0; //Null terminator

	UART0_PrintString(dataout);

}
void vUART0_Task(void* pvParameters)
{
	char c; // holds the character that will be read from UART0
	commandStruct CommandData;	//Declare the struct
	char stackToSend[STACK_LIMIT];	//Declare memory for the stack copy
	int arrowcounter = 0;
	int foundArrow = 0x0;
	#define upArrow 0x01
	#define downArrow 0x02
	#define rightArrow 0x04
	#define leftArrow 0x08
//	vSemaphoreCreateBinary(semUart0);
//	xSemaphoreGive(semUart0);
//	UART0_Init(57600);
	while(1)
	{
		if(LPC_UART0->LSR & 0x01) //if there is a character to be recieved....
		{
	//		taskENTER_CRITICAL();
//			LPC_GPIO1->FIOSET = OP1;
			c = LPC_UART0->RBR & 0xFF;
			push(c); //Push the character onto the stack
			switch(c)
			{
				case 0x08: //Backspace
					pop();
					pop(); //pop off the backspace char and the char meant to be deleted
					UART0_Sendchar(0x08);	//back it up
					UART0_Sendchar(0x20);	//over write it
					UART0_Sendchar(0x08);	//back it up again
					break;
				case 0x0D: //\r
//				case 0x0A:
//					pop();
					UART0_Sendchar(0x0D);
					UART0_Sendchar(0x0A);
					push(0x0); //Null terminator
					sendStack(); // Send the stack
					break;
				case 0x1B:
					arrowcounter++;
			//		pop();
					break;
				case 0x5B:
					arrowcounter++;
			//		pop();
					break;
				case 0x41:
					if(arrowcounter == 2)
					{
						arrowcounter = 0;
					//	UART0_PrintString("Up arrow detected\n\r");
						foundArrow = upArrow;
					//	pop();	
					}
					else
						UART0_Sendchar(0x41);
					break;
				case 0x42: //Down arrow
					if(arrowcounter == 2)
					{
						arrowcounter = 0;
						foundArrow = downArrow;
					//	pop();
					}
					else
						UART0_Sendchar(0x42);
					break;
				case 0x43: //right arrow
					if(arrowcounter == 2)
					{
						arrowcounter = 0;
						foundArrow = rightArrow;
					//	pop();
					}	
					else
						UART0_Sendchar(0x43);
					break;
				case 0x44: //Left arrow
					if(arrowcounter == 2)
					{
						arrowcounter = 0;
						foundArrow = leftArrow;
					//	pop();
					}
					else
						UART0_Sendchar(0x43);
					break;
				default:
					UART0_Sendchar(c);
					break;
					
			}
//			LPC_GPIO1->FIOCLR = OP1;
//			taskEXIT_CRITICAL();
			if(foundArrow >0)
			{
				switch(foundArrow)
				{
					case upArrow:
				//		UART0_PrintString("Found up arrow\n\r");
	//					vVolumeIncrease();
						foundArrow = 0;
						break;	
					case downArrow:
				//		UART0_PrintString("Found down arrow\n\r");
	//					vVolumeDecrease();
						foundArrow = 0;
						break;
				
					case rightArrow:
	//			//		UART0_PrintString("Found right arrow\n\r");
	//					vBassIncrease();
						foundArrow = 0;
						break;
					case leftArrow:
	//			//		UART0_PrintString("Found left arrow\n\r");
	//					vBassDecrease();
						foundArrow = 0;
						break;
					default:
						UART0_PrintString("An arrow error has occured\n\r");
						break;
				}
			}
		}

	}
}
