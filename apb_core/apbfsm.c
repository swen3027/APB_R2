/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	File:     apfsm.c
*	Engineer: Ben Sprague
*	Date:	  31 March 2011 	
*	Description:	APB finite state machine.
*	Revisions
*		31 May 2011 - Combining with apbfsmDefault.c (redundant since
*			apbfsmDefault had all modes)
*
******************************************************************************/

#include <stdio.h>
#include "apbfsm.h"

void fsm_reset( void ){
   FSMState = S_INIT;
}

/* START FUNCTION DESCRIPTION ********************************************
fsm_main
DESCRIPTION:	Continuous loop that steps through FSM states and sets the
                  phase state/call based on the station status.
Parameter:		None
RETURN VALUE:  None
Notes:			Prototype in apbfsm.h
END DESCRIPTION ********************************************************* */
void fsm_main(void){
//   CreateVariableTimer();  //Multi-purpose software timer
//	CreateTimeoutTimer();   //Walk Timeout software timer
   while(1){
      GetConfig();   // Get Station Configuration
	   GetStatus();   // Get phase and station status
#ifdef FSM_LEDONWALK
      if(PhaseState == P_WALK)
         APB_SetIO(LED);
      else
         APB_ClrIO(LED);
#endif /* FSM_LEDONWALK */
      FSMState = GoNextState(); // Advance to next states
   }
}

/* START FUNCTION DESCRIPTION ********************************************
GoNextState
DESCRIPTION:	Get the next FSM state depending on the current state.
Parameter:		None
RETURN VALUE:  The value returned by the next state function of the current
                  FSM state.
Notes:			Prototype in apbfsm.h
END DESCRIPTION ********************************************************* */
apb_fsmstate_t GoNextState(void){
   apb_fsmstate_t next_state;
   next_state = S_INIT;

      //Preempt overrides any state
   if((PhaseState == P_PREEMPT) && (MAINSTATE(FSMState) != MS_PREEMPT)){
      return S_PREEMPT_MESSAGE;
   }

	switch(MAINSTATE(FSMState)){
		case MS_INIT:
			next_state = NextState_INIT();
         break;
		case MS_IDLE:
			next_state = NextState_IDLE();
         break;
		case MS_PHASECALL:
			next_state = NextState_PHASECALL();
         break;
		case MS_SENDCALL:
			next_state = NextState_SENDCALL();
         break;
		case MS_WAIT:
			next_state = NextState_WAIT();
         break;
		case MS_WALK:
			next_state = NextState_WALK();
         break;
		case MS_SENDAPS:
			next_state = NextState_SENDAPS();
         break;
		case MS_LOCATION:
			next_state = NextState_LOCATION();
         break;
		case MS_APSWALK:
			next_state = NextState_APSWALK();
         break;
		case MS_FDW:
			next_state = NextState_FDW();
         break;
		case MS_DESTWAIT:
			next_state = NextState_DESTWAIT();
         break;
		case MS_DESTWALK:
			next_state = NextState_DESTWALK();
         break;
		case MS_DESTFDW:
			next_state = NextState_DESTFDW();
         break;
		case MS_OFF:
			next_state = NextState_OFF();
         break;
		case MS_IDENT:
			next_state = NextState_IDENT();
         break;
      case MS_PREEMPT:
         next_state = NextState_PREEMPT();
         break;
      case MS_COUNTDOWN:
         next_state = NextState_COUNTDOWN();
         break;
	}
   return next_state;
}

/* START FUNCTION DESCRIPTION ********************************************
NextState
DESCRIPTION:	Get the next FSM state depending on the current state.
				Each main state is split into substates, contained in the
				following functions.
Parameter:		None
RETURN VALUE:  The value returned by the next state function of the current
                  FSM state.
Notes:			Prototype in apbfsm.h
END DESCRIPTION **********************************************************/


/* NextState_IDLE - Select next state function for a substate of IDLE */
apb_fsmstate_t NextState_IDLE(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case IDLE_INIT:
		next_state = NextState_IDLE_INIT();
      break;
	case IDLE_MAIN:
		next_state = NextState_IDLE_MAIN();
      break;
	case IDLE_TONE:
		next_state = NextState_IDLE_TONE();
      break;
  }
  return next_state;
}

/* NextState_PHASECALL - Select next state function for a substate of PHASECALL */
apb_fsmstate_t NextState_PHASECALL(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case PHASECALL_MAIN:
		next_state = NextState_PHASECALL_MAIN();
      break;
	case PHASECALL_TONE:
		next_state = NextState_PHASECALL_TONE();
      break;
  }
  return next_state;
}
	
/* NextState_SENDCALL - Select next state function for a substate of SENDCALL */
apb_fsmstate_t NextState_SENDCALL(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case SENDCALL_SEND:
		next_state = NextState_SENDCALL_SEND();
      break;
	case SENDCALL_MAIN:
		next_state = NextState_SENDCALL_MAIN();
      break;
  }
  return next_state;
}
	
/* NextState_WAIT - Select next state function for a substate of WAIT */
apb_fsmstate_t NextState_WAIT(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case WAIT_INIT:
		next_state = NextState_WAIT_INIT();
      break;
	case WAIT_MESSAGE:
		next_state = NextState_WAIT_MESSAGE();
      break;
   case WAIT_VIBPULSE:
		next_state = NextState_WAIT_VIBPULSE();
      break;
	case WAIT_PLAYING:
		next_state = NextState_WAIT_PLAYING();
      break;
	case WAIT_IDLEREPEATED:
		next_state = NextState_WAIT_IDLEREPEATED();
      break;
	case WAIT_TONE:
		next_state = NextState_WAIT_TONE();
      break;
	case WAIT_IDLETONE:
		next_state = NextState_WAIT_IDLETONE();
      break;
  }
  return next_state;
}	

/* NextState_WALK - Select next state function for a substate of WALK */
apb_fsmstate_t NextState_WALK(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case WALK_INIT:
		next_state = NextState_WALK_INIT();
      break;
	case WALK_MESSAGE:
		next_state = NextState_WALK_MESSAGE();
      break;
	case WALK_PLAYING:
		next_state = NextState_WALK_PLAYING();
      break;
  }
  return next_state;
}

/* NextState_SENDAPS - Select next state function for a substate of SENDAPS */
apb_fsmstate_t NextState_SENDAPS(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case SENDAPS_SEND:
		next_state = NextState_SENDAPS_SEND();
      break;
	case SENDAPS_MAIN:
		next_state = NextState_SENDAPS_MAIN();
      break;
  }
  return next_state;
}		
	
/* NextState_LOCATION - Select next state function for a substate of LOCATION */
apb_fsmstate_t NextState_LOCATION(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case LOCATION_INIT:
		next_state = NextState_LOCATION_INIT();
      break;
   case LOCATION_MESSAGE:
		next_state = NextState_LOCATION_MESSAGE();
      break;
   case LOCATION_VIBPULSE:
      next_state = NextState_LOCATION_VIBPULSE();
      break;
	case LOCATION_PLAYING:
		next_state = NextState_LOCATION_PLAYING();
      break;
	case LOCATION_IDLEREPEATED:
		next_state = NextState_LOCATION_IDLEREPEATED();
      break;
	case LOCATION_TONE:
		next_state = NextState_LOCATION_TONE();
      break;
	case LOCATION_IDLETONE:
		next_state = NextState_LOCATION_IDLETONE();
      break;
  }
  return next_state;
}	

/* NextState_APSWALK - Select next state function for a substate of APSWALK */
apb_fsmstate_t NextState_APSWALK(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case APSWALK_INIT:
		next_state = NextState_APSWALK_INIT();
      break;
	case APSWALK_MESSAGE:
		next_state = NextState_APSWALK_MESSAGE();
      break;
	case APSWALK_PLAYING:
		next_state = NextState_APSWALK_PLAYING();
      break;
  }
  return next_state;
}	
	
/* NextState_FDW - Select next state function for a substate of FDW */
apb_fsmstate_t NextState_FDW(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case FDW_INIT:
		next_state = NextState_FDW_INIT();	
      break;
	case FDW_MAIN:
		next_state = NextState_FDW_MAIN();	
      break;
	case FDW_TONE:
		next_state = NextState_FDW_TONE();
      break;
	case FDW_IBEAC:
		next_state = NextState_FDW_IBEAC();	
      break;
	case FDW_WAIT:
      next_state = NextState_FDW_WAIT();
      break;
  }
  return next_state;
}		

/* NextState_DESTWAIT - Select next state function for a substate of DESTWAIT */
apb_fsmstate_t NextState_DESTWAIT(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case DESTWAIT_MAIN:
		next_state = NextState_DESTWAIT_MAIN();	
      break;
	case DESTWAIT_TONE:
		next_state = NextState_DESTWAIT_TONE();
      break;
  }
  return next_state;
}	

/* NextState_DESTWALK - Select next state function for a substate of DESTWALK */
apb_fsmstate_t NextState_DESTWALK(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case DESTWALK_INIT:
		next_state = NextState_DESTWALK_INIT();
      break;
	case DESTWALK_MAIN:
		next_state = NextState_DESTWALK_MAIN();
      break;
	case DESTWALK_TONE:
		next_state = NextState_DESTWALK_TONE();	
      break;
  }
  return next_state;
}			

/* NextState_DESTFDW - Select next state function for a substate of DESTFDW */
apb_fsmstate_t NextState_DESTFDW(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case DESTFDW_INIT:
		next_state = NextState_DESTFDW_INIT();	
      break;
	case DESTFDW_MAIN:
		next_state = NextState_DESTFDW_MAIN();	
      break;
	case DESTFDW_DBEAC:
		next_state = NextState_DESTFDW_DBEAC();	
      break;
	case DESTFDW_WAIT:
      next_state = NextState_DESTFDW_WAIT();
      break;
  }
  return next_state;
}		

/* NextState_OFF - Select next state function for a substate of OFF */
apb_fsmstate_t NextState_OFF(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case OFF_MAIN:
		next_state = NextState_OFF_MAIN();
      break;
	case OFF_SEND:
		next_state = NextState_OFF_SEND();
      break;
	case OFF_DELAY:
		next_state = NextState_OFF_DELAY();
      break;
  }
  return next_state;
}	

/* NextState_IDENT - Select next state function for a substate of IDENT */
apb_fsmstate_t NextState_IDENT(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case IDENT_TURNON:
		next_state = NextState_IDENT_TURNON();
      break;
	case IDENT_ON:
		next_state = NextState_IDENT_ON();
      break;
	case IDENT_TURNOFF:
		next_state = NextState_IDENT_TURNOFF();
      break;
	case IDENT_MAIN:
		next_state = NextState_IDENT_MAIN();
      break;
  }
  return next_state;
}	
	
/* NextState_PREEMPT - Select next state function for a substate of PREEMPT */
apb_fsmstate_t NextState_PREEMPT(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case PREEMPT_MESSAGE:
		next_state = NextState_PREEMPT_MESSAGE();
      break;
	case PREEMPT_PLAYING:
		next_state = NextState_PREEMPT_PLAYING();
      break;
  }
  return next_state;
}	
	
/* NextState_COUNTDOWN - Select next state function for a substate of COUNTDOWN */
apb_fsmstate_t NextState_COUNTDOWN(void){
  apb_fsmstate_t next_state;
  next_state = S_INIT;

  switch(SUBSTATE(FSMState)){
	case COUNTDOWN_INIT:
		next_state = NextState_COUNTDOWN_INIT();
      break;
	case COUNTDOWN_MAIN:
		next_state = NextState_COUNTDOWN_MAIN();
      break;
   case COUNTDOWN_MESSAGE:
		next_state = NextState_COUNTDOWN_MESSAGE();
      break;
  }
  return next_state;
}	

/* INIT State *************************************************************
Initializes the FSM
Substates:	S_INIT		
END DESCRIPTION **********************************************************/
apb_fsmstate_t NextState_INIT(void){

	return S_IDLE_INIT;
}

/* IDLE State *************************************************************
Next state and output logic for the IDLE state.
Substates: 	S_IDLE_INIT
			S_IDLE_MAIN
			S_IDLE_TONE
END DESCRIPTION **********************************************************/

/* S_IDLE_INIT Substate - Set up IDLE State */
apb_fsmstate_t NextState_IDLE_INIT(void){
   SetAudio(A_OFF);                //Clear audio
	EP_SecondDW = 0;        //DW count for Extra Press
   Destination = 0;        //Destination beacon off
	SetLed(LED_OFF);
	SetVib(VIB_OFF);
	RestartVariableTimer(T_ONESEC);  //Start one second timer for locator tone
	return S_IDLE_MAIN;
}

/* S_IDLE_MAIN Substate - Main IDLE subtate */
apb_fsmstate_t NextState_IDLE_MAIN(void){
	if(Mode == M_OFF){      //Off mode - go to off state
		return S_OFF_MAIN;
	}

	if(Mode == M_IDENT){    //Identify mode - go to ident state
		VariableCounter = GetStationID();   //Initialize LED counter with ID num
		return S_IDENT_TURNON;
	}
	
   switch(RecallMode){
      case R_PED:          //Ped Recall - repeatedly send ped calls
         return S_SENDCALL_SEND;
      case R_APS:          //APS Recall - repeatedly send APS calls
         return S_SENDAPS_SEND;
   }

	switch(ButtonState){
		case B_REGPRESS:           //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:          //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	
	switch(PhaseCall){
		case P_CALL:                  //Phase call
         if(GroupCallOn){           //Behave like calling button if Group Call is on
            Destination = 1;        //Used to avoid vib call pulse
            return S_WAIT_INIT;
         }
         else{
            SetLed(LED_ON);
            return S_PHASECALL_MAIN;
         }

		case P_APSCALL:               //Group call
			SetLed(LED_ON);
         Destination = 1;           //Button is destination (used for beaconing)

         if(GroupCallOn){            //Behave like calling button if Group Call is on
            return S_LOCATION_INIT;
         }
			else if(BeacMode == M_NOBEAC){ //No beaconing - treat same as phase call
				return S_PHASECALL_MAIN;
         }
			else{
				return S_DESTWAIT_MAIN;    //Beaconing - begin destination sequence
         }
		default:
         if(VarTimerDone){         //One Second expired - play locator tone
		  		return S_IDLE_TONE;
         }
         else{
				return S_IDLE_MAIN;
         }
	}

}

/* S_IDLE_TONE Substate - Play Locator Tone and go to Main substate */
apb_fsmstate_t NextState_IDLE_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play tone
	return S_IDLE_MAIN;
}

/* PHASECALL State ********************************************************
Next state and output logic for the PHASECALL state.
Substates: 	S_PHASECALL_MAIN
			S_PHASECALL_TONE
END DESCRIPTION **********************************************************/

/* S_PHASECALL_MAIN Substate - Main PHASECALL substate */
apb_fsmstate_t NextState_PHASECALL_MAIN(void){
	switch(ButtonState){
		case B_REGPRESS:                 //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:                //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	
	switch(PhaseState){
		case P_WALK:                     //Button on phase goes back to idle during walk
			return S_IDLE_INIT;
		default:
         if(VarTimerDone){             //One Second expired - play locator tone
				return S_PHASECALL_TONE;
         }
			else{
				return S_PHASECALL_MAIN;
         }
   }
}

/* S_PHASECALL_TONE Substate - Play Locator Tone and go Main substate */
apb_fsmstate_t NextState_PHASECALL_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play tone
	return S_PHASECALL_MAIN;
}

/* SENDCALL State ********************************************************
Next state and output logic for the SENDCALL state.
Substates: 	S_SENDCALL_SEND
			S_SENDCALL_MAIN
END DESCRIPTION **********************************************************/

/* S_SENDCALL_SEND Substate - Send call and return to Main substate */
apb_fsmstate_t NextState_SENDCALL_SEND(void){
	ButtonState = B_NOPRESS;            //Clear button press
	SetLed(LED_OFF);
	SendCall(P_CALL);                   //Send ped call
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for sending calls
	return S_SENDCALL_MAIN;
}

/* S_SENDCALL_MAIN Substate - Main SENDCALL substate */
apb_fsmstate_t NextState_SENDCALL_MAIN(void){
	if(PhaseState == P_WALK){           //Walk before call acknowledgment
		return S_WALK_INIT;
   }
	else if((PhaseCall == P_CALL)||(PhaseCall == P_APSCALL)){ //Call acknowledgement
		return S_WAIT_INIT;
   }
	else if(VarTimerDone){              //One second expired - send ped call
		return S_SENDCALL_SEND;
   }
	else{
		return S_SENDCALL_MAIN;
   }
}

/* WAIT State *************************************************************
Next state and output logic for the WAIT state.
Substates: 	S_WAIT_INIT
			S_WAIT_MESSAGE
         S_WAIT_VIBPULSE
			S_WAIT_PLAYING
			S_WAIT_IDLEREPEATED
			S_WAIT_TONE
			S_WAIT_IDLETONE
END DESCRIPTION **********************************************************/

/* S_WAIT_INIT Substate - Initialize WAIT State */
apb_fsmstate_t NextState_WAIT_INIT(void){
	SetLed(LED_ON);
	if(Mode == M_EPAPS){                //EPAPS mode - no wait message
		if(VarTimerDone){                //One second expired - play locator tone
			return S_WAIT_TONE;
      }
		else{
			return S_WAIT_IDLETONE;
      }
	}
	else if(VibPulseOn && !Destination){   //Vibrotactile call pulse option
      SetVib(VIB_ON);                  //Pulse vibrotactile for 100 ms
      RestartVariableTimer(T_MS100);   //Restart 100 ms timer for vib pulse
      MessageComplete = 0;             //Location message not complete
      SetAudio(A_WAIT);                //Play location message
	   return S_WAIT_VIBPULSE;
   }
   else{
	   MessageComplete = 0;                //Wait message not complete
	   SetAudio(A_WAIT);                   //Play message
	   return S_WAIT_PLAYING;
   }
}

/* S_WAIT_MESSAGE Substate - Play Wait Message */
apb_fsmstate_t NextState_WAIT_MESSAGE(void){
   MessageComplete = 0;                //Wait message not complete
	SetAudio(A_WAIT);                   //Play message
	return S_WAIT_PLAYING;
}

/* S_WAIT_VIBPULSE Substate - Play Wait Message and pulse Vib */
apb_fsmstate_t NextState_WAIT_VIBPULSE(void){
   if(VarTimerDone){                   //100 ms done - stop vib pulse
      SetVib(VIB_OFF);
	   return S_WAIT_PLAYING;
   }
   else{
      return S_WAIT_VIBPULSE;
   }
}

/* S_WAIT_PLAYING Substate - Wait Message is playing */
apb_fsmstate_t NextState_WAIT_PLAYING(void){
   //Disabling calls during wait/location messages
   ButtonState = B_NOPRESS;
	switch(ButtonState){
		case B_REGPRESS:
			return S_SENDCALL_SEND;
		case B_LONGPRESS:
			return S_SENDAPS_SEND;
	}
	
	if(PhaseState == P_WALK){           //Walk
		return S_WALK_INIT;
   }
	else if(MessageComplete){           //Wait message complete
		if(RepeatWaitEnabled){           //Repeated wait
			RestartVariableTimer(T_WAIT); //Start wait timer (time between messages)
			return S_WAIT_IDLEREPEATED;
		}
		else{
         return S_WAIT_TONE;           //No repeated wait - play locator tone
      }
	}
	else{
		return S_WAIT_PLAYING;           //No walk - wait message still playing
   }
}

/* S_WAIT_IDLEREPEATED Substate - Pause between repeated Wait Messages */
apb_fsmstate_t NextState_WAIT_IDLEREPEATED(void){
	switch(ButtonState){
		case B_REGPRESS:                 //Regular press - send ped call
			StopVariableTimer();          //Stop wait timer
			return S_SENDCALL_SEND;
		case B_LONGPRESS:                //Extended press - send APS call
			StopVariableTimer();          //Stop wait timer
			return S_SENDAPS_SEND;
	}
	
	if(PhaseState == P_WALK){           //Walk
		StopVariableTimer();             //Stop wait timer
		return S_WALK_INIT;
	}
	else if(VarTimerDone){              //Repeated wait delay expired - play message
		return S_WAIT_MESSAGE;
   }
	else{
		return S_WAIT_IDLEREPEATED;
   }
}

/* S_WAIT_TONE Substate - Play Locator Tone and go to S_WAIT_IDLETONE */
apb_fsmstate_t NextState_WAIT_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play tone
	return S_WAIT_IDLETONE;
}

/* S_WAIT_IDLETONE Substate - Pause between Locator Tones */
apb_fsmstate_t NextState_WAIT_IDLETONE(void){
	switch(ButtonState){
		case B_REGPRESS:                 //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:                //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	
	if(PhaseState == P_WALK){           //Walk
		return S_WALK_INIT;
   }
	else if(VarTimerDone){              //One second expired - play locator tone
		return S_WAIT_TONE;
   }
	else{
		return S_WAIT_IDLETONE;
   }
}

/* WALK State *************************************************************
Next state and output logic for the WALK state.
Substates: 	S_WALK_INIT
			S_WALK_MESSAGE
			S_WALK_PLAYING
END DESCRIPTION **********************************************************/

/* S_WALK_INIT Substate - Initialize WALK State */
apb_fsmstate_t NextState_WALK_INIT(void){
	if(Mode == M_EPAPS){                //EPAPS mode - no audio without extended press
		return S_IDLE_INIT;
   }
	else{
		SetLed(LED_OFF);
		SetVib(VIB_ON);
		RestartTimeoutTimer();           //Start walk timeout timer
		return S_WALK_MESSAGE;
	}	
}

/* S_WALK_MESSAGE Substate - Play Walk Message */
apb_fsmstate_t NextState_WALK_MESSAGE(void){
   MessageComplete = 0;                //Walk message not complete
	SetAudio(A_WALK);                   //Play walk message
	return S_WALK_PLAYING;
}

/* S_WALK_PLAYING Substate - Walk Message playing */
apb_fsmstate_t NextState_WALK_PLAYING(void){
   ButtonState = B_NOPRESS;            //Clear button presses

	if(Timeout){                        //Walk timeout - go back to idle state
		Timeout = 0;
		return S_IDLE_INIT;
	}
	
	switch(PhaseState){
      case P_WALK:                     //Still in walk
         if(MessageComplete){          //Walk message complete - play message again
			   return S_WALK_MESSAGE;
         }
		   else{                         //Walk message still playing
			   return S_WALK_PLAYING;
         }
	   case P_FLASHINGDW:               //FDW
         StopTimeoutTimer();           //Stop walk timeout timer
         if(AudibleCountdown){
            return S_COUNTDOWN_INIT;
         }
         else if(RecallMode == R_PED){      //Ped recall - go to FDW state and send call on DW
            return S_FDW_INIT;
         }
         else{
            return S_IDLE_INIT;
         }
	   default:                         //DW or unknown
		   StopTimeoutTimer();           //Stop walk timeout timer
		   return S_IDLE_INIT;
	}
}

/* SENDAPS State ********************************************************
Next state and output logic for the SENDAPS state.
Substates: 	S_SENDAPS_SEND
			S_SENDAPS_MAIN
END DESCRIPTION **********************************************************/

/* S_SENDAPS_SEND Substate - Send APS call and return to Main substate */
apb_fsmstate_t NextState_SENDAPS_SEND(void){
	ButtonState = B_NOPRESS;            //Clear button presses
	SetLed(LED_OFF);
	SendCall(P_APSCALL);                //Send APS call
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for APS calls
	return S_SENDAPS_MAIN;
}

/* S_SENDAPS_MAIN Substate - Main SENDAPS substate */
apb_fsmstate_t NextState_SENDAPS_MAIN(void){
	if(PhaseState == P_WALK){           //Walk before call acknowledgement
		return S_APSWALK_INIT;
   }
	else if(PhaseCall == P_APSCALL){    //APS call acknowledgement - play location message
		return S_LOCATION_INIT;
   }
	else if(VarTimerDone){              //One second expired - send APS call
		return S_SENDAPS_SEND;
   }
	else{
		return S_SENDAPS_MAIN;
   }
}

/* LOCATION State *********************************************************
Next state and output logic for the LOCATION state.
Substates: 	S_LOCATION_INIT
         S_LOCATION_MESSAGE
         S_LOCATION_VIBPULSE
			S_LOCATION_PLAYING
			S_LOCATION_IDLEREPEATED
			S_LOCATION_TONE
			S_LOCATION_IDLETONE
END DESCRIPTION **********************************************************/

/* S_LOCATION_INIT Substate - Initialize Location State */
apb_fsmstate_t NextState_LOCATION_INIT(void){
	SetLed(LED_ON);
   MessageComplete = 0;                //Location message not complete
   if(VibPulseOn && !Destination){     //Vibrotactile call pulse option
      SetVib(VIB_ON);                  //Pulse vibrotactile for 100 ms
      RestartVariableTimer(T_MS100);   //Restart 100 ms timer for vib pulse
      SetAudio(A_LOCATION);            //Play location message
	   return S_LOCATION_VIBPULSE;
   }
   else{
      SetAudio(A_LOCATION);            //Play location message
      return S_LOCATION_PLAYING;
   }
}

/* S_LOCATION_MESSAGE Substate - Play Location Message */
apb_fsmstate_t NextState_LOCATION_MESSAGE(void){
	MessageComplete = 0;                //Location message not complete
	SetAudio(A_LOCATION);               //Play Location message
	return S_LOCATION_PLAYING;
}

/* S_LOCATION_VIBPULSE Substate - Play Location Message and pulse Vib */
apb_fsmstate_t NextState_LOCATION_VIBPULSE(void){
   if(VarTimerDone){                   //100 ms done - stop vib pulse
      SetVib(VIB_OFF);
	   return S_LOCATION_PLAYING;
   }
   else{
      return S_LOCATION_VIBPULSE;
   }
}

/* S_LOCATION_PLAYING Substate - Location Message is playing */
apb_fsmstate_t NextState_LOCATION_PLAYING(void){
   ButtonState = B_NOPRESS; //Trouble with vib setting off two calls
                           //Disabling calls while message going.
	if((ButtonState == B_REGPRESS)||(ButtonState == B_LONGPRESS)){
		return S_SENDAPS_SEND;
   }
	
	if(PhaseState == P_WALK){           //Walk
		return S_APSWALK_INIT;
   }
	else if(MessageComplete){           //Location message complete
		if(RepeatWaitEnabled){            //Repeated wait enabled
			RestartVariableTimer(T_WAIT); //Start wait timer (time between messages)
			return S_LOCATION_IDLEREPEATED;
		}
		else{                            //No repeated wait - play locator tone
		   return S_LOCATION_TONE;
      }
	}
	else{
		return S_LOCATION_PLAYING;       //Still playing message
   }
}

/* S_LOCATION_IDLEREPEATED Substate - Pause between repeated Location Messages */
apb_fsmstate_t NextState_LOCATION_IDLEREPEATED(void){
	if((ButtonState == B_REGPRESS)||(ButtonState == B_LONGPRESS)){
	                                    //Button press - send APS call
		StopVariableTimer();             //Stop wait timer
		return S_SENDAPS_SEND;
	}
	
	if(PhaseState == P_WALK){           //Walk
		StopVariableTimer();             //Stop wait timer
		return S_APSWALK_INIT;
	}
	else if(VarTimerDone){              //Repeated Wait delay expired - play message again
		return S_LOCATION_MESSAGE;
	}
	else{
		return S_LOCATION_IDLEREPEATED;
   }
}

/* S_LOCATION_TONE Substate - Play Locator Tone and go to S_LOCATION_IDLETONE */
apb_fsmstate_t NextState_LOCATION_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play tone
	return S_LOCATION_IDLETONE;
}

/* S_LOCATION_IDLETONE Substate - Pause between Locator Tones */
apb_fsmstate_t NextState_LOCATION_IDLETONE(void){
                                       //Button press - send APS call
	if((ButtonState == B_REGPRESS)||(ButtonState == B_LONGPRESS)){
		return S_SENDAPS_SEND;
   }
	
	if(PhaseState == P_WALK){           //Walk
		return S_APSWALK_INIT;
   }
	
	else if(VarTimerDone){              //One second done - play locator tone
		return S_LOCATION_TONE;
   }
	else{
		return S_LOCATION_IDLETONE;
   }
}

/* APSWALK State **********************************************************
Next state and output logic for the APSWALK state.
Substates: 	S_APSWALK_INIT
			S_APSWALK_MESSAGE
			S_APSWALK_PLAYING
END DESCRIPTION **********************************************************/

/* S_APSWALK_INIT Substate - Initialize APSWALK State */
apb_fsmstate_t NextState_APSWALK_INIT(void){
	SetLed(LED_OFF);
	SetVib(VIB_ON);
	RestartTimeoutTimer();              //Start walk timeout timer
	return S_APSWALK_MESSAGE;	
}

/* S_APSWALK_MESSAGE Substate - Play Walk Message */
apb_fsmstate_t NextState_APSWALK_MESSAGE(void){
   MessageComplete = 0;                //Walk message not complete
	SetAudio(A_WALK);                   //Play walk message
	return S_APSWALK_PLAYING;
}

/* S_APSWALK_PLAYING Substate - Walk Message playing */
apb_fsmstate_t NextState_APSWALK_PLAYING(void){
   ButtonState = B_NOPRESS;            //Clear button presses

	if(Timeout){                        //Walk timeout - go back to idle
		Timeout = 0;
		return S_IDLE_INIT;
	}
	
	switch(PhaseState){
		case P_WALK:                     //Still in walk
         if(MessageComplete){          //Walk message complete
				return S_APSWALK_MESSAGE;  //Play message again
         }
			else{
				return S_APSWALK_PLAYING;  //Walk message still playing
	      }
		case P_FLASHINGDW:               //FDW
         StopTimeoutTimer();
         if(AudibleCountdown){
            return S_COUNTDOWN_INIT;
         }
         else if((Destination)&&(BeacMode == M_TARGET)){ //Destination button in Target beaconing
            return S_DESTFDW_INIT;                 //Go to FDW state for dest beaconing
         }
		   else if((ExtraPress)||(RecallMode == R_APS)||(BeacMode != M_NOBEAC)){
				   return S_FDW_INIT;      //Go to FDW state if EP, APS recall, or beaconing
         }
         else{
            return S_IDLE_INIT;
         }
      default:
         return S_IDLE_INIT;
   }
}

/* FDW State *************************************************************
Next state and output logic for the EXTRAPRESS state.
Substates: 	S_FDW_INIT
			S_FDW_MAIN
			S_FDW_TONE
			S_FDW_IBEAC
         S_FDW_WAIT
END DESCRIPTION **********************************************************/

/* S_FDW_INIT Substate - Initialize FDW State */	
apb_fsmstate_t NextState_FDW_INIT(void){
   SetAudio(A_OFF);                //Clear audio
	SetVib(VIB_OFF);
   VariableCounter = PINGPONGREST;     //Spacing between beacon tones is configurable
                                       //Counter decrements every 250 ms

	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	return S_FDW_MAIN;
}

/* S_FDW_MAIN Substate - Main FDW substate */	
apb_fsmstate_t NextState_FDW_MAIN(void){
   switch(ButtonState){
		case B_REGPRESS:           //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:          //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	switch(PhaseState){
		case P_FLASHINGDW:               //FDW
			switch(BeacMode){
				case M_NOBEAC:             //No beaconing
               if(VarTimerDone){       //One second expired - play locator tone
						return S_FDW_TONE;
               }
					else{
						return S_FDW_MAIN;
               }
				case M_PINGPONG:           //Ping pong beaconing
               if(BeacEnable == B_INIT){ //Sync - Dest waits while Init plays
                  VariableCounter--;	//250ms has elapsed,decrement rest time
					   return S_FDW_WAIT;
               }
					else{
						return S_FDW_MAIN;
               }
				default:                   //Target beaconing - do nothing at init
					return S_FDW_MAIN;
			}
		case P_DONTWALK:                 //DW
         switch(RecallMode){
            case R_PED:                //Ped recall - send ped call
               return S_SENDCALL_SEND;
            case R_APS:                //APS recall - send APS call
               return S_SENDAPS_SEND;
            default:                   //No recall
               if((ExtraPress)&&(!EP_SecondDW)){ //First DW in Extra Press
				      EP_SecondDW = 1;     //Seen DW
				      return S_SENDAPS_SEND;  //Send second APS call
			      }
			      else{                    //Second DW or no Extra Press
				      return S_IDLE_INIT;
               }
         }
		default:
			return S_IDLE_INIT;
	}
}

/* S_FDW_TONE Substate - Play Locator Tone and go to FDW_MAIN */	
apb_fsmstate_t NextState_FDW_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play locator tone
	return S_FDW_MAIN;	
}

/* S_FDW_IBEAC Substate - Play IBeacon Tone and go to S_FDW_MAIN */
apb_fsmstate_t NextState_FDW_IBEAC(void){
   VariableCounter = PINGPONGREST;     //Number of 250ms cycles between tones
	SetAudio(A_IBEACON);		            //Play IBeacon tone
	return S_FDW_MAIN;
}

/* S_FDW_WAIT Substate - Wait for BeacEnable and go to S_FDW_IBEAC */
apb_fsmstate_t NextState_FDW_WAIT(void){
	if(BeacEnable == B_INIT){	         //Synchronization - only one should play
      return S_FDW_WAIT;
   }
   else if(VariableCounter == 0){       //Beacon rest period has elapsed
	   return S_FDW_IBEAC;
   }
   else{							            //Keep resting - main state will decrement counter
      return S_FDW_MAIN;
   }
}

/* DESTWAIT State *********************************************************
Next state and output logic for the DESTWAIT state.
Substates: 	S_DESTWAIT_MAIN
			S_DESTWAIT_TONE
END DESCRIPTION **********************************************************/

/* S_DESTWAIT_MAIN Substate - Main DESTWAIT substate */	
apb_fsmstate_t NextState_DESTWAIT_MAIN(void){
	SetLed(LED_ON);
	
	switch(ButtonState){
		case B_REGPRESS:                 //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:                //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	
	if(PhaseState == P_WALK){           //Walk
		return S_DESTWALK_INIT;
   }
	else if(VarTimerDone){               //One second expired - play locator tone
		return S_DESTWAIT_TONE;
   }
	else{
		return S_DESTWAIT_MAIN;	
   }
}

/* S_DESTWAIT_TONE Substate - Play Locator Tone and go to DESTWAIT_MAIN */	
apb_fsmstate_t NextState_DESTWAIT_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play locator tone
	return S_DESTWAIT_MAIN;	
}

/* DESTWALK State *********************************************************
Next state and output logic for the DESTWALK state.
Substates: 	S_DESTWALK_INIT
			S_DESTWALK_MAIN
			S_DESTWALK_TONE
END DESCRIPTION **********************************************************/

/* S_DESTWALK_INIT Substate - Initialize DESTWALK State */
apb_fsmstate_t NextState_DESTWALK_INIT(void){
	SetLed(LED_OFF);
	RestartTimeoutTimer();              //Start walk timeout timer
	return S_DESTWALK_MAIN;	
}

/* S_DESTWALK_MAIN Substate - Main DESTWALK substate */	
apb_fsmstate_t NextState_DESTWALK_MAIN(void){
	switch(ButtonState){
      case B_REGPRESS:
         return S_SENDCALL_SEND;
      case B_LONGPRESS:
         return S_SENDAPS_SEND;
   }

	if(Timeout){                        //Walk timeout - go back to idle
		Timeout = 0;
		return S_IDLE_INIT;
	}
	
	switch(PhaseState){
		case P_FLASHINGDW:               //FDW
			StopTimeoutTimer();
         return S_DESTFDW_INIT;
		case P_WALK:                     //Walk
         if(VarTimerDone){              //One second done - play locator tone
				return S_DESTWALK_TONE;
         }
			else{
				return S_DESTWALK_MAIN;	
         }
		default:
			StopTimeoutTimer();
			return S_IDLE_INIT;
	}
}

/* S_DESTWALK_TONE Substate - Play Locator Tone and go to DESTWALK_MAIN */	
apb_fsmstate_t NextState_DESTWALK_TONE(void){
	RestartVariableTimer(T_ONESEC);     //Restart one second timer for locator tone
	SetAudio(A_LOCATOR);                //Play locator tone
	return S_DESTWALK_MAIN;	
}

/* DESTFDW State *********************************************************
Next state and output logic for the DESTFDW state.
Substates:  S_DESTFDW_INIT 	
         S_DESTFDW_MAIN
			S_DESTFDW_DBEAC
         S_DESTFDW_WAIT
END DESCRIPTION **********************************************************/

/* S_DESTFDW_INIT Substate - Initialize DESTFDW State */
apb_fsmstate_t NextState_DESTFDW_INIT(void){
   SetAudio(A_OFF);                //Clear audio
   SetVib(VIB_OFF);
		//Set rest period between beacon tones
   if(BeacMode == M_TARGET){           //Target beaconing - use target rest time
      VariableCounter = TARGETREST;
   }
   else{                               //Ping pong beaconing - use ping pong rest time
      VariableCounter = PINGPONGREST;
   }
   return S_DESTFDW_MAIN;
}
/* S_DESTFDW_MAIN Substate - Main DESTFDW State */
apb_fsmstate_t NextState_DESTFDW_MAIN(void){
   switch(ButtonState){
		case B_REGPRESS:           //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:          //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
	switch(PhaseState){
		case P_FLASHINGDW:               //Still in FDW
         if(BeacEnable == B_DEST){     //Sync - Dest waits while Init plays
            VariableCounter--;	      //250ms has elapsed,decrement rest time
				return S_DESTFDW_WAIT;
         }
			else{
				return S_DESTFDW_MAIN;	
         }
		default:
			return S_IDLE_INIT;           //FDW done - go back to idle
   }
}

/* S_DESTFDW_DBEAC Substate - Play DBeacon Tone and go to S_DESTFDW_MAIN */
apb_fsmstate_t NextState_DESTFDW_DBEAC(void){
		//Reset rest time
   if(BeacMode == M_TARGET){
      VariableCounter = TARGETREST;
   }
   else{
      VariableCounter = PINGPONGREST;
   }
	SetAudio(A_DBEACON);	               //Play DBeacon tone
	return S_DESTFDW_MAIN;
}

/* S_DESTFDW_WAIT Substate - Wait for sync and go to S_DESTFDW_DBEAC */
apb_fsmstate_t NextState_DESTFDW_WAIT(void){
	if(BeacEnable == B_DEST){	         //Synchronization
	   return S_DESTFDW_WAIT;
   }
   else if(VariableCounter == 0){      //Rest period has elapsed
      return S_DESTFDW_DBEAC;
   }
   else{							            //Rest period has not elapsed
      return S_DESTFDW_MAIN;
   }
}

/* OFF State **************************************************************
Next state and output logic for the OFF state (OFF Mode).
Substates: 	S_OFF_MAIN
			S_OFF_SEND
			S_OFF_DELAY
END DESCRIPTION **********************************************************/

/* S_OFF_MAIN Substate - Main OFF substate */
apb_fsmstate_t NextState_OFF_MAIN(void){
	if(Mode != M_OFF){                  //Mode changed - go back to idle
      return S_IDLE_INIT;
   }
	else if(ButtonState == B_NOPRESS){  //No press - do nothing
		return S_OFF_MAIN;
   }
	else{                               //Button press - light LED and send ped call
		ButtonState = B_NOPRESS;
		return S_OFF_SEND;
	}
}

/* S_OFF_SEND Substate - Send call, LED on, go to S_OFF_DELAY */
apb_fsmstate_t NextState_OFF_SEND(void){
	SendCall(P_CALL);                   //Send ped call
	SetLed(LED_ON);
	
   if(VibPulseOn){                     //Vibrotactile pulse for call
		SetVib(VIB_ON);
   }

	RestartVariableTimer(T_MS100);      //Restart 100 ms timer for LED ON delay
	return S_OFF_DELAY;
}

/* S_OFF_DELAY Substate - LED off and go to S_OFF_MAIN after 0.5 sec */
apb_fsmstate_t NextState_OFF_DELAY(void){
	if(VarTimerDone){                   //100 ms done
		SetLed(LED_OFF);                 //Turn LED back off
		SetVib(VIB_OFF);
      ButtonState = B_NOPRESS;         //Clear button presses
		return S_OFF_MAIN;
	}
	else{
		return S_OFF_DELAY;              //Keep delaying
   }
}

/* IDENT State ************************************************************
Next state and output logic for the IDENT state.
Substates: 	S_IDENT_TURNON
			S_IDENT_ON
			S_IDENT_TURNOFF
			S_IDENT_OFF
			S_IDENT_MAIN
END DESCRIPTION **********************************************************/

/* S_IDENT_TURNON Substate - Turn LED on, go to S_IDENT_ON */	
apb_fsmstate_t NextState_IDENT_TURNON(void){
	SetLed(LED_ON);                     //Turn LED on
	VariableCounter--;                  //Decrement counter - keeps track of LED flashes
	RestartVariableTimer(T_MS200);      //Restart 200 millisecond timer for LED
	return S_IDENT_ON;
}

/* S_IDENT_ON Substate - Go to S_IDENT_TURNOFF after 200 ms */	
apb_fsmstate_t NextState_IDENT_ON(void){
	if(VarTimerDone){                   //200 ms done - turn LED off
		return S_IDENT_TURNOFF;
	}
	else{
		return S_IDENT_ON;               //Keep delaying
   }
}

/* S_IDENT_TURNOFF Substate - Turn LED off, go to S_IDENT_OFF if not done with
						the VariableCounter cycles, go to S_IDENT_MAIN if done */	
apb_fsmstate_t NextState_IDENT_TURNOFF(void){
	SetLed(LED_OFF);
	if(VariableCounter > 0){            //Still in LED cycle
		RestartVariableTimer(T_MS200);   //Restart 200 ms timer for LED
   }
	else{                               //LED cycle done
		RestartVariableTimer(T_ONESEC);  //Start one second timer for LED OFF delay
      VariableCounter = GetStationID();   //Reset LED flash counter to station ID
   }
   return S_IDENT_MAIN;
}

/* S_IDENT_MAIN Substate - Start LED cycle after one second */	
apb_fsmstate_t NextState_IDENT_MAIN(void){
	if(Mode != M_IDENT){                //Mode changed - go back to idle
      return S_IDLE_INIT;
   }
	else if(VarTimerDone){              //200 ms if Ident cycle running, 1 sec if done
		return S_IDENT_TURNON;           //Flash LED
   }
	else{
		return S_IDENT_MAIN;
   }
}

/* PREEMPT State ************************************************************
Next state and output logic for the PREEMPT state.
Substates: 	S_PREEMPT_MESSAGE
            S_PREEMPT_PLAYING
END DESCRIPTION **********************************************************/

/* S_PREEMPT_MESSAGE Substate - Play Preempt Message */	
apb_fsmstate_t NextState_PREEMPT_MESSAGE(void){
   MessageComplete = 0;                //Preempt message not complete
   SetVib(VIB_OFF);
   SetLed(LED_OFF);
   SetAudio(A_PREEMPT);                //Play preempt message
   return S_PREEMPT_PLAYING;
}

/* S_PREEMPT_PLAYING Substate - Playing Preempt Message */	
apb_fsmstate_t NextState_PREEMPT_PLAYING(void){
   if(PhaseState != P_PREEMPT){        //No longer in preemption - go back to idle
      return S_IDLE_INIT;
   }
   else if(MessageComplete){           //Preempt message complete - play again
      return S_PREEMPT_MESSAGE;
   }
   else{
      return S_PREEMPT_PLAYING;        //Preempt message still playing
   }
}

/* S_COUNTDOWN State ************************************************************
Next state and output logic for the COUNTDOWN state.
Substates: 	S_COUNTDOWN_INIT
            S_COUNTDOWN_MAIN
            S_COUNTDOWN_MESSAGE
END DESCRIPTION **********************************************************/

/* S_COUNTDOWN_INIT Substate - Initialize Countdown state */	
apb_fsmstate_t NextState_COUNTDOWN_INIT(void){
   SetAudio(A_OFF);                //Clear audio
   SetVib(VIB_OFF);
   VariableCounter = FDW_Time;         //Set counter to expected time of FlashingDW
   RestartVariableTimer(T_ONESEC);     //Start one sec timer for countdown messages
   return S_COUNTDOWN_MAIN;
}

/* S_COUNTDOWN_MAIN Substate - Main Countdown substate */	
apb_fsmstate_t NextState_COUNTDOWN_MAIN(void){
   switch(ButtonState){
		case B_REGPRESS:           //Regular press - send ped call
			return S_SENDCALL_SEND;
		case B_LONGPRESS:          //Extended press - send APS call
			return S_SENDAPS_SEND;
	}
   if(PhaseState == P_DONTWALK){       //End of FDW - go back to idle
      return S_IDLE_INIT;
   }
   else if(VariableCounter < 0){           //End of countdown - go back to idle
      return S_IDLE_INIT;
   }
   else if(VarTimerDone){              //One sec done - play countdown message
      return S_COUNTDOWN_MESSAGE;
   }
   else{
      return S_COUNTDOWN_MAIN;
   }
}

/* S_COUNTDOWN_MESSAGE Substate - Play a Countdown Message */	
apb_fsmstate_t NextState_COUNTDOWN_MESSAGE(void){
   RestartVariableTimer(T_ONESEC);     //Restart one sec timer for countdown messages
   SetAudio(A_COUNTDOWN);
   VariableCounter--;                  //Decrement countdown counter
   return S_COUNTDOWN_MAIN;
}

