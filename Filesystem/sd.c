#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "uart0.h"
#include "ssp.h"
#include "sd.h"

/*START FUNCTION DESCRIPTION: sd_Init <sd.c>
**SYNATX:		int8_t sd_Init(hwInterface *)
**PARAMETER1:		Pointer to hardware interface
**KEYWORDS:		sd init initalization
**DESCRIPTION:		Initializes the SD card
**RETURN VALUE:		0 on success
			-1 on hardware failure
			-2 on other type of failure
			-3 on initalization on error
**NOTES:		Written by CodeRed for the RDB1768	
**END FUNCTION DESCRIPTION*/
int8_t sd_Init()
{
	int16_t i;
	uint8_t resp;
	/* Try to send reset command up to 100 times */
	MESSAGE("SD init\n\r");
	i=100;
	do{
		sd_Command(0, 0, 0);
		resp=sd_Resp8b();
	}
	while(resp!=1 && i--);
	
	if(resp!=1){
		if(resp==0xff){
			return(-1);
		}
		else{
			sd_Resp8bError(resp);
			return(-2);
		}
	}

	/* Wait till card is ready initialising (returns 0 on CMD1) */
	/* Try up to 32000 times. */
	i=32000;
	do{
		sd_Command(1, 0, 0);
		resp=sd_Resp8b();
		if(resp!=0)
			sd_Resp8bError(resp);
	}
	while(resp==1 && i--);
	if(resp!=0){
		sd_Resp8bError(resp);
		return(-3);
	}
	return(0);
}

/*****************************************************************************/
/*START FUNCTION DESCRIPTION: sd_Command <sd.c>
**SYNATX:		void sd_Command(hwInterface *, uint8_t, uint16_t, euint16);
**PARAMETER1:		Pointer to hardware interface
**PARAMETER2:		8 bit command to send to SD card
**PARAMETER3:		Parameter X
**PARAMETER4:		Parameter Y
**KEYWORDS:		send SD command cmd SD
**DESCRIPTION:		This function sends a command to the SD card
**RETURN VALUE:		void
**NOTES:		Written for CodeRed RDB1768
**END FUNCTION DESCRIPTION*/


void sd_Command(uint8_t cmd, uint16_t paramx, uint16_t paramy)
{
	spi_send(0xff);
	spi_send(0x40 | cmd);
	spi_send((uint8_t) (paramx >> 8)); /* MSB of parameter x */
	spi_send((uint8_t) (paramx)); /* LSB of parameter x */
	spi_send((uint8_t) (paramy >> 8)); /* MSB of parameter y */
	spi_send((uint8_t) (paramy)); /* LSB of parameter y */
	spi_send(0x95); /* Checksum (should be only valid for first command (0) */
	spi_send(0xff); /* eat empty command - response */
}
/*****************************************************************************/
uint8_t sd_Resp8b()
{
	uint8_t i;
	uint8_t resp;
	/* Respone will come after 1 - 8 pings */
	for(i=0;i<8;i++){
		resp = spi_send(0xff);
		if(resp != 0xff)
			return(resp);
	}
	return(resp);
}

/*****************************************************************************/
uint16_t sd_Resp16b()
{
	uint16_t resp;
	resp = ( sd_Resp8b() << 8 ) & 0xff00;
	resp |= spi_send(0xff);
	return(resp);
}

/*****************************************************************************/
void sd_Resp8bError(uint8_t value)
{
	switch(value)
	{
		case 0x40:
			printf("Argument out of bounds.\n\r");
			break;
		case 0x20:
			printf("Address out of bounds.\n\r");
			break;
		case 0x10:
			printf("Error during erase sequence.\n\r");
			break;
		case 0x08:
			printf("CRC failed.\n\r");
			break;
		case 0x04:
			printf("Illegal command.\n\r");
			break;
		case 0x02:
			printf("Erase reset (see SanDisk docs p5-13).\n\r");
			break;
		case 0x01:
			printf("Card is initialising.\n\r");
			break;
		default:
			printf("Unknown error 0x%x (see SanDisk docs p5-13)\n\r",value); 
			break;

	}

}

/*****************************************************************************/
int8_t sd_State()
{
	int16_t value;
	sd_Command(13, 0, 0);
	value=sd_Resp16b();
	switch(value)
	{
		case 0x000:
			return(1);
			break;
		case 0x0001:
			printf("Card is Locked.\n");
			break;
		case 0x0002:
			printf("WP Erase Skip, Lock/Unlock Cmd Failed.\n");
			break;
		case 0x0004:
			printf("General / Unknown error -- card broken?.\n");
			break;
		case 0x0008:
			printf("Internal card controller error.\n");
			break;
		case 0x0010:
			printf("Card internal ECC was applied, but failed to correct the data.\n");
			break;
		case 0x0020:
			printf("Write protect violation.\n");
			break;
		case 0x0040:
			printf("An invalid selection, sectors for erase.\n");
			break;
		case 0x0080:
			printf("Out of Range, CSD_Overwrite.\n");
			break;
		default:
			if(value>0x00FF)
				sd_Resp8bError((uint8_t) (value>>8));
			else
				printf("Unknown error: 0x%x (see SanDisk docs p 5-14)\n\r", value);
			break;
	}
	return(-1);
}
int8_t sd_writeSector(uint32_t address, uint8_t* buf)
{
	uint32_t place;
	uint16_t i;
	uint16_t t=0;
	printf("Trying to write %x to sector 0x%x\n\r", &buf, address); 
	place=512*address;
	sd_Command(CMDWRITE, (uint16_t) (place >> 16), (uint16_t) place);
	sd_Resp8b(); /* Card response */
	spi_send(0xfe); /* Start block */
	for(i=0;i<512;i++) 
		spi_send(buf[i]); /* Send data */
	spi_send(0xff); /* Checksum part 1 */
	spi_send(0xff); /* Checksum part 2 */
	spi_send(0xff);
	while(spi_send(0xff)!=0xff){
		t++;
	}

	return(0);
}

int8_t sd_readSector(uint32_t address, uint8_t* buf, uint16_t len)
{
	uint8_t cardresp;
	uint8_t firstblock;
	uint8_t c;
	uint16_t fb_timeout=0xffff;
	uint32_t i;
	uint32_t place;
	/*printf("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
	place=512*address;
	sd_Command(CMDREAD, (uint16_t) (place >> 16), (uint16_t) place);
	cardresp=sd_Resp8b(); /* Card response */ 
	/* Wait for startblock */
	do
		firstblock=sd_Resp8b(); 
	while(firstblock==0xff && fb_timeout--);
	if(cardresp!=0x00 || firstblock!=0xfe){
		sd_Resp8bError(firstblock);
		return(-1);
	}
	for(i=0;i<512;i++){
		c = spi_send(0xff);
		if(i<len)
			buf[i] = c;
	}

	/* Checksum (2 byte) - ignore for now */
	spi_send(0xff);
	spi_send(0xff);
	return(0);
}



int8_t sd_getDriveSize(uint32_t* drive_size )
{
	uint8_t cardresp, i, by;
	uint8_t iob[16];
	uint16_t c_size, c_size_mult, read_bl_len;
	sd_Command(CMDREADCSD, 0, 0);
	do {
		cardresp = sd_Resp8b();
	} while ( cardresp != 0xFE );
	printf("CSD:");
	for( i=0; i<16; i++) {
		iob[i] = sd_Resp8b();
		UART0_PrintHex(iob[i]);

	}
	newline();
	spi_send(0xff);
	spi_send(0xff);
	c_size = iob[6] & 0x03; // bits 1..0
	c_size <<= 10;
	c_size += (uint16_t)iob[7]<<2;
	c_size += iob[8]>>6;
	by= iob[5] & 0x0F;
	read_bl_len = 1;
	read_bl_len <<= by;
	by=iob[9] & 0x03;
	by <<= 1;
	by += iob[10] >> 7;
	c_size_mult = 1;
	c_size_mult <<= (2+by);
	*drive_size = (uint32_t)(c_size+1) * (uint32_t)c_size_mult * (uint32_t)read_bl_len;
	return 0;
}
