


#ifndef _APBCONFIG_H_
#define _APBCONFIG_H_

#include <stdint.h>

#define DEFAULT_IP 0xc0a8000a

typedef struct {
	int32_t ID;
	int32_t N_mode;
	int32_t D_locator_vol;
	int32_t D_speech_vol;
	int32_t N_locator_vol;
	int32_t N_speech_vol;
	uint32_t IP_addr;
	int32_t Mode;
	int32_t Exp;
	int32_t Phase;
	int32_t Group;
	uint32_t	Beacon_mode;
	uint32_t AGC_Sens; // Valid Range 0-20
	int32_t WalkTimeout;// Valid Range 4000-20000mS
	uint32_t EXP_Time;// Valid Range 500-3000mS
	uint32_t REM_Butt;//Valid input 1,2
	uint32_t Repeat_Wait;//Valid Range (0-16000)mS
	uint32_t VIB_Intens;//Valid Range (0-?)mS
   int32_t GroupCall;
   uint32_t Recall;
   int32_t VibPulse;
   int32_t AudibleCountdown;
   int32_t LocatorAGC;
} apb_station_config_t;

typedef struct {
	uint8_t	dWalks;
	uint8_t clears;
	uint8_t	walks;
	uint8_t	calls;
	uint8_t	apsCalls;
	uint8_t bSrc;
	uint8_t bDest;	
	uint8_t	state;
	uint8_t	aMesg;
} apb_status_block_legacy_t;

typedef struct {
   uint16_t	dWalks;
	uint16_t clears;
	uint16_t	walks;
	uint16_t	calls;
	uint16_t	apsCalls;
	uint16_t bSrc;
	uint16_t bDest;
   uint16_t globalStatus;	
	uint32_t	state;
	uint32_t	aMesg;
} apb_status_block_t;

void apbconfig_save( apb_station_config_t* sc , const char* filename );
void apbconfig_load( apb_station_config_t* sc , const char* filename );

extern apb_station_config_t StationConfig;

extern apb_status_block_t StatusBlock;
extern apb_status_block_legacy_t StatusBlockLegacy;

extern int DontReset;

#endif /* _APBCONFIG_H_ */
