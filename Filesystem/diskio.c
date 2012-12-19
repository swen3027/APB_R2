#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "diskio.h"
#include "sd.h"
#include "ssp.h"

DSTATUS disk_initialize (BYTE drive)
{
	uint32_t disk_size;
/*	spi_init(); //init SPI
	if(sd_Init() < 0)
	{
		MESSAGE("Card failed to init.\n\r");
		return STA_NOINIT|STA_NODISK;
	}
	if(sd_State() < 0)
	{
		MESSAGE("Card didn't return the ready state.\n\r");
		return STA_NOINIT|STA_NODISK;
	}
	sd_getDriveSize(&disk_size);
	MESSAGE("Drive size is %d bytes\n\r");
	spi_set_speed(8);	
	return 0;
*/
	MESSAGE("in disk_initalize\n\r");
	return MMC_disk_initialize();

}
DSTATUS disk_status (BYTE drive)
{
	
/*	uint16_t value;
	DSTATUS retval = 0x0; 
	sd_Command(13,0,0);
	value = sd_Resp16b();
	switch(value)
	{
		case 0x0000:
			MESSAGE("Card okay\n\r");
			break;
		case 0x0001:
			MESSAGE("Card is locked\n\r");
			retval |= STA_PROTECT;
			break;
		case 0x0002:
			MESSAGE("WP Erase Skip, Lock/Unlock cmd failed\n\r");
			retval |= STA_PROTECT;
			break;
		case 0x0004:
			MESSAGE("General error\n\r");
			retval |= STA_NOINIT;
			break;
		case 0x0008:
			MESSAGE("Internal card error\n\r");
			retval |= STA_NODISK;
			break;
		case 0x0010:
			MESSAGE("Card internall ECC was applied, but failed to correct the data\n\r");
			retval |= 0x0010;
			break;
		case 0x0020:
			MESSAGE("Write protect violation\n\r");
			retval |= STA_PROTECT;
			break;
		case 0x0040:
			MESSAGE("An invalid section, sectors for erase\n\r");
			retval |= 0x0040;
			break;
		case 0x0080:
			MESSAGE("Out of range, CSD overwrite\n\r");
			retval |= STA_PROTECT;
			break;
		default:
			if(value > 0x00FF)
				sd_Resp8bError(value >> 8);
			else
				MESSAGE("Unknown error : 0x%x\n\r", value);
			
	}
	return retval;*/

	return MMC_disk_status();
}
DRESULT disk_read (BYTE drive, BYTE* buff , DWORD sector, BYTE count)
{
	
	return MMC_disk_read(buff, sector, count);
}
DRESULT disk_write (BYTE drive, const BYTE* buff , DWORD sector, BYTE count)
{
	return MMC_disk_write(buff, sector, count);
}

DRESULT disk_ioctl (BYTE drive , BYTE ctl, void* param)
{
	return MMC_disk_ioctl(ctl, param);
}
