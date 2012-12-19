/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	File:     apbio.h
*	Engineer: Ben Sprague
*	Date:	  31 March 2011 	
*	Description:	Contains function prototypes and external variables used
*                 for the I/O functionality of the APB state machine
*	Revisions
*
******************************************************************************/

#ifndef __APBIO_H__
#define __APBIO_H__

#include "apbtypes.h"

#undef FSM_LEDONWALK


//#define FSMTesting
#ifndef FSMTesting

//#include "audio.h"
//#include <RTOS.h>
#include "apbconfig.h"
#include "apbio.h"
//#include "APB_snmp.h"
//#include "tasks.h"

#define OFF             0x0000
#define DEFAULT         0x0001
#define APS             0x0002
#define IDENT			0x0004
#define TARGET	0x01
#define PING_PONG 0x02
#define RECALL_OFF   0x00
#define RECALL_PED   0x01
#define RECALL_APS   0x02

#else

/* Global output variables for fsm testing */
extern apb_ledstate_t LEDState;
extern apb_vibstate_t VibState;
extern apb_audiostate_t AudioState;
extern apb_phasecall_t CallTrap;
extern int LEDFlag;
extern int VibFlag;
extern int AudioFlag;
extern int CallFlag;

#endif

#define B_INIT 0
#define B_DEST 1

#ifdef __cplusplus
extern "C" {
#endif

void GetConfig(void);
void GetStatus(void);
void SendCall( apb_phasecall_t call );
void SetLed( apb_ledstate_t led );
void SetVib( apb_vibstate_t vib );
void SetAudio( apb_audiostate_t audio );
int GetStationID(void);

void VariableTimer(xTimerHandle);
void TimeoutTimer(xTimerHandle);

void CreateVariableTimer(void);
void CreateTimeoutTimer(void);
void RestartVariableTimer(apb_variabletimer_t timer);
void RestartTimeoutTimer(void);
void StopVariableTimer(void);
void StopTimeoutTimer(void);

#ifdef __cplusplus
}
#endif

// External variables. Read these for next state logic.
extern apb_phasestate_t PhaseState;
extern apb_phasecall_t PhaseCall;
extern apb_buttonstate_t ButtonState; //FSM must reset to "no press" after detected

// External variable for current state of the state machine.
extern apb_fsmstate_t FSMState;
apb_fsmstate_t NextState_COUNTDOWN_MESSAGE(void);	

// External variable for current audio message to played for AUDIO event
extern apb_audiostate_t AudioMessage;

// External variables for current FSM modes.
extern apb_mode_t Mode;
extern apb_beaconmode_t BeacMode;
extern int ExtraPress;
extern int RepeatWaitEnabled;
extern int GroupCallOn;
extern apb_recall_t RecallMode;
extern int VibPulseOn;
extern int AudibleCountdown;

// External variables for timers, counters, and other FSM inputs
extern int VarTimerDone;
extern int Timeout;
extern int MessageComplete;
extern int VariableCounter;
extern int EP_SecondDW;
extern int BeacEnable;
extern int Destination;
extern int Timing;

//FDW time measurement
extern uint32_t t;
extern int FDW_Time;


#endif /* __APBIO_H__ */
