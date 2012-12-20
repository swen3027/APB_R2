#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "handlers.h"

void DebugMon_Handler(void)
{
	MESSAGE("In DebugMon Handler\n\r");
	while(1);
	
}
void NMI_Handler (void)
{
	MESSAGE("In NMI Handler \n\r");
	while(1);
	
}
void HardFault_Handler(void)
{
	MESSAGE("In HardFault Handler \n\r");
	while(1);
	
}
void MemManage_Handler(void)
{
	MESSAGE("In MemManage Handler \n\r");
	while(1);
	
}
void BusFault_Handler(void)
{
	MESSAGE("In BusFault Handler\n\r");
	while(1);


} 
void UsageFault_Handler(void)
{
	MESSAGE("In UsageFault Handler\n\r");
	while(1);
	
}
void IntDefaultHandler(void)

{

	MESSAGE("In Default Handler \n\r");

	while(1);



} 
