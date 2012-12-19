#define SSP	LPC_SSP0 //using ssp0


//SSPx->CR0 definiions
#define CHPA 7
#define CPOL 6
#define DSS 0
#define FRF 4
#define SCR 8


//SSPx->CR1 defintions
#define LBM 0
#define SSE 1
#define MS 2
#define SOD 3

//SSPx->SR definitions
#define TFE 0
#define TNF 1
#define RNE 2
#define RFF 3
#define BSY 4

//SSPx->CPSR
#define CPSRDVSR 0

//Pin defs
#define SCK	15	//P0.15 
#define SSEL	16	//P0.16
#define MISO	17	//P0.17
#define MOSI	18	//P0.18

#define SSP0_SCK_FUNCMASK	(3 << 30) //PINSEL 0
#define SSP0_SSEL_FUNCMASK	(3 << 0) //PINSEL 1
#define SSP0_MISO_FUNCMASK	(3 << 2) //PINSEL 1
#define SSP0_MOSI_FUNCMASK	(3 << 4) //PINSEL 1

#define SSP0_SCK_FUNCBIT	(2 << 30) //PINSEL 0
#define SSP0_SSEL_FUNCBIT	(0 << 0) //PINSEL 1; using GPIO for CS#
#define SSP0_MISO_FUNCBIT	(2 << 2) //PINSEL 1
#define SSP0_MOSI_FUNCBIT	(2 << 4) //PINSEL 1

#define SSP_PRESCALE_MIN 8
void select();
void deselect();
void spi_set_speed(uint8_t div);
uint8_t spi_send_no_cs(uint8_t outgoing);
uint8_t spi_send(uint8_t outgoing);
void spi_init();

