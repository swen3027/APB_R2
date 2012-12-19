#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "spi.h"



// SP0SPCR  Bit-Definitions
#define CPHA    3
#define CPOL    4
#define MSTR    5
// SP0SPSR  Bit-Definitions
#define SPIF	7

#define SPI_IODIR			LPC_GPIO0->FIODIR
#define SPI_SCK_PIN			15		/* Clock		P0.15 out		(PINSEL0) */
#define SPI_SSEL_PIN		16		/* Card-Select	P0.16 GPIO out	(PINSEL1) */
#define SPI_MISO_PIN		17		/* from Card	P0.17 in		(PINSEL1) */
#define SPI_MOSI_PIN		18		/* to Card		P0.18 out		(PINSEL1) */

#define SPI_PINSEL			LPC_PINCON->PINSEL1
#define SPI_SCK_FUNCMASK	(3 << 30) // P0.15 - PINSEL0 [31:30]
#define SPI_SSEL_FUNCMASK	(3 << 0)  // P0.16 - PINSEL1 [1:0]
#define SPI_MISO_FUNCMASK	(3 << 2)  // P0.17 - PINSEL1 [3:2]
#define SPI_MOSI_FUNCMASK	(3 << 4)  // P0.18 - PINSEL1 [5:4]

#define SPI_SCK_FUNCBIT		(3 << 30)
#define SPI_SSEL_FUNCBIT	(0 << 0) // Note - SSEL needed as GPIO
#define SPI_MISO_FUNCBIT	(3 << 2)
#define SPI_MOSI_FUNCBIT	(3 << 4)

#define SPI_PRESCALE_REG	LPC_SPI->SPCCR

#define SPI_PRESCALE_MIN  8


void SELECT_CARD()
{
	LPC_GPIO0->FIOCLR = (1 << SPI_SSEL_PIN);

}
void UNSELECT_CARD()
{
	LPC_GPIO0->FIOSET = (1 << SPI_SSEL_PIN);
}
int8_t init_interface()
{
	uint32_t sc;
	
	if_spiInit(file); /* init at low speed */
	
	if(sd_Init(file)<0)	{
		printf("Card failed to init, breaking up...\n\r");
		return(-1);
	}
	if(sd_State(file)<0){
		printf("Card didn't return the ready state, breaking up...\n\r");
		return(-2);
	}
	// file->sectorCount=4; /* FIXME ASAP!! */
	
	sd_getDriveSize(file, &sc);
	file->sectorCount = sc/512;
	if( (sc%512) != 0) {
		file->sectorCount--;
	}
	printf("Drive Size is %d Bytes (%d sectors)\n\r ", sc, file->sectorCount); 	
	if_spiSetSpeed(SPI_PRESCALE_MIN);
	// if_spiSetSpeed(100); /* debug - slower */
	
	printf("Init done...\n\r");
	return 0;
}

void if_spiInit(BYTE drive)
{
	int i;

	printf("spiInit for SPI(0)\n\r");

	// Turn on the power
	LPC_SC->PCONP |= (1<<8); // PCSPI

	// Clock
	LPC_SC->PCLKSEL0 &= ~(3<<16);  // PCLK_SPI
	LPC_SC->PCLKSEL0 |=  (1<<16);  // PCLK_periph = CCLK

	// setup GPIO
	LPC_GPIO0->FIODIR |= (1 << SPI_SCK_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_SSEL_PIN);
	LPC_GPIO0->FIODIR &= ~(1 << SPI_MISO_PIN);

	//// reset Pin-Functions
	// P0.15 set to SCK
	LPC_PINCON->PINSEL0 &= ~SPI_SCK_FUNCMASK;
	LPC_PINCON->PINSEL0 |=  SPI_SCK_FUNCBIT;
	// P0.16, P0.17, P0.18 set to SSEL,MISI, MOSI
	SPI_PINSEL &= ~(SPI_MOSI_FUNCMASK | SPI_MISO_FUNCMASK | SPI_SSEL_FUNCMASK);
	SPI_PINSEL |=  (SPI_MOSI_FUNCBIT | SPI_MISO_FUNCBIT | SPI_SSEL_FUNCBIT);

	// enable SPI-Master
	LPC_SPI->SPCR = (1 << MSTR) | (0 << CPOL);	// TODO: check CPOL

	UNSELECT_CARD();

	// Code for GPIO setup
    /* Switch the SSI TX line to a GPIO and drive it high too. */
    // P0.18 back to GPIO
	LPC_PINCON->PINSEL1 &= ~(SPI_MOSI_FUNCMASK);
	LPC_GPIO0->FIODIR |= (1<<SPI_MOSI_PIN);
	LPC_GPIO0->FIOSET = (1<<SPI_MOSI_PIN);

	// low speed during init
	if_spiSetSpeed(254); 

	/* Send 20 spi commands with card not selected */
	for(i=0;i<21;i++)
		my_if_spiSend(file, 0xff);

    // P0.18 back to SPI
	LPC_PINCON->PINSEL1 |= (SPI_MOSI_FUNCBIT);

}
uint8_t my_if_spiSend(hwInterface * iface, uint8_t outgoing)
{
	LPC_SPI->SPDR = outgoing;
	while (!(LPC_SPI->SPSR & (1 << SPIF)));
	return LPC_SPI->SPDR;
}
int8_t if_readBuf(hwInterface* file, uint32_t address,uint8_t * buf)
{
	return(sd_readSector(file,address,buf,512));
}
int8_t if_writeBuf(hwInterface * file, uint32_t address,uint8_t * buf)
{
	return(sd_writeSector(file,address, buf));
}
int8_t if_setPos(hwInterface * file, uint32_t address)
{
	return 0;
}
void if_spiSetSpeed(uint8_t speed)
{
	speed &= 0xFE;
	if ( speed < SPI_PRESCALE_MIN  ) speed = SPI_PRESCALE_MIN ;
	SPI_PRESCALE_REG = speed;
}
uint8_t if_spiSend(hwInterface * iface,  uint8_t outgoing)
{
	euint8 incoming;

	SELECT_CARD();
	LPC_SPI->SPDR = outgoing;
	while (!(LPC_SPI->SPSR & (1 << SPIF)));
	incoming = LPC_SPI->SPDR;
	UNSELECT_CARD();
	return incoming;
}


