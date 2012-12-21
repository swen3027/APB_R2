/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	File:     apbtypes.h
*	Engineer: Ben Sprague
*	Date:	  31 March 2011 	
*	Description:	Contains enumeration of all types needed for the states
*                 of the APB state machine.
*
*	Revisions
*
******************************************************************************/

#include <stdint.h>

#ifndef __APBTYPES_H__
#define __APBTYPES_H__

// Enumeration of the possible fsm modes.
//Changed mode_t to apb_mode_t
typedef enum apb_mode_t {
   M_OFF = 0,
   M_DEFAULT ,
   M_EPAPS ,
   M_IDENT
} apb_mode_t;

// Enumeration of the possible beaconing modes.
typedef enum apb_beaconmode_t {
   M_NOBEAC = 0 ,
   M_TARGET ,
   M_PINGPONG
} apb_beaconmode_t;

// Enumeration of the possible recall modes.
typedef enum apb_recall_t {
   R_OFF = 0,
   R_PED,
   R_APS
} apb_recall_t;

//Main states
enum {
  MS_INIT = 1,
  MS_IDLE,
  MS_PHASECALL,
  MS_SENDCALL,
  MS_WAIT,
  MS_WALK,
  MS_SENDAPS,
  MS_LOCATION,
  MS_APSWALK,
  MS_FDW,
  MS_DESTWAIT,
  MS_DESTWALK,
  MS_DESTFDW,
  MS_OFF,
  MS_IDENT,
  MS_PREEMPT,
  MS_COUNTDOWN
};

//INIT substates
enum {
  INIT = 1
};

//IDLE substates
enum {
  IDLE_INIT = 1,
  IDLE_MAIN,
  IDLE_TONE
};

//PHASECALL substates
enum {
  PHASECALL_MAIN = 1,
  PHASECALL_TONE
};

//SENDCALL substates
enum {
  SENDCALL_SEND = 1,
  SENDCALL_MAIN
};

//WAIT substates
enum {
  WAIT_INIT = 1,
  WAIT_MESSAGE,
  WAIT_VIBPULSE,
  WAIT_PLAYING,
  WAIT_IDLEREPEATED,
  WAIT_TONE,
  WAIT_IDLETONE
};

//WALK substates
enum {
  WALK_INIT = 1,
  WALK_MESSAGE,
  WALK_PLAYING
};

//SENDAPS substates
enum {
  SENDAPS_SEND = 1,
  SENDAPS_MAIN
};

//LOCATION substates
enum {
  LOCATION_INIT = 1,
  LOCATION_MESSAGE,
  LOCATION_VIBPULSE,
  LOCATION_PLAYING,
  LOCATION_IDLEREPEATED,
  LOCATION_TONE,
  LOCATION_IDLETONE
};

//APSWALK substates
enum {
  APSWALK_INIT = 1,
  APSWALK_MESSAGE,
  APSWALK_PLAYING
};

//FDW substates
enum {
  FDW_INIT = 1,
  FDW_MAIN,
  FDW_TONE,
  FDW_IBEAC,
  FDW_WAIT
};

//DESTWAIT substates
enum {
  DESTWAIT_MAIN = 1,
  DESTWAIT_TONE
};

//DESTWALK substates
enum {
  DESTWALK_INIT = 1,
  DESTWALK_MAIN,
  DESTWALK_TONE
};

//DESTFDW substates
enum {
  DESTFDW_INIT = 1,
  DESTFDW_MAIN,
  DESTFDW_DBEAC,
  DESTFDW_WAIT
};

//OFF substates
enum {
  OFF_MAIN = 1,
  OFF_SEND,
  OFF_DELAY
};

//IDENT substates
enum {
  IDENT_TURNON = 1,
  IDENT_ON,
  IDENT_TURNOFF,
  IDENT_MAIN
};

//PREEMPT substates
enum {
  PREEMPT_MESSAGE = 1,
  PREEMPT_PLAYING
};

//COUNTDOWN substates
enum {
  COUNTDOWN_INIT = 1,
  COUNTDOWN_MAIN,
  COUNTDOWN_MESSAGE
};

//Make state with main state and substate
#define MAKESTATE(x,y) ( ( ( x ) << 24 ) | ( y ) )

#define S_INIT MAKESTATE(MS_INIT,INIT)	

#define S_IDLE_INIT MAKESTATE(MS_IDLE,IDLE_INIT)
#define S_IDLE_MAIN MAKESTATE(MS_IDLE,IDLE_MAIN)	
#define S_IDLE_TONE MAKESTATE(MS_IDLE,IDLE_TONE)

#define S_PHASECALL_MAIN MAKESTATE(MS_PHASECALL,PHASECALL_MAIN)
#define S_PHASECALL_TONE MAKESTATE(MS_PHASECALL,PHASECALL_TONE)

#define S_SENDCALL_SEND MAKESTATE(MS_SENDCALL,SENDCALL_SEND)
#define S_SENDCALL_MAIN MAKESTATE(MS_SENDCALL,SENDCALL_MAIN)

#define S_WAIT_INIT MAKESTATE(MS_WAIT,WAIT_INIT)
#define S_WAIT_MESSAGE MAKESTATE(MS_WAIT,WAIT_MESSAGE)
#define S_WAIT_VIBPULSE MAKESTATE(MS_WAIT,WAIT_VIBPULSE)
#define S_WAIT_PLAYING MAKESTATE(MS_WAIT,WAIT_PLAYING)	
#define S_WAIT_IDLEREPEATED MAKESTATE(MS_WAIT,WAIT_IDLEREPEATED)
#define S_WAIT_TONE MAKESTATE(MS_WAIT,WAIT_TONE)	
#define S_WAIT_IDLETONE MAKESTATE(MS_WAIT,WAIT_IDLETONE)

#define S_WALK_INIT MAKESTATE(MS_WALK,WALK_INIT)
#define S_WALK_MESSAGE MAKESTATE(MS_WALK,WALK_MESSAGE)
#define S_WALK_PLAYING MAKESTATE(MS_WALK,WALK_PLAYING)

#define S_SENDAPS_SEND MAKESTATE(MS_SENDAPS,SENDAPS_SEND)
#define S_SENDAPS_MAIN MAKESTATE(MS_SENDAPS,SENDAPS_MAIN)

#define S_LOCATION_INIT MAKESTATE(MS_LOCATION,LOCATION_INIT)
#define S_LOCATION_MESSAGE MAKESTATE(MS_LOCATION,LOCATION_MESSAGE)
#define S_LOCATION_VIBPULSE MAKESTATE(MS_LOCATION,LOCATION_VIBPULSE)
#define S_LOCATION_PLAYING MAKESTATE(MS_LOCATION,LOCATION_PLAYING)
#define S_LOCATION_IDLEREPEATED MAKESTATE(MS_LOCATION,LOCATION_IDLEREPEATED)
#define S_LOCATION_TONE MAKESTATE(MS_LOCATION,LOCATION_TONE)
#define S_LOCATION_IDLETONE MAKESTATE(MS_LOCATION,LOCATION_IDLETONE)

#define S_APSWALK_INIT MAKESTATE(MS_APSWALK,APSWALK_INIT)
#define S_APSWALK_MESSAGE MAKESTATE(MS_APSWALK,APSWALK_MESSAGE)
#define S_APSWALK_PLAYING MAKESTATE(MS_APSWALK,APSWALK_PLAYING)

#define S_FDW_INIT MAKESTATE(MS_FDW,FDW_INIT)
#define S_FDW_MAIN MAKESTATE(MS_FDW,FDW_MAIN)
#define S_FDW_TONE MAKESTATE(MS_FDW,FDW_TONE)
#define S_FDW_IBEAC MAKESTATE(MS_FDW,FDW_IBEAC)
#define S_FDW_WAIT MAKESTATE(MS_FDW,FDW_WAIT)

#define S_DESTWAIT_MAIN MAKESTATE(MS_DESTWAIT,DESTWAIT_MAIN)
#define S_DESTWAIT_TONE MAKESTATE(MS_DESTWAIT,DESTWAIT_TONE)

#define S_DESTWALK_INIT MAKESTATE(MS_DESTWALK,DESTWALK_INIT)
#define S_DESTWALK_MAIN MAKESTATE(MS_DESTWALK,DESTWALK_MAIN)
#define S_DESTWALK_TONE MAKESTATE(MS_DESTWALK,DESTWALK_TONE)

#define S_DESTFDW_INIT MAKESTATE(MS_DESTFDW,DESTFDW_INIT)
#define S_DESTFDW_MAIN MAKESTATE(MS_DESTFDW,DESTFDW_MAIN)
#define S_DESTFDW_DBEAC MAKESTATE(MS_DESTFDW,DESTFDW_DBEAC)
#define S_DESTFDW_WAIT MAKESTATE(MS_DESTFDW,DESTFDW_WAIT)

#define S_OFF_MAIN MAKESTATE(MS_OFF,OFF_MAIN)
#define S_OFF_SEND MAKESTATE(MS_OFF,OFF_SEND)
#define S_OFF_DELAY MAKESTATE(MS_OFF,OFF_DELAY)

#define S_IDENT_TURNON MAKESTATE(MS_IDENT,IDENT_TURNON)
#define S_IDENT_ON MAKESTATE(MS_IDENT,IDENT_ON)
#define S_IDENT_TURNOFF MAKESTATE(MS_IDENT,IDENT_TURNOFF)
#define S_IDENT_MAIN MAKESTATE(MS_IDENT,IDENT_MAIN)

#define S_PREEMPT_MESSAGE MAKESTATE(MS_PREEMPT,PREEMPT_MESSAGE)
#define S_PREEMPT_PLAYING MAKESTATE(MS_PREEMPT,PREEMPT_PLAYING)

#define S_COUNTDOWN_INIT MAKESTATE(MS_COUNTDOWN,COUNTDOWN_INIT)
#define S_COUNTDOWN_MAIN MAKESTATE(MS_COUNTDOWN,COUNTDOWN_MAIN)
#define S_COUNTDOWN_MESSAGE MAKESTATE(MS_COUNTDOWN,COUNTDOWN_MESSAGE)

//Masks used for identifying main/substates
#define MAINSTATE(x) ( ( ( x ) & 0xFF000000 ) >> 24)
#define SUBSTATE(x) ( ( x ) & 0x00FFFFFF )

// FSM State type
typedef int apb_fsmstate_t;

// Enumeration of the possible phase states.
typedef enum apb_phasestate_t {
   P_WALK = 0 ,
   P_DONTWALK ,
   P_FLASHINGDW ,
   P_NOTCONNECTED ,
   P_PREEMPT ,
   P_UNKNOWN
} apb_phasestate_t;

// Enumeration of possible phase call types.
typedef enum apb_phasecall_t {
   P_NOCALL = 0 ,
   P_APSCALL ,
   P_CALL ,
   P_EXTERNALCLL
} apb_phasecall_t;

// Enumeration of possible button states.
typedef enum apb_buttonstate_t {
   B_NOPRESS = 0 ,
   B_REGPRESS ,
   B_LONGPRESS
} apb_buttonstate_t;

// Enumeration of possible LED states.
typedef enum apb_ledstate_t {
   LED_ON = 0 ,
   LED_OFF
} apb_ledstate_t;

// Enumeration of possible vibration states.
typedef enum apb_vibstate_t {
   VIB_ON = 0 ,
   VIB_OFF
} apb_vibstate_t;

// Enumeration of possible audio states.
typedef enum apb_audiostate_t {
   A_LOCATOR = 0 ,
   A_WAIT ,
   A_WALK ,
   A_LOCATION ,
   A_DBEACON ,
   A_IBEACON ,
   A_PREEMPT ,
   A_COUNTDOWN ,
   A_OFF
} apb_audiostate_t;

// Enumeration of timers.
typedef enum apb_variabletimer_t {
   T_ONESEC = 0 ,
   T_HALFSEC ,
   T_MS200 ,
   T_MS100 ,
   T_WAIT
} apb_variabletimer_t;

#endif /* __APBTYPES_H__ */
