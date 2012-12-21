/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	File:     apbfsm.h
*	Engineer: Ben Sprague
*	Date:	  31 March 2011 	
*	Description:	Contains external variables and function prototypes used by
*                  apbfsm.c (APB state machine)
*	Revisions
*					Adding function prototypes previously in apbfsmDefault.c
*
******************************************************************************/

#ifndef __APBFSM_H__
#define __APBFSM_H__

#include "apbtypes.h"
#include "apbfsmio.h"

#define PINGPONGREST 2 // * 500 ms
#define TARGETREST 1 // * 500 ms

// Function prototypes

#ifdef __cplusplus
extern "C" {
#endif

//Main state machine loop - get intersection status and go to next state
void fsm_main(void);

void fsm_reset( void );

// Go to the next state - switch statement for main states
apb_fsmstate_t GoNextState( void );

// Next state functions for each main state - switch statements for substates
apb_fsmstate_t NextState_IDLE(void);
apb_fsmstate_t NextState_PHASECALL(void);
apb_fsmstate_t NextState_SENDCALL(void);
apb_fsmstate_t NextState_WAIT(void);
apb_fsmstate_t NextState_WALK(void);
apb_fsmstate_t NextState_SENDAPS(void);
apb_fsmstate_t NextState_LOCATION(void);
apb_fsmstate_t NextState_APSWALK(void);
apb_fsmstate_t NextState_FDW(void);
apb_fsmstate_t NextState_DESTWAIT(void);
apb_fsmstate_t NextState_DESTWALK(void);
apb_fsmstate_t NextState_DESTFDW(void);
apb_fsmstate_t NextState_OFF(void);
apb_fsmstate_t NextState_IDENT(void);
apb_fsmstate_t NextState_PREEMPT(void);
apb_fsmstate_t NextState_COUNTDOWN(void);

// Next state functions for each substate - next state and output logic
apb_fsmstate_t NextState_INIT(void);

apb_fsmstate_t NextState_IDLE_INIT(void);	
apb_fsmstate_t NextState_IDLE_MAIN(void);	
apb_fsmstate_t NextState_IDLE_TONE(void);	

apb_fsmstate_t NextState_PHASECALL_MAIN(void);
apb_fsmstate_t NextState_PHASECALL_TONE(void);

apb_fsmstate_t NextState_SENDCALL_SEND(void);
apb_fsmstate_t NextState_SENDCALL_MAIN(void);

apb_fsmstate_t NextState_WAIT_INIT(void);
apb_fsmstate_t NextState_WAIT_MESSAGE(void);
apb_fsmstate_t NextState_WAIT_VIBPULSE(void);
apb_fsmstate_t NextState_WAIT_PLAYING(void);
apb_fsmstate_t NextState_WAIT_IDLEREPEATED(void);
apb_fsmstate_t NextState_WAIT_TONE(void);
apb_fsmstate_t NextState_WAIT_IDLETONE(void);

apb_fsmstate_t NextState_WALK_INIT(void);	
apb_fsmstate_t NextState_WALK_MESSAGE(void);	
apb_fsmstate_t NextState_WALK_PLAYING(void);	

apb_fsmstate_t NextState_SENDAPS_SEND(void);	
apb_fsmstate_t NextState_SENDAPS_MAIN(void);

apb_fsmstate_t NextState_LOCATION_INIT(void);
apb_fsmstate_t NextState_LOCATION_MESSAGE(void);
apb_fsmstate_t NextState_LOCATION_VIBPULSE(void);
apb_fsmstate_t NextState_LOCATION_PLAYING(void);
apb_fsmstate_t NextState_LOCATION_IDLEREPEATED(void);
apb_fsmstate_t NextState_LOCATION_TONE(void);
apb_fsmstate_t NextState_LOCATION_IDLETONE(void);

apb_fsmstate_t NextState_APSWALK_INIT(void);	
apb_fsmstate_t NextState_APSWALK_MESSAGE(void);	
apb_fsmstate_t NextState_APSWALK_PLAYING(void);

apb_fsmstate_t NextState_FDW_INIT(void);	
apb_fsmstate_t NextState_FDW_MAIN(void);	
apb_fsmstate_t NextState_FDW_TONE(void);	
apb_fsmstate_t NextState_FDW_IBEAC(void);	
apb_fsmstate_t NextState_FDW_WAIT(void);

apb_fsmstate_t NextState_DESTWAIT_MAIN(void);	
apb_fsmstate_t NextState_DESTWAIT_TONE(void);	

apb_fsmstate_t NextState_DESTWALK_INIT(void);	
apb_fsmstate_t NextState_DESTWALK_MAIN(void);	
apb_fsmstate_t NextState_DESTWALK_TONE(void);	

apb_fsmstate_t NextState_DESTFDW_INIT(void);
apb_fsmstate_t NextState_DESTFDW_MAIN(void);	
apb_fsmstate_t NextState_DESTFDW_DBEAC(void);	
apb_fsmstate_t NextState_DESTFDW_WAIT(void);

apb_fsmstate_t NextState_OFF_MAIN(void);	
apb_fsmstate_t NextState_OFF_SEND(void);	
apb_fsmstate_t NextState_OFF_DELAY(void);	

apb_fsmstate_t NextState_IDENT_TURNON(void);	
apb_fsmstate_t NextState_IDENT_ON(void);	
apb_fsmstate_t NextState_IDENT_TURNOFF(void);	
apb_fsmstate_t NextState_IDENT_MAIN(void);

apb_fsmstate_t NextState_PREEMPT_MESSAGE(void);
apb_fsmstate_t NextState_PREEMPT_PLAYING(void);

apb_fsmstate_t NextState_COUNTDOWN_INIT(void);	
apb_fsmstate_t NextState_COUNTDOWN_MAIN(void);	
apb_fsmstate_t NextState_COUNTDOWN_MESSAGE(void);	

#ifdef __cplusplus
}
#endif

#endif /* __APBFSM_H__ */
