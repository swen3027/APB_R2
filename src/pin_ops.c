#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "RDB1768.h"
#include "LPC17xx.h"
#include "pin_ops.h"
/*START FUNCTION DESCRIPTION	SetPinInput	<pin_ops.c>
**SYNTAX:		SetPinInput(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO input pin port 
**DESCRIPTION:		Sets the specifed port and pin as an input
**RETURN VALUE:		1 on success, 0 on failure
**NOTES:		Ex: SetPinInput(0, 1) will set P0.1 as an input
**END FUNCTION DESCRIPTION*/
uint8_t SetPinInput(uint8_t port, uint8_t index)
{
	uint32_t rmw;
	switch(port)
	{
		case 0:
			rmw = LPC_GPIO0->FIODIR;
			rmw = rmw & ~(1 << index);
			LPC_GPIO0->FIODIR = rmw;
			return 1;
		case 1:
			rmw = LPC_GPIO1->FIODIR;
			rmw = rmw & ~(1 << index);
			LPC_GPIO1->FIODIR = rmw;
			return 1;
		case 2:
			rmw = LPC_GPIO2->FIODIR;
			rmw = rmw & ~(1 << index);
			LPC_GPIO2->FIODIR = rmw;
			return 1;
		case 3:
			rmw = LPC_GPIO3->FIODIR;
			rmw = rmw & ~(1 << index);
			LPC_GPIO3->FIODIR = rmw;
			return 1;
		case 4:
			rmw = LPC_GPIO4->FIODIR;
			rmw = rmw & ~(1 << index);
			LPC_GPIO4->FIODIR = rmw;
			return 1;
		default:
			return 0;

	}

}
/*START FUNCTION DESCRIPTION	SetPinOutput	<pin_ops.c>
**SYNTAX:		SetPinOutput(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO output pin port 
**DESCRIPTION:		Sets the specifed port and pin as an output
**RETURN VALUE:		1 on success, 0 on failure
**NOTES:		Ex: SetPinInput(0, 1) will set P0.1 as an output
**END FUNCTION DESCRIPTION*/
uint8_t SetPinOutput(uint8_t port, uint8_t index)
{
	uint32_t rmw;
	switch(port)
	{
		case 0:
			rmw = LPC_GPIO0->FIODIR;
			rmw = rmw | (1 << index);
			LPC_GPIO0->FIODIR = rmw;
			return 1;
		case 1:
			rmw = LPC_GPIO1->FIODIR;
			rmw = rmw | (1 << index);
			LPC_GPIO1->FIODIR = rmw;
			return 1;
		case 2:
			rmw = LPC_GPIO2->FIODIR;
			rmw = rmw | (1 << index);
			LPC_GPIO2->FIODIR = rmw;
			return 1;
		case 3:
			rmw = LPC_GPIO3->FIODIR;
			rmw = rmw | (1 << index);
			LPC_GPIO3->FIODIR = rmw;
			return 1;
		case 4:
			rmw = LPC_GPIO4->FIODIR;
			rmw = rmw | (1 << index);
			LPC_GPIO4->FIODIR = rmw;
			return 1;
		default:
			return 0;

	}
	
}
/*START FUNCTION DESCRIPTION	SetPin		<pin_ops.c>
**SYNTAX:		SetPin(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO pin output set 1 
**DESCRIPTION:		Sets the specified port pin high
**RETURN VALUE:		1 on success, 0 on failure
**NOTES:		Ex: SetPinInput(0, 1) will set P0.1 to a logical 1 value
**END FUNCTION DESCRIPTION*/
uint8_t SetPin(uint8_t port, uint8_t index)
{
	switch(port)
	{
		case 0:
			LPC_GPIO0->FIOSET = (1 << index);
			return 1;
		case 1:
			LPC_GPIO1->FIOSET = (1 << index);
			return 1;
		case 2:
			LPC_GPIO2->FIOSET = (1 << index);
			return 1;
		case 3:
			LPC_GPIO3->FIOSET = (1 << index);
			return 1;
		case 4:
			LPC_GPIO4->FIOSET = (1 << index);
			return 1;
		default:
			return 0;

	}

}
/*START FUNCTION DESCRIPTION	ClrPin		<pin_ops.c>
**SYNTAX:		ClrPin(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO pin output clr 0 
**DESCRIPTION:		Sets the specified port pin low
**RETURN VALUE:		1 on success, 0 on failure
**NOTES:		Ex: SetPinInput(0, 1) will set P0.1 to a logical 0 value
**END FUNCTION DESCRIPTION*/
uint8_t ClrPin(uint8_t port, uint8_t index)
{
	switch(port)
	{
		case 0:
			LPC_GPIO0->FIOCLR = (1 << index);
			return 1;
		case 1:
			LPC_GPIO1->FIOCLR = (1 << index);
			return 1;
		case 2:
			LPC_GPIO2->FIOCLR = (1 << index);
			return 1;
		case 3:
			LPC_GPIO3->FIOCLR = (1 << index);
			return 1;
		case 4:
			LPC_GPIO4->FIOCLR = (1 << index);
			return 1;
		default:
			return 0;

	}

}
/*START FUNCTION DESCRIPTION	TogPin		<pin_ops.c>
**SYNTAX:		TogPin(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO pin output toggle
**DESCRIPTION:		Sets the specified port pin high if it was low, or low if it was high
**RETURN VALUE:		1 on success, 0 on failure
**NOTES:		Ex: SetPinInput(0, 1) will set P0.1 to a logical 1 value if it was orginally a logic 0 value
**END FUNCTION DESCRIPTION*/
uint8_t TogPin(uint8_t port, uint8_t index)
{
	switch(port)
	{		
		case 0:
			if(LPC_GPIO0->FIOPIN & (1 << index))
				LPC_GPIO0->FIOCLR = (1 << index);
			else
				LPC_GPIO0->FIOSET = (1 << index);
			return 1;
		case 1:
			if(LPC_GPIO1->FIOPIN & (1 << index))
				LPC_GPIO1->FIOCLR = (1 << index);
			else
				LPC_GPIO1->FIOSET = (1 << index);
	
			return 1;
		case 2:
			LPC_GPIO2->FIOPIN ^= (1 << index);
			return 1;
		case 3:
			LPC_GPIO3->FIOPIN ^= (1 << index);
			return 1;
		case 4:
			LPC_GPIO4->FIOPIN ^= ( 1 << index);
			return 1;
		default:
			return 0;
		
	}

}
/*START FUNCTION DESCRIPTION	ReadPin		<pin_ops.c>
**SYNTAX:		ReadPin(uint8_t, uint8_t)
**PARAMETER1:		Port selection: 0-4 for ports 0-4
**PARAMETER2:		Pin index on a given port (0 - 31)
**KEYWORDS:		GPIO read port pin
**DESCRIPTION:		Reads the specifed pin on the specifed port
**RETURN VALUE:		value of the pin ( 1 if the pin is high, 0 if the pin is low
**NOTES:		
**END FUNCTION DESCRIPTION*/
uint8_t ReadPin(uint8_t port, uint8_t index)
{
	uint32_t data;
	switch(port)
	{
		case 0:
			data = LPC_GPIO0->FIOPIN;
			break;
		case 1:
			data = LPC_GPIO1->FIOPIN;
			break;
		case 2:
			data = LPC_GPIO2->FIOPIN;
			break;
		case 3:
			data = LPC_GPIO3->FIOPIN;
			break;
		case 4:
			data = LPC_GPIO4->FIOPIN;
			break;

		default:
			data = 0;
			break;
	}
	return ((data & (1 << index)) >> index);

}
uint8_t vGPIOIntPending(uint8_t port, uint32_t pin)
{
	switch(port)
	{
		case 0:
			if(LPC_GPIOINT->IntStatus & 0x01)
				return 1;
			else
				return 0;
		case 2:
			if(LPC_GPIOINT->IntStatus & 0x04)
				return 1;
			else
				return 0;
		default:
			return 0;
	}


}
uint8_t vGPIOIntClr(uint8_t port, uint32_t pin)
{
//TODO: Write this function	
}
uint8_t GPIOInterruptEnable(uint8_t port, uint32_t pin, uint8_t rising)
{
	uint32_t rmw;
//	vGPIOClearInterrupt(port, pin);
	switch(port)
	{
		case 0:
			rmw = LPC_GPIOINT->IO0IntEnR;
			rmw = rmw | (1 << pin);
			LPC_GPIOINT->IO0IntEnR = rmw;
			break;
		case 2:
			rmw = LPC_GPIOINT->IO2IntEnR;
			rmw = rmw | (1 << pin);
			LPC_GPIOINT->IO2IntEnR = rmw;
			break;
		default:
			return 0;
	}
	
	NVIC_EnableIRQ(EINT3_IRQn);
}
uint8_t getGPIOIntStatusR(uint8_t port, uint32_t pin)
{
	switch(port)
	{
		case 0:
			return ((LPC_GPIOINT->IO0IntStatR & (1 << pin)) >> pin);
		case 2:
			return ((LPC_GPIOINT->IO2IntStatR & (1 << pin)) >> pin);
		default:
			return 0;
	}

}
uint8_t getGPIOIntStatusF(uint8_t port, uint32_t pin)
{
	switch(port)
	{
		case 0:
			return ((LPC_GPIOINT->IO0IntStatF & ( 1 << pin)) >> pin);
		case 2:
			return ((LPC_GPIOINT->IO2IntStatF & (1 << pin)) >> pin);
		default:
			return 0;
	}

}
void EINT3_IRQHandler()
{
	
}

