#include <stdlib.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "RDB1768.h"
#include "LPC17xx.h"
#include "uart0.h"
#include "rtc.h"

RTC_time readRTC()
{
        RTC_time t1; 
        uint32_t c0, c1, c2; 
        if(LPC_RTC->GPREG0 == 0xDEADBEEF && LPC_RTC->GPREG1 == 0xCAFEBABE && LPC_RTC->GPREG2 == 0xBEEFBABE)
        {
		c0 = LPC_RTC->CTIME0;
        	c1 = LPC_RTC->CTIME1;
        	c2 = LPC_RTC->CTIME2;
		
	        t1.seconds = c0 & 0x3F; //Five lower bits are seconds
	        t1.minutes = (c0 & (0x3F << 8))>>8;     //Minutes
       		t1.hours = (c0 & (0x1F << 16))>>16;     //hours
   	     	t1.DOW = (c0 & (0x1F << 24))>>24;       //DOW
        	t1.DOM = (c1 & 0x1F);                   //DOM
        	t1.month = (c1 & (0x0F << 8))>>8;       //Month
        	t1.year = (c1 & (0xFFF << 16))>>16;     //year
        	t1.DOY = (c2 & 0x7FF);                  //DOY
        	return t1;
	}
	else
	{
		t1.seconds = 0;
		t1.minutes = 0;
		t1.hours = 0;
		t1.DOW = 0;
		t1.DOM = 0;
		t1.month = 0;
		t1.year = 0;
		t1.DOY = 0;
		printf("RTC not set!\n\r");
		return t1;
	} 


}
int setRTC(uint8_t sec, uint8_t min, uint8_t hour, uint8_t DOM, uint8_t DOW, uint16_t DOY, uint8_t month, uint16_t year)
{
        if(LPC_RTC->GPREG0 == 0xDEADBEEF && LPC_RTC->GPREG1 == 0xCAFEBABE && LPC_RTC->GPREG2 == 0xBEEFBABE)
        {
                return -1;         //RTC is set
        }
        else
        {

                LPC_RTC->CCR = (0 << 0) | //Disable time counters for so we can set registers
                                (1 << 1)| //When 1, internal clock divider is disable
                                (1 << 4); //Reset and stop calibration counter
                LPC_RTC->CCR = 0x00;;
                LPC_RTC->CIIR = 0x0;    //Disable all interrups on counter sources
                LPC_RTC->AMR = 0xFF;    //Disable comparing all values for interrupts
                LPC_RTC->RTC_AUX = 0x10;        //Clear RTC oscillator fail detect flag
                LPC_RTC->RTC_AUXEN = 0x00;      //Disable RTC oscillator fail detect interrupt
                LPC_RTC->SEC = sec;
                LPC_RTC->MIN = min;
                LPC_RTC->HOUR = hour;
                LPC_RTC->DOM = DOM;
                LPC_RTC->DOW = DOW;
                LPC_RTC->DOY = DOY;
                LPC_RTC->MONTH = month;
                LPC_RTC->YEAR = year;
                LPC_RTC->CCR = 0x01;  //enable time counters;
                LPC_RTC->GPREG0 = 0xDEADBEEF;
                LPC_RTC->GPREG1 = 0xCAFEBABE;
                LPC_RTC->GPREG2 = 0xBEEFBABE;
    		return 0;
        }   
}

