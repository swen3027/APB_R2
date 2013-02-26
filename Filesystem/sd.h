//#include "config.h"
//#include "types.h"
//#include "debug.h"




#define	CMDREAD		17

#define	CMDWRITE	24

#define	CMDREADCSD       9

int8_t sd_Init();
void sd_Command(uint8_t , uint16_t , uint16_t );
uint8_t sd_Resp8b();
uint16_t sd_Resp16b();
void sd_Resp8bError(uint8_t );
int8_t sd_State();
int8_t sd_writeSector(uint32_t , uint8_t* );
int8_t sd_readSector(uint32_t , uint8_t* , uint16_t );
int8_t sd_getDriveSize(uint32_t* );


