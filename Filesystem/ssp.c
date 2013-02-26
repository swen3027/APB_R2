#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "diskio.h"
#include "ssp.h"
#define rcvr_spi_m(dst)	*(dst)=spi_rw(0xff)
#define xmit_spi(dat) spi_rw(dat)
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

typedef uint8_t BYTE;
typedef uint8_t Bool;
typedef uint32_t DWORD;
#define FALSE 0
#define TRUE 1
static volatile DSTATUS Stat = STA_NOINIT;

static BYTE CardType;
static BYTE rcvr_spi(void);
void select()
{
	LPC_GPIO0->FIOCLR = (1 << SSEL);
}
void deselect()
{
	LPC_GPIO0->FIOSET = (1 << SSEL);
	rcvr_spi();
}
void spi_set_speed(uint8_t div)
{
	SSP->CR1 &= ~(1 << SSE);
	SSP->CPSR &= ~(0xFF);
	SSP->CPSR |= (div & 0xFF);
	SSP->CR1 |= (1 << SSE);
	while((SSP->SR & (1 << BSY)));
}
static BYTE spi_rw(BYTE out)
{
	BYTE in;
	SSP->DR = out;
	while((SSP->SR & (1 << BSY)));
	in = SSP->DR;
	return in;
}
static BYTE rcvr_spi(void)
{
	return (spi_rw(0xFF));
}
static void spi_rcvr_block(BYTE * buff, uint32_t btr)
{
	uint32_t hwtr, startcnt, i, rec;
	hwtr = btr/2;
	if(btr < 8)
		startcnt = hwtr;
	else
		startcnt = 8;
	SSP->CR0 |= 0x0F; //16 bit transfers
	for(i = startcnt; i; i--)
		SSP->DR= 0xFFFF;
	do
	{
		while(!(SSP->SR & RNE));
		rec = SSP->DR;
		if(i < (hwtr-startcnt))
		{
			SSP->DR = 0xFFFF;
		}
		*buff++ = (BYTE) (rec >> 8);
		*buff++ = (BYTE) (rec);
		i++;
	}while(i < hwtr);
	SSP->CR0 &= ~(0x0F);
	SSP->CR0 |= 0x07; //switch back to 8 bit transfers

}
static inline void spi_xmit_block(const BYTE* buff) //512 byte data block to be transmitted
{
	uint32_t cnt;
	uint16_t data;
	SSP->CR0 |= 0x0F; //Pop over to 16 bit mode
	for(cnt = 0; cnt < (512/2); cnt++)
	{	
		while(!(SSP->SR & TNF)); //wait for tx fif not full TNF
		data = (*buff++) << 8;
		data |= *buff++;
		SSP->DR = data;
	}
	while(SSP->SR & BSY);
	while(SSP->SR & RNE) //drain rx fifo
	{
		data = SSP->DR;
	}
	SSP->CR0 &= ~(0x0F);
	SSP->CR0 |= (0x07); //back to 8 bit mode.
}
static BYTE wait_ready(void)
{
	BYTE res;
	uint16_t timeout = 0xFFFF;
	rcvr_spi();
	do
		res = rcvr_spi();
	while((res != 0xFF) && timeout--);

	return res;
}
static Bool rcvr_datablock( BYTE * buff, uint32_t btr)
{
	BYTE token;
	uint16_t timout = 0xFFFF;
	do
		token = rcvr_spi();
	while((token == 0xFF) && timout--);
	do
	{
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	}while(btr -= 4);
	rcvr_spi();
	rcvr_spi();
	return TRUE;

}
static Bool xmit_datablock(const BYTE * buff, BYTE token)
{
	BYTE resp;
	BYTE wc;
	if(wait_ready() != 0xFF) return FALSE;
	
	xmit_spi(token);
	
	if(token != 0xFD)
	{
		wc = 0;
		do
		{
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		}while(--wc);
	}

	xmit_spi(0xFF);
	xmit_spi(0xFF);
	resp = rcvr_spi();
	if((resp & 0x1F) != 0x05)
		return FALSE;

}
static BYTE send_cmd(BYTE cmd, DWORD arg)
{
	BYTE n, res;
	if(cmd & 0x80)
	{
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if(res > 1)
			return res;
	}
	deselect();
	select();
	if(wait_ready() != 0xFF)
		return 0xFF;

	xmit_spi(cmd);
	xmit_spi((BYTE)(arg >> 24));
	xmit_spi((BYTE)(arg >> 16));
	xmit_spi((BYTE)(arg >> 8));
	xmit_spi((BYTE)arg);
	n = 0x01;
	if(cmd == CMD0) n = 0x95;
	if(cmd == CMD8) n = 0x87; //checksums
	
	xmit_spi(n);
	if(cmd == CMD12) rcvr_spi();
	
	n = 10;
	do
		res = rcvr_spi();
	while((res & 0x80) && --n);
	return res;
}
uint8_t spi_send_no_cs(uint8_t outgoing)
{
	
	SSP->DR = outgoing;
	while((SSP->SR & (1 << BSY)));
	return SSP->DR;

}
uint8_t spi_send(uint8_t outgoing)
{
	uint8_t incoming;
	select();
	SSP->DR = outgoing;
	while((SSP->SR & (1 << BSY)));
	incoming = SSP->DR;
	deselect();
	return incoming;
	
}
void spi_init()
{
	int i ;
	MESSAGE("Initializing SSP0 Interface for the SD card\n\r");
	LPC_SC->PCONP |= (1 << 21);
	LPC_SC->PCLKSEL1 &= ~(3 << 10);
	LPC_SC->PCLKSEL1 |= (1 << 10); //PCLK = CCLK
	LPC_GPIO0->FIODIR |= (1 << SCK) | ( 1 << MOSI) | (1 << SSEL);
	LPC_GPIO0->FIODIR &= ~(1 << MISO);
	LPC_PINCON->PINSEL0 &= ~(SSP0_SCK_FUNCMASK);
	LPC_PINCON->PINSEL0 |= (SSP0_SCK_FUNCBIT);
	LPC_PINCON->PINSEL1 &= ~(SSP0_SSEL_FUNCMASK | SSP0_MISO_FUNCMASK | SSP0_MOSI_FUNCMASK);
	LPC_PINCON->PINSEL1 |= (SSP0_SSEL_FUNCBIT | SSP0_MISO_FUNCBIT | SSP0_MOSI_FUNCBIT);
	SSP->CR0 |= (7 << DSS) | ( 0 << FRF) | (0 << CPOL) | (0 << CHPA) | (0 << SCR);
	SSP->CR1 |= (1 << SSE);
	deselect();
	LPC_PINCON->PINSEL1 &= ~(SSP0_MOSI_FUNCMASK);
	LPC_GPIO0->FIODIR |= (1 << MOSI);
	LPC_GPIO0->FIOSET = (1 << MOSI);
	
	LPC_PINCON->PINSEL1 |= (SSP0_MOSI_FUNCBIT);
	//spi_set_speed(8);
}
DRESULT MMC_disk_read(
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
	if (count == 1) 
	{	/* Single block read */
		if (send_cmd(CMD17, sector) == 0)	
		{ /* READ_SINGLE_BLOCK */
			if (rcvr_datablock(buff, 512)) 
			{
				count = 0;
			}
		}
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) 
		{	/* READ_MULTIPLE_BLOCK */
			do 
			{
				if (!rcvr_datablock(buff, 512)) 
				{
					break;
				}
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();
	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/

/* Write Sector(s)                                                       */

/*-----------------------------------------------------------------------*/
DRESULT MMC_disk_write(
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;
	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();
	return count ? RES_ERROR : RES_OK;
}
DSTATUS MMC_disk_ioctl(
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;
	res = RES_ERROR;
	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:		/* Sub control code == 0 (POWER_OFF) */
			res = RES_OK;
			break;
		case 1:		/* Sub control code == 1 (POWER_ON) */
			spi_init();
			spi_set_speed(254);
			deselect();
			res = RES_OK;
			break;
		case 2:		/* Sub control code == 2 (POWER_GET) */
			*(ptr+1) = 1;
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else 
	{
		if (Stat & STA_NOINIT) return RES_NOTRDY;
		switch (ctrl) 
		{
			case CTRL_SYNC :		/* Make sure that no pending write process */
				select();
				if (wait_ready() == 0xFF)
					res = RES_OK;
				break;
			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) 
				{
					if ((csd[0] >> 6) == 1) 
					{	/* SDC version 2.00 */
						csize = csd[9] + ((WORD)csd[8] << 8) + 1;
						*(DWORD*)buff = (DWORD)csize << 10;
					} 
					else 
					{					/* SDC version 1.XX or MMC*/
						n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
						csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
						*(DWORD*)buff = (DWORD)csize << (n - 9);
					}
					res = RES_OK;

				}
				break;
		case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			if (CardType & CT_SD2) 
			{	/* SDC version 2.00 */
				if (send_cmd(ACMD13, 0) == 0) 
				{	/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) 
					{				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} 
			else 
			{					/* SDC version 1.XX or MMC */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) 
				{	/* Read CSD */
					if (CardType & CT_SD1) 
					{	/* SDC version 1.XX */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} 
					else 
					{					/* MMC */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;
		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;
		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;
		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;
		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;
		case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;
		default:
			res = RES_PARERR;
		}
		deselect();
	}
	return res;
}

/*-----------------------------------------------------------------------*/

/* Device Timer Interrupt Procedure  (Platform dependent)                */

/*-----------------------------------------------------------------------*/

/* This function must be called in period of 10ms                        */

void MMC_disk_timerproc(void)
{

}
DSTATUS MMC_disk_initialize(void)
{
	uint32_t Timer1;
	BYTE n, cmd, ty, ocr[4];
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */
	//power_on();							/* Force socket power on and initialize interface */
	spi_init();
	spi_set_speed(254);
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks with card de-selected */
	ty = 0;
	if (send_cmd(CMD0, 0) == 1) 
	{			/* Enter Idle state */
		Timer1 = 1000000;						/* Initialization timeout of 1000 milliseconds */
		if (send_cmd(CMD8, 0x1AA) == 1) 
		{	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 response */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) 
			{				/* The card can work at VDD range of 2.7-3.6V */
				while (Timer1-- && send_cmd(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				Timer1 = 1000000;
				if (Timer1-- && send_cmd(CMD58, 0) == 0) 
				{		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}

		}
		else 
		{							/* SDSC or MMC */
			if (send_cmd(ACMD41, 0) <= 1) 	
			{
				ty = CT_SD1; cmd = ACMD41;	/* SDSC */
			} 
			else 
			{
				ty = CT_MMC; cmd = CMD1;	/* MMC */
			}
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
		// xprintf("cmd 0 failed\n");
	}
	CardType = ty;
	deselect();
	if (ty) {			/* Initialization succeeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
		spi_set_speed(8);

	} 
	else {			/* Initialization failed */

		MESSAGE("SD card failed to initalize at the hardware level\n\r");
	}
	return Stat;
}

/*-----------------------------------------------------------------------*/

/* Get Disk Status                                                       */

/*-----------------------------------------------------------------------*/

DSTATUS MMC_disk_status(void)
{
	return Stat;
}

