#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "apbconfig.h"
#include "ff.h"

apb_status_block_t StatusBlock;
apb_status_block_legacy_t StatusBlockLegacy;
apb_station_config_t 	StationConfig;

int DontReset;

static void _apbconfig_makedefault( apb_station_config_t* sc );

FIL fp[2];
void apbconfig_save( apb_station_config_t* sc , const char* filename )
{

	
	//Assumes that filesystem has been initalized (mounted)
	uint8_t return_value = 0;
	uint32_t bytes_written = 0;
	FIL fout;
	MESSAGE("filename we are trying to open: %s\n\r", filename);
	return_value = f_open(&fout, filename, FA_WRITE|FA_CREATE_ALWAYS); //Overwrites old configuration
	if(return_value == FR_OK)
	{	
		return_value = f_write(&fout, sc, sizeof(*sc), &bytes_written);
		if(return_value != FR_OK)
		{
			MESSAGE("f_write failed with %u\n\r", return_value);
		}
		if(bytes_written != sizeof(*sc))
			MESSAGE("%d bytes written, should have been %d\n\r", bytes_written, sizeof(*sc));
		return_value = f_close(&fout);
		if(return_value == FR_OK)
		{
			MESSAGE("apbconfig saved\n\r");
			return;
		}
		else
		{
			MESSAGE("f_close failed with %u\n\r", return_value); 
		}
	
	}
	else
	{
		MESSAGE("f_open in apbconfig_save failed, return value: %d\n\r", return_value);

	}
}

void apbconfig_load( apb_station_config_t* sc , const char* filename ){
	FIL  fin;
	int n;
	uint8_t return_value;
	uint32_t bytes_read;
	apb_station_config_t  s2;
	return_value = 0;
	return_value = f_open(&fin, filename, FA_READ);
	if(return_value == FR_NO_FILE)
	{
		MESSAGE("File doesn't exist\n\r");
		_apbconfig_makedefault(&s2);
		MESSAGE("Made default\n\r");
		apbconfig_save(&s2, filename);
	}
	else
	{
		MESSAGE("f_open return value: %u\n\r", return_value);
		bytes_read = 0;
		return_value = 0;
		return_value = f_read(&fin, sc, sizeof(*sc), &bytes_read);
		if(return_value != FR_OK)
			MESSAGE("Failed to read the file: %u\n\r", return_value);
		return_value = f_close(&fin);
		MESSAGE("f_close in apbconfig_load return value: %u \n\r", return_value);
		if(bytes_read != sizeof(*sc))
		{
			MESSAGE("Bad config file\n\r");
			_apbconfig_makedefault(sc);
			apbconfig_save(sc, filename);
		}
		

	}
}

void _apbconfig_makedefault( apb_station_config_t* sc ){
	sc->ID = 0;
	sc->N_mode = 0;
	sc->D_locator_vol = 0;
	sc->D_speech_vol = 60;
	sc->N_locator_vol = -7;
	sc->N_speech_vol = 17;
	sc->IP_addr = DEFAULT_IP;
	sc->Mode = 1;
	sc->Exp = 0;
	sc->Phase = 0;
	sc->Group = 0;
	sc->Beacon_mode = 0;
	sc->AGC_Sens = 5;
	sc->WalkTimeout = 7000;
	sc->EXP_Time = 1000;
	sc->Repeat_Wait = 0;
   sc->GroupCall = 0;
   sc->Recall = 0;
   sc->VibPulse = 0;
   sc->AudibleCountdown = 0;
   sc->LocatorAGC = 1;
}
