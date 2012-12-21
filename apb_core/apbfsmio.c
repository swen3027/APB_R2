/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	File:     apbio.c
*	Engineer: Ben Sprague
*	Date:	  31 March 2011 	
*	Description:	Contains functions used by APB state machine to control the
*                 the APB I/O.
Revisions

******************************************************************************/
/**
*	@file apbio.c
*	@author Ben Sprague, changed by Kyle Swenson
*	@brief Contains functions used by APB state machine to control the APB I/O
*	@date 31 March 2011, revised 18 Dec 2012
*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "apbfsmio.h"
#include "timer3.h" //FDW time measurement
#include "timer.h"	//FDW time measurement
#include "pin_ops.h" //Pin operations
#include <stdio.h>

#define FDW_TIMER 2 //using timer2 for FDW time measurement

#define APB_BUTTON_PORT 0 //Set actual button port and index
#define APB_BUTTON_INDEX 0

#define APB_LED_PORT 0
#define APB_LED_INDEX 0

#define APB_VIB_PORT 0
#define APB_VIB_INDEX 0
// External variables for mode/state/inputs with initial values.
apb_mode_t Mode = M_DEFAULT;
apb_beaconmode_t BeacMode = M_NOBEAC;
apb_fsmstate_t FSMState = S_INIT;
apb_phasestate_t PhaseState = P_DONTWALK;
apb_phasecall_t PhaseCall = P_NOCALL;
apb_buttonstate_t ButtonState = B_NOPRESS;

int ExtraPress = 0;                 /**<Extra Press option*/
int EP_SecondDW = 0;                /**<Count DW's seen in Extra Press*/
int VarTimerDone = 0;               /**<Variable software timer expired*/
int MessageComplete = 0;            /**<Audio message completed*/
apb_audiostate_t AudioMessage = A_OFF;  /**<Current audio message*/
int Timeout = 0;                    /**<Walk timeout*/
int RepeatWaitDone = 0;             /**<Time between repeated wait messages done*/
int RepeatWaitEnabled = 0;          /**<Repeated wait enabled*/
int VariableCounter;                /**<Multi-purpose counter*/
int BeacEnable;                     /**<Enable init or dest beacon*/
int GroupCallOn = 0;                /**<Group Call option*/
int VibPulseOn = 0;                 /**<Vibrotactile Call Pulse option*/
apb_recall_t RecallMode = R_OFF;        /**<Recall mode*/
int Destination = 0;                /**<Destination beacon*/
uint32_t t;                           /**<Used for time measurement*/
int FDW_Time;                       /**<Record time in FlashingDW*/
int Timing = 0;                     /**<Show if currently measuring FDW period*/
int AudibleCountdown = 0;           /**<Audible countdown enabled*/

static int FDWInRange( int prevtime , int currtime , int tolerance );

#ifndef FSMTesting
xTimerHandle VARIABLETIMER; /**< FreeRTOS timer handle for a variable length timer*/
xTimerHandle TIMEOUTTIMER; /**< FreeRTOS timer handle for a time out timer*/

/**
*	This function checks the station configuration and sets the appropriate FSM variables
*	@author Ben Sprague
*
*/
void GetConfig(void){
      // Check AAPS Mode
	if(StationConfig.Mode == DEFAULT)
	{
		Mode = M_DEFAULT;
	}
	else if(StationConfig.Mode == APS)
	{
		Mode = M_EPAPS;
   	}
   	else if(StationConfig.Mode == IDENT)
	{
      		Mode = M_IDENT;
   	}
   	else
	{
      		Mode = M_OFF;
   	}
      	// Check Beaconing Mode
   	if(StationConfig.Beacon_mode == PING_PONG )
	{
      		BeacMode = M_PINGPONG;
   	}
   	else if(StationConfig.Beacon_mode == TARGET)
	{
      		BeacMode = M_TARGET;
   	}
   	else
	{
      		BeacMode = M_NOBEAC;
   	}

      	// Check for Extra Press option
 	if(StationConfig.Exp)
	{
      		ExtraPress = 1;
   	}
   	else
	{
      		ExtraPress = 0;
   	}

      	// Check for Repeated Wait Message option
   	if(StationConfig.Repeat_Wait == 0)
	{
      		RepeatWaitEnabled = 0;
   	}
   	else
	{
      		RepeatWaitEnabled = 1;
   	}

      	// Check for Group Call option
   	if(StationConfig.GroupCall)
	{
      		GroupCallOn = 1;
   	}
   	else
	{
      		GroupCallOn = 0;
   	}

      	// Check for Vib Call Pulse option
	if(StationConfig.VibPulse)
	{
      		VibPulseOn = 1;
   	}
   	else
	{
      		VibPulseOn = 0;
   	}

     	// Check Recall Mode
   	if(StationConfig.Recall == RECALL_PED)
	{
      		RecallMode = R_PED;
   	}
   	else if(StationConfig.Recall == RECALL_APS)
	{
      		RecallMode = R_APS;
   	}
   	else
	{
      		RecallMode = R_OFF;
   	}
}
/**
*	Internal function to check if the FDW signal is within range given a tolerance
*	@param prevtime Previous time
*	@param currtime Current time
*	@param tolerance Tolerance for difference between current and previous time
*	@author Ben Sprague
*/
static int FDWInRange( int prevtime , int currtime , int tolerance )
{
	int delta;
   	delta = prevtime - currtime;

   	if( delta < 0 )
	{
      		delta = -delta;
   	}
	return( delta <= tolerance );

}
/**
*	This function checks the phase state and call and set the appropriate FSM variables.
*	Button State is set in apbio.c in the button ISR.  Assumes that FDW_TIMER has been initailized prior to this function getting called
*	@author Ben Sprague, modified by Kyle Swenson
*	
*/
void GetStatus(void)
{
      	// Check Phase State
   	if(StatusBlock.globalStatus & 0x01)
	{
      		PhaseState = P_PREEMPT;
   	}
   	else if(StationConfig.Phase & StatusBlock.dWalks) //DW
	{ 
      		PhaseState = P_DONTWALK;
      		if(Timing)
		{
			//stopTimer3(); //DW - end FDW time measurement
	  		disableTimerX(FDW_TIMER);
       			int prev = FDW_Time;
			FDW_Time = timerXValue(FDW_TIMER)/10000 - 1; //Convert 100 us to s

         		if( FDWInRange( prev , FDW_Time , 1 ) )
			{
            			//AudibleCountdown = StationConfig.AudibleCountdown;
            			AudibleCountdown = 1;
         		}
         		else
			{
            			AudibleCountdown = 0;
         		}

         		Timing = 0;                   //No longer measuring time
      		}
   	}
   	else if(StationConfig.Phase & StatusBlock.clears) //FDW
	{ 
      		PhaseState = P_FLASHINGDW;
      		if(!Timing)
		{
			resetTimerX(FDW_TIMER);
			enableTimerX(FDW_TIMER); //Start FDW time measurement
         		Timing = 1;                   //Currently measuring time
      		}
   	}
   	else if(StationConfig.Phase & StatusBlock.walks) //Walk
	{
      		if(Timing)
		{    //If timing in W, end timing but don't record
			disableTimerX(FDW_TIMER);
         		Timing = 0;
      		}
    		PhaseState = P_WALK;
   	}
   	else
	{
      		if(Timing)
		{    //If timing in bad state, end timing but don't record
			disableTimerX(FDW_TIMER);
         		Timing = 0;
      		}
      		PhaseState = P_UNKNOWN;
    		  /*TODO: this is a bad fix to disabling button in flash. */
      		FSMState = S_IDLE_INIT;
   	}

      	// Check Phase Call
  	if(StationConfig.Group & StatusBlock.apsCalls)
	{
	   	PhaseCall = P_APSCALL;
   	}
   	else if(StationConfig.Phase & StatusBlock.calls)
	{
	   PhaseCall = P_CALL;
   	}
   	else
	{
	   PhaseCall = P_NOCALL;
   	}
}

/**
*	This function sends a call specified by the APB state machine using SNMP_Trap
*	@param call The type of call sent by FSM
*	@author Ben Sprague
*/
void SendCall( apb_phasecall_t call )
{
	switch(call)
  	{
		case P_NOCALL:
	 	break;
		case P_CALL:
			//TODO	  SNMP_Trap(PEDCALL); //FInd out what SNMP_Trap actually is
	  	break;
		case P_APSCALL:
			//TODO	  SNMP_Trap(APSCALL); //Find out what SNMP_Trap is
	  	break;
   		default:
	  	break;
  	}
}

/**
*	Turns the APB LED on or off depending on the value specified by the APB FSM
*	@param led The value specified by the FSM (either ON or OFF)
*	@author Ben Sprague
*/

void SetLed( apb_ledstate_t led )
{
#ifndef FSM_LEDONWALK
	switch(led)
  	{
		case LED_OFF:
			ClrPin(APB_LED_PORT, APB_LED_INDEX);
	  	break;
		case LED_ON:
			SetPin(APB_LED_PORT, APB_LED_INDEX);
	  	break;
  	}
#endif
}

/**
*	Turns the APB vibration on or off depending on the value specified by the APB state machine.  Also enables/disabled the button ISR.
*	@author Ben Sprague
*	@param vib The value specified by the FSM (either ON or OFF)
*/
void SetVib( apb_vibstate_t vib )
{
  	switch(vib)
  	{
		case VIB_OFF:
			ClrPin(APB_VIB_PORT, APB_VIB_INDEX);
     			// Wait and then enable the button.
			//TODO     OS_Delay( 1 );
			//TODO     enablebutton(); //Enables button ISR
	  	break;
		case VIB_ON:
      			SetPin(APB_VIB_PORT, APB_VIB_INDEX);
			// Disable the button isr to prevent false calls.
			//TODO      disablebutton(); //Disabled button ISR
	  	break;
  	}
}

/**
*	Sets the APB audio depending on the value specified by the APB state machine
*	@param audio The type of audio sent by the FSM
*	@author Ben Sprague
*/
void SetAudio( apb_audiostate_t audio )
{
	AudioMessage = audio;
	//TODO	OS_SignalEvent( AUDIOTASK_E_RELOAD , &AudioTask ); // Play Locator message
}

/**
*	Returns the station's ID number, so that the state machine can run independently of I/O for software testing
*/
int GetStationID(void)
{
	return StationConfig.ID;
}
/**
*	Callback function for the variable timer, called when the variable timer expires
*	@author Kyle Swenson
*	@param xTimer Handle to timer (Not used, included to fit FreeRTOS function prototype)
*/
void VariableTimer(xTimerHandle xTimer) //changed to fit FreeRTOS function prototype
{
	VarTimerDone = 1;
}
/**
*	Callback function for the Timeout timer, called when the timeout timer expires
*	@author Kyle Swenson
*	@param xTimer Handle to timer (Not used, included to fit FreeRTOS function prototype)
*/
void TimeoutTimer(xTimerHandle xTimer)
{
   if(StationConfig.WalkTimeout > 0)
	   Timeout = 1;
   //Never time out if WalkTimeout is negative
}
/**
*	This function creates a FreeRTOS software timer for the variable timer, intially set to a 1 second period.  The timer does not reload when it expires
*/
void CreateVariableTimer()
{
//   OS_CreateTimer(&VARIABLETIMER, VariableTimer, 1000); //Initial 1 sec period
	VARIABLETIMER = xTimerCreate("VarTimer", //Text name (not used by system)
				1000/portTICK_RATE_MS, //Initial 1 sec period
				0,	//AutoReload is false
				1, 	//Timer ID = 1
				VariableTimer); //Callback function	

}
/**
*	This function creates a FreeRTOS software timer for the timeout timer, initially set to StationConfi.WalkTimeout.  The timer does not reload when it expires
*/
void CreateTimeoutTimer()
{
//   OS_CreateTimer(&TIMEOUTTIMER, TimeoutTimer, StationConfig.WalkTimeout);
	TIMEOUTTIMER = xTimerCreate("TOTim",
				StationConfig.WalkTimeout/portTICK_RATE_MS,
				0,
				2,
				TimeoutTimer);
}
/**
*	This function restarts the variable timer after changing the length of the timer
*	@param timer Specifies the length of the timer.  Valid values are T_ONESEC, T_HALFSEC, T_MS200 and T_MS100
*/
void RestartVariableTimer(apb_variabletimer_t timer)
{
   VarTimerDone = 0;
	switch(timer)
	{
		case T_ONESEC:
			if(xTimerChangePeriod(VARIABLETIMER, 1000/portTICK_RATE_MS, 0) == pdFAIL)
				MESSAGE("Failure to change variable timer's period, apbfsmio.c:345\n\r");
        	break;
		case T_HALFSEC:
         		if(xTimerChangePeriod(VARIABLETIMER, 500/portTICK_RATE_MS, 0) == pdFAIL)
				MESSAGE("Failure to change variable timer's period, apbfsmio.c:349\n\r");
         	break;
		case T_MS200:
         		if(xTimerChangePeriod(VARIABLETIMER, 200/portTICK_RATE_MS, 0) == pdFAIL)
				MESSAGE("Failure to change variable timer's period, apbfsmio.c:353\n\r");
         	break;
      		case T_MS100:
         		if(xTimerChangePeriod(VARIABLETIMER, 100/portTICK_RATE_MS, 0) == pdFAIL)
				MESSAGE("Failure to change variable timer's period, apbfsmio.c:357\n\r");
       
         	break;
		case T_WAIT:
         		if(xTimerChangePeriod(VARIABLETIMER, StationConfig.Repeat_Wait/portTICK_RATE_MS, 0) == pdFAIL) 
				MESSAGE("Failure to change variable timer's period, apbfsmio.c:362\n\r");
         	break;
	}
  	if(xTimerStart(VARIABLETIMER, 0) == pdFAIL)
		MESSAGE("Failure to start the variable timer, apbfsmio.c:366\n\r"); 
//OS_RetriggerTimer(&VARIABLETIMER);
}
/**
*	This function restarts the time out timer to the StationConfig.WalkTimeout, but only if StationConfig.WalkTimeout is positive or zero.  If it isn't positive or zero, it is handled in the timer function (?)
*/
void RestartTimeoutTimer()
{
	Timeout = 0;
	if(StationConfig.WalkTimeout >= 0)
		if(xTimerChangePeriod(TIMEOUTTIMER, StationConfig.WalkTimeout/portTICK_RATE_MS, 0) == pdFAIL)
			MESSAGE("Failure to change timeout timer's period, apbfsmio.c:375\n\r");
 //     OS_SetTimerPeriod(&TIMEOUTTIMER,StationConfig.WalkTimeout);
   	else  //Never time out - will be handled in timer function
  		if(xTimerChangePeriod(TIMEOUTTIMER, 10000/portTICK_RATE_MS, 0)==pdFAIL) //Arbitrary period
			MESSAGE("Failure to change the timeout timer's period, apbfsmio.c:379\n\r");
//	   OS_SetTimerPeriod(&TIMEOUTTIMER,10000); //Arbitrary period
  	if(xTimerStart(TIMEOUTTIMER, 0) == pdFAIL)
		MESSAGE("Failure to re/start the time out timer, apbfsmio.c:382\n\r");
	
//	 OS_RetriggerTimer(&TIMEOUTTIMER);
}
/**
*	This function stops the variable timer
*/
void StopVariableTimer()
{
//	OS_StopTimer(&VARIABLETIMER);
	if(xTimerStop(VARIABLETIMER, 0) == pdFAIL)
		MESSAGE("Failure to stop the variable timer, apbfsmio.c:391\n\r");
}
/**
*	 This function stops the timeout timer
*/
void StopTimeoutTimer()
{
	
//	OS_StopTimer(&TIMEOUTTIMER);
	if(xTimerStop(TIMEOUTTIMER, 0) == pdFAIL)
		MESSAGE("Failure to stop the timeout time, apbfsmio.c:399\n\r");
}

#else

apb_ledstate_t LEDState = LED_OFF;
apb_vibstate_t VibState = VIB_OFF;
apb_audiostate_t AudioState = A_OFF;
apb_phasecall_t CallTrap = P_NOCALL;
int LEDFlag = 0;
int VibFlag = 0;
int AudioFlag = 0;
int CallFlag = 0;

void GetConfig(void){ /* Do nothing for testing */
}

void GetStatus(void){ /* Do nothing for testing */
}

void SendCall( apb_phasecall_t call ){
  CallTrap = call;
  CallFlag = 1;
}

void SetLed( apb_ledstate_t led ){
  LEDState = led;
  LEDFlag = 1;
}

void SetVib( apb_vibstate_t vib ){
  VibState = vib;
  VibFlag = 1;
}

void SetAudio( apb_audiostate_t audio ){
  AudioState = audio;
  AudioFlag = 1;
}

int GetStationID(void){
  return 1;
}

void CreateVariableTimer(){
}

void CreateTimeoutTimer(){
}

void RestartVariableTimer(apb_variabletimer_t timer){
}

void RestartTimeoutTimer(){
}

void StopVariableTimer(){
}

void StopTimeoutTimer(){
}

void VariableTimer(void){
}

void TimeoutTimer(void){
}

#endif /* FSMTesting */
