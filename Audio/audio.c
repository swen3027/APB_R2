#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "LPC17xx.h"
#include "audio.h"
#include "i2c.h"
#include "uart0.h"
#include "RDB1768.h"
#include "ff.h"
#define ABS(a)	   (((a) < 0) ? -(a) : (a))


extern long int gQ31_Peak_L, gQ31_Peak_R;                              
extern long int gQ31_Peak[2];                                          
long int Q31_I2S_Left, Q31_I2S_Right;
int volume = 0;
int bass = 0;
int fon = 1;
xQueueHandle lineInQueue;
xQueueHandle audioControl;
xQueueHandle settings;
#define MAX_DEPTH 8
typedef struct {
	long int left[MAX_DEPTH];
	long int right[MAX_DEPTH];
	int depth;
} lineData;
/*START FUNCTION DESCRIPTION: AudioReadReg  <audio.c>
**SYNATX:		uint16_t AudiotReadReg(uint16_t)	
**PARAMETER1:		Hexadecimal value of register to read
**KEYWORDS:		Audio read register data
**DESCRIPTION:		Function reads a register and returns its value
**RETURN VALUE:		Value of the register 'reg'
**NOTES:		Uses I2C to read the UDA1380 Registesr
**END FUNCTION DESCRIPTION*/
uint16_t AudioReadReg(uint16_t reg)
{
	uint32_t i2c_status;
	uint8_t tx_data[3];
	uint8_t rx_data[2];
	char str[128];
	I2C_TRANSFER_OPT_Type OptType;	
	I2C_M_SETUP_Type transferConfig;
	tx_data[0] = reg;
	transferConfig.sl_addr7bit = UDA1380_ADDR >> 1;
	transferConfig.tx_data = tx_data;
	transferConfig.tx_length = 1;
	transferConfig.rx_data = rx_data;
	transferConfig.rx_count = 0;
	transferConfig.rx_length = 2;
	transferConfig.retransmissions_max = 0xFF;
	transferConfig.callback = NULL;
	OptType = I2C_TRANSFER_INTERRUPT;
	I2C_MasterTransferData(LPC_I2C0,&transferConfig, OptType);
//	MESSAGE("Register 0x%x data: 0x%x\n\r", reg, (rx_data[0]<<8)| rx_data[1] );	
	return (rx_data[0] << 8)|rx_data[1];
}
/*START FUNCTION DESCRIPTION: vAudioSendCommand	<audio.c>
**SYNATX:		void vAudioSendCommand(uint16_t, uint32_t)
**PARAMETER1:		Hex address of register to write to
**PARAMETER2:		Command to send into the register
**KEYWORDS:		send command
**DESCRIPTION:		Writes command 'cmd' into register 'reg
**RETURN VALUE:		Void
**NOTES:		Uses I2C to write commands to registers in the UDA1380
**END FUNCTION DESCRIPTION*/
void vAudioSendCommand(uint16_t reg, uint32_t cmd)
{

	uint32_t i2c_status;
	uint8_t transferData[4];
	I2C_M_SETUP_Type transferConfig;
	I2C_TRANSFER_OPT_Type OptType;
	transferData[0] = reg & 0xFF;
	transferData[1] = ((cmd >> 8) & 0xFF);
	transferData[2] = cmd & 0xFF;
	transferConfig.sl_addr7bit = UDA1380_ADDR>>1;//7 bit addr of device
	transferConfig.tx_data = transferData;
	transferConfig.tx_length = 3;
	transferConfig.tx_count = 0;
	transferConfig.rx_data = NULL;
	transferConfig.rx_length = 0;
	transferConfig.rx_count = 0;
	transferConfig.retransmissions_max = 0xFF;
	transferConfig.retransmissions_count = 0;
	transferConfig.status = 0x00;
	transferConfig.callback = NULL; //Pointer to callback function when complete
	OptType = I2C_TRANSFER_INTERRUPT;
	i2c_status = I2C_MasterTransferData(LPC_I2C0, &transferConfig, OptType); 
	MESSAGE( "I2C info:\n\r\tretransmission count: 0x%x\n\r\tstatus: 0x%x\n\r", transferConfig.retransmissions_count, transferConfig.status);

}
/*START FUNCTION DESCRIPTION: vAudioInit	<audio.c>
**SYNATX:		void vAudioInit(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t)
**PARAMETER1:		Peripherial clock divider value for I2S block( 1, 2, 4, 8)
**PARAMETER2:		I2S Rx and Tx rate multiplier (0 - 255)
**PARAMETER3:		I2S Rx and Tx rate divider (0 - 255)
**PARAMETER4:		Sets the Rx and Tx bitrate divider (0 - 31) to produce the bitclock
**PARAMETER5:		Number of channels desired (1, 2)
**PARAMETER6:		Sets the bitwidth of the channels (8, 16, 32)	
**KEYWORDS:		init initialization UDA1380 I2S I2C
**DESCRIPTION:		This function initalizes the I2C interface, I2S interface, and the UDA1380
**RETURN VALUE:		void
**NOTES:		See UM10360 pg 473
**END FUNCTION DESCRIPTION*/
void vAudioInit(uint32_t pclkdiv, uint32_t x, uint32_t y, uint32_t tx_bitrate, int32_t nChan, int32_t bw)
{
	uint16_t data;
	volume = 0;
	LPC_PINCON->PINSEL1 &= ~0x03C00000;
	LPC_PINCON->PINSEL1 |= (1 << 22) | (1 << 24); //Enable I2C	
	LPC_PINCON->PINSEL0 &= ~((3 << 14)|(3 << 16)|(3 << 18));
	LPC_PINCON->PINSEL0 |= (1 << 14)|(1 << 16)|(1 << 18);
	LPC_SC->PCONP |= (1 << 19); //Power up I2C
	I2C_Cmd(LPC_I2C0, ENABLE);
	I2C_Init(LPC_I2C0, 400000);//400 kHz I2C rate
	I2C_Cmd(LPC_I2C0, ENABLE);
	LPC_GPIO0->FIODIR |= (1 << 10);//Set direction of Codec reset as output
	LPC_GPIO0->FIOCLR = (1 << 10); //Reset Codec
	LPC_GPIO0->FIOSET = (1 << 10); //Bring out of reset
	
	LPC_GPIO0->FIODIR |= (1 << 10);//Set direction of Codec reset as output
	LPC_GPIO0->FIOCLR = (1 << 10); //Reset Codec
	LPC_GPIO0->FIOSET = (1 << 10); //Bring out of reset
	LPC_SC->PCONP |= (1 << 27); //Turn on power to I2S
	LPC_SC->PCLKSEL1 &= ~(3 << 22); 
	switch(pclkdiv)
	{
		case 1:
			LPC_SC->PCLKSEL1 |=( 0x01 << 22); 
			break;
		case 2:
			LPC_SC->PCLKSEL1 |=( 0x02 << 22); 
			break;
		case 4:
			LPC_SC->PCLKSEL1 |=( 0x00 << 22); 
			break;
		case 8:
			LPC_SC->PCLKSEL1 |=( 0x03 << 22); //CCLK /8
			break;
		default:
			UART0_PrintString("Bad pclk divsor\n\r");			
			break;
	}
	LPC_PINCON->PINSEL0 |= ( 1 << 14); //P0.7 = I2STX_CLK (the bit clock)
	LPC_PINCON->PINSEL0 |= (1 << 16); //P0.8 = I2STX_WS (word clock)
	LPC_PINCON->PINSEL0 |= (1 << 8); //P0.4 = I2SRX_CLK 
	LPC_PINCON->PINSEL1 |= (2 << 16);//P0.24 = I2SRX_WS
	LPC_PINCON->PINSEL0 |= (1 << 18); //P0.9 = I2STX_SD (data to codec)
	LPC_PINCON->PINSEL1 |= (2 << 18); //P0.25 = I2SRX_SD (data from codec)
	LPC_PINCON->PINSEL9 |= (1 << 24); //P4.28 = RX_MCLK
	LPC_PINCON->PINSEL9 |= (1 << 26); //P4.29 = TX_MCLK
	
	LPC_I2S->I2SRXRATE = ((x & 0xFF) << 8) | (y & 0xFF);
	LPC_I2S->I2STXRATE = ((x & 0xFF) <<8) | (y & 0xFF);
	// MCLK for RX
	LPC_I2S->I2SRXMODE = (1<<3);		//from TX MCLK
	// MCLK for TX
	LPC_I2S->I2STXMODE = (1<<3);	// generated
	// MCLK is 256xFs
	// Bitclock rate is 64xFs = MCLK/4
	LPC_I2S->I2STXBITRATE = tx_bitrate & 0x3F;
	LPC_I2S->I2SRXBITRATE = tx_bitrate;
	// Codec works in 32-bit per channel mode.
	switch(bw)
	{
		case 8:
			LPC_I2S->I2SDAI = ( 0x00 ) | ((((nChan * 8)/2)-1)<<6);
			LPC_I2S->I2SDAO = ( 0x00 ) | ((((nChan * 8)/2)-1)<<6);
			break;
		case 16:
			LPC_I2S->I2SDAI = ( 0x01 ) | ((((nChan * 16)/2)-1)<<6);
			LPC_I2S->I2SDAO = ( 0x01 ) | ((((nChan * 16)/2)-1)<<6);
			break;
		case 32:
			LPC_I2S->I2SDAI = ( 0x03 ) | ((((nChan * 32)/2)-1)<<6);
			LPC_I2S->I2SDAO = ( 0x03 ) | ((((nChan * 32)/2)-1)<<6);
			break;
		default:
			MESSAGE("bad bitwidth\n\r");
			break;

	}
//	LPC_I2S->I2SDAI = (bw & 0x03) | ( 31 <<6);
//	LPC_I2S->I2SDAO = (bw & 0x03) | (31<<6);<F9>
	LPC_I2S->I2SDAO = (7 << 6) | (0x00 << 0) |( 0 << 2);
	// RESET I2S
	LPC_I2S->I2SDAO |= (1<<4) | (1<<3);
	LPC_I2S->I2SDAI |= (1<<4) | (1<<3);
	LPC_I2S->I2SDAO &= ~((1<<4) | (1<<3) | (1 << 15)| (1 << 5));
	LPC_I2S->I2SDAI &= ~((1<<4) | (1<<3)| (1 << 5));	
	

	MESSAGE("Pins and I2S initalized\n\r");
	//Software reset for the codec	
	vAudioSendCommand(UDA1380_SW_RESET, 0xFFFF);
	MESSAGE("UDA1380 Reset\n\r");
	//Power control--
	//Turns on Headphone driver, dac, bias network, analog mixer, PGA(L+R) and ADC(L+R)
	vAudioSendCommand(UDA1380_PWRCTL_02, (PON_PLL|PON_HP|PON_DAC|PON_BIAS|EN_AVC|PON_AVC|PON_PGAL|PON_ADCL|PON_PGAR|PON_ADCR|PON_LNA));	
	vAudioSendCommand(UDA1380_I2SSET_01, (SFORI_I2S|SFORO_I2S|SIM_SLAVE|SEL_SOURCE_DMIXER));
	vAudioSendCommand(UDA1380_ANAMIX_03, 0xFFFF);//Analog Mixer settings (muted)
	vAudioSendCommand(UDA1380_MUTDEM_13,MT1 );
	vAudioSendCommand(UDA1380_MIXVOL_11, 0xFFFF);
	vAudioSendCommand(UDA1380_CLKSET_00,EN_DEC|EN_DAC|EN_INT|DAC_CLK|ADC_CLK|WSPLL_6_25); //changed from 12_5
	vAudioSendCommand(UDA1380_ADCSET_22,  0x04);
	vAudioSendCommand(UDA1380_PGAMUT_21, 0x00);
	vAudioSendCommand(UDA1380_TONCTL_12, EQFLAT);
	vAudioSendCommand(UDA1380_MASVOL_10, 0x1F1F);
//	vAudioSendCommand(UDA1380_MIXOVS_14, SILENCE);
	//Input and output control
	//I2S Bus as input and output, set slave and use digital mixer
	MESSAGE("Codec Configured\n\r");	
//	LPC_I2S->I2SIRQ = (1 << 0)| (4 << 8)|(4 << 16); //Enable Recieve interrupt, interrupt when FIFO is 4 levels full
//	NVIC_EnableIRQ(I2S_IRQn);
//	MESSAGE("I2S interrupt enabled\n\r");

}
void I2S_IRQHandler(void)
{
	#define I2S_RX_LEVEL 0x00000F00
	long int left, right;
	uint32_t data;
	char str[100];
	uint8_t i = 0;
	static last_sample = 0;
	double sample;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;	
	lineData output;
	output.depth = 0;
	while( (LPC_I2S->I2SSTATE & I2S_RX_LEVEL) && (output.depth < MAX_DEPTH))
	{
		output.left[output.depth] = LPC_I2S->I2SRXFIFO;
		output.right[output.depth++] = LPC_I2S->I2SRXFIFO;
	}
//	xQueueSendFromISR(lineInQueue, &output, &xHigherPriorityTaskWoken);

	portEND_SWITCHING_ISR(&xHigherPriorityTaskWoken);
}
void vVolumeControl(uint16_t volume)
{
	char str[20];
	vAudioSendCommand(UDA1380_DECVOL_20, volume);	
	MESSAGE( "0x%x\n\r", volume);
}
void vVolumeIncrease()
{
	char str[20];
	volume++;
	vVolumeControl((uint16_t)volume);	
	MESSAGE("0x%x\n\r", volume);
}
void vVolumeDecrease()
{
	volume--;
	vVolumeControl((uint16_t)volume);
}
void vBassIncrease()
{
	char str[20];
	if(bass != 0xF)
	{
		bass++;
	}
	MESSAGE( "0x%x\n\r", bass);
	vAudioSendCommand(UDA1380_TONCTL_12, bass<<8|bass);	
}
void vBassDecrease()
{
	char str[20];
	if(bass != 0)
	{
		bass--;
	}
	else
		bass = 0;
	MESSAGE( "0x%x\n\r", bass);
	vAudioSendCommand(UDA1380_TONCTL_12, bass<<8|bass);
}
uint32_t makeHex32b(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
	return ((b3 << 24) | (b2 << 16) | (b1 << 8) | b0);

}
uint8_t unsignedtosigned(uint8_t us)
{
	
	if(us & 0x80)
		us &= 0x7F;
	else
		us += 128;
	return us;

}
void readWaveFile(char * filename)
{
	FIL wave_file;
	uint8_t wave_header[44];
	uint8_t sample_rate_str[5];
	uint8_t i = 0;
	uint32_t size = 0;
	uint32_t num_channels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint32_t blockAlign;
	uint32_t BitsPerSample;
	uint32_t dataSize;
	uint32_t return_value;
	uint32_t bytes_read = 0;
	uint32_t dataOut;
	uint16_t d1, d2;
	uint8_t test_data;
	uint8_t out_data;
	uint8_t direction;
	uint8_t buffer[1024];

	#define UP 0
	#define DOWN 1
	test_data = 0;
	direction = UP;
	return_value = f_open(&wave_file, filename, FA_READ);
	if(return_value != FR_OK)
		MESSAGE("f_open failed with: %u\n\r", return_value);
	else
	{
		return_value = f_read(&wave_file, wave_header, 44, &bytes_read);
		if(return_value != FR_OK || bytes_read != 44)
			MESSAGE("f_read failed; return value: %u bytes_read: %u\n\r", return_value, bytes_read);
		else
		{
			if(wave_header[0] == 'R' &&
				wave_header[1] == 'I' &&
				wave_header[2] == 'F' &&
				wave_header[3] == 'F')
			{
				MESSAGE("Valid wave file detected\n\r");
			}	
			num_channels = wave_header[22];
			sampleRate = makeHex32b(wave_header[27], wave_header[26], wave_header[25], wave_header[24]);
			byteRate = makeHex32b(wave_header[31],wave_header[30],wave_header[29],  wave_header[28] );
			blockAlign = makeHex32b(0,0,wave_header[33], wave_header[32]);
			BitsPerSample = makeHex32b(0, 0, wave_header[35], wave_header[34]);
			dataSize = makeHex32b(wave_header[43], wave_header[42], wave_header[41], wave_header[40]);
			MESSAGE("Audio file information: \n\r\tNumber of channels: %u\n\r\tSample Rate: %u\n\r\tByte Rate: %u\n\r\tNumber of bytes for 1 sample: %u\n\r\tBits per sample: %u\n\r\tData Size: %u\n\r", num_channels, sampleRate, byteRate, blockAlign, BitsPerSample, dataSize);
			//Set up I2S interface
			//16 bit data (bw) 1 channel, 	
			d1 = 0x0000; d2 = 0x0000;
			while(1)
			{

				if(!f_eof(&wave_file))
				{
					return_value = f_read(&wave_file, buffer, 32, &bytes_read);
					if(return_value != FR_OK/* || bytes_read != 4*/)
					{
						MESSAGE("Error reading wave file data, error is: %u, number of bytes read is: %u\n\r",return_value, bytes_read);
						MESSAGE("i: %u\n\r", i);
					}
					else{
					for(i = 0; i < 64; i++)
					{
						buffer[i] =  unsignedtosigned(buffer[i]); //convert sign...
					}			
						while(((LPC_I2S->I2SSTATE & (0x0F << 16))>>16)  );
			
					for( i = 3; i < 32; i+=4)
					{
					//	while(((LPC_I2S->I2SSTATE & (0x0F << 16))>>16)  );
				
						LPC_I2S->I2STXFIFO = (buffer[i] << 24) | (buffer[i-1] << 16) | (buffer[i-2] << 8) | buffer[i-3];
					//	LPC_I2S->I2STXFIFO = (buffer[i-4] << 24) | (buffer[i-5] << 16) | (buffer[i-6] << 8) | buffer[i-7];
					//	LPC_I2S->I2STXFIFO = (buffer[i-8] << 24) | (buffer[i-9] << 16) | (buffer[i-10] << 8) | buffer[i-11];
					//	LPC_I2S->I2STXFIFO = (buffer[i-12] << 24) | (buffer[i-13] << 16) | (buffer[i-14] << 8) | buffer[i-15];
					//	LPC_I2S->I2STXFIFO = (buffer[i-16] << 24) | (buffer[i-17] << 16) | (buffer[i-18] << 8) | buffer[i-19];
					//	LPC_I2S->I2STXFIFO = (buffer[i-20] << 24) | (buffer[i-21] << 16) | (buffer[i-22] << 8) | buffer[i-23];
					///	LPC_I2S->I2STXFIFO = (buffer[i-24] << 24) | (buffer[i-25] << 16) | (buffer[i-26] << 8) | buffer[i-27];
					//	LPC_I2S->I2STXFIFO = (buffer[i-28] << 24) | (buffer[i-29] << 16) | (buffer[i-30] << 8) | buffer[i-31];
					
								
					}}
				}
				else
				{

					return_value = f_lseek(&wave_file, 44);
					if(return_value != FR_OK)
						MESSAGE("f_lseek failed\n\r");

				}
			
			}
		}
		f_close(&wave_file);
	}
	

}
void vAudioTask(void * pvParameters)
{
	long int left, right;
	uint16_t volume;
	int i = 0;
	char str[64];
	I2Ssettings data;
	lineData output;
	lineInQueue = xQueueCreate(2, sizeof(lineData));
	audioControl = xQueueCreate(1, sizeof(uint16_t));
	settings = xQueueCreate(1, sizeof(I2Ssettings));
	if(lineInQueue != NULL)
		MESSAGE("Queue Created Successfully\n\r");
	else
		MESSAGE("Queue failed to be created\n\r");
//	vAudioInit(8,1,1,3,2,32);
	vAudioInit(8,64,125,24,2,8);
	AudioReadReg(0x20);
	MESSAGE("Entering main audio task\n\r");	
	readWaveFile("test.wav");
	while(1);
	while(1)
	{
		if(uxQueueMessagesWaiting(audioControl) > 0)
		{
			xQueueReceive(audioControl, &volume, 0);
			vVolumeControl(volume); 
		}
		if(uxQueueMessagesWaiting(settings) > 0)
		{
			xQueueReceive(settings, &data, 0);
			vAudioInit(data.Pclk_Div, data.x, data.y, data.tx_bitrate, data.nChan, data.bw);
			MESSAGE("Pclk: %u, x: %u, y: %u, tx_br: %u, nchan: %u, bw: %u\n\r", data.Pclk_Div, data.x, data.y,  data.tx_bitrate, data.nChan, data.bw);
		}
		if(uxQueueMessagesWaiting(lineInQueue) > 0)
		{
			xQueueReceive(lineInQueue, &output, 0);
			i = 0;
			while(i < output.depth)
			{
				left = output.left[i];
				right = output.right[i++];	
				LPC_GPIO1->FIOPIN ^= LED3;		
				LPC_I2S->I2STXFIFO = left;
				LPC_I2S->I2STXFIFO = right;
			}
		}
		
	}

}
