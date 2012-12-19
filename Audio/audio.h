#define UDA1380_ADDR 0x30 //I2C address of the codec chip
#define UDA1380_CLKSET_00 0x00
	#define EN_ADC (1 << 11)
	#define EN_DEC (1 << 10)
	#define EN_DAC (1 << 9)
	#define EN_INT (1 << 8)
	#define ADC_CLK (1 << 5)
	#define DAC_CLK (1 << 4)
	#define SYS_DIV256 ( 0x0 << 2)
	#define SYS_DIV384 ( 0x1 << 2)
	#define SYS_DIV512 ( 0x2 << 2)
	#define SYS_DIV768 ( 0x3 << 2)
	#define WSPLL_6_25 (0x0 << 0)
	#define WSPLL_12_5 (0x1 << 0)
	#define WSPLL_25 (0x2 << 0)
	#define WSPLL_50 (0x3 << 0)
#define UDA1380_I2SSET_01 0x01
	#define SFORI_I2S (0x0 << 8)	//I2S bus as input
	#define SFORI_LSB16 (0x01 << 8)		//LSB Justfied, 16b
	#define SFORI_LSB18 (0x02 << 8)
	#define SFORI_LSB20 (0x03 << 8)
	#define SFORI_MSB (0x05 << 8)		//MSB Justified
	#define SFORO_I2S (0x0 << 0)	//I2C bus as output
	#define SFORO_LSB16 (0x01 << 0)
	#define SFORO_LSB18 (0x02 << 0)
	#define SFORO_LSB20 (0x03 << 0)
	#define SFORO_MSB (0x05)
	#define SEL_SOURCE_DECIMATOR (1 << 6) //Deciamtor as source
	#define SEL_SOURCE_DMIXER (0 << 6) //Digital Mixer
	#define SIM_MASTER (1 << 4) //Mode of digital output interface
	#define SIM_SLAVE (0 << 4) 
#define UDA1380_PWRCTL_02 0x02
	#define PON_PLL (1 << 15) //Power on
	#define PON_HP (1 << 13) //Power on headphone driver
	#define PON_DAC (1 << 10)
	#define PON_BIAS ( 1 << 8) //ADC, AVC, FSDAC
	#define EN_AVC	(1 << 7) //Enable analog mixer
	#define PON_AVC (1 << 6) //Power on analog mixer
	#define PON_LNA (1 << 4) 
	#define PON_PGAL (1 << 3) //PGA left
	#define PON_ADCL (1 << 2) //ADC left
	#define PON_PGAR (1 << 1)
	#define PON_ADCR (1 << 0)	
#define UDA1380_ANAMIX_03 0x03 //Analog volume control
	#define ANAMUTE (0x3F << 8)|0x3F
#define UDA1380_HPHONE_04 0x04 //headphone amplifier settings
	#define EN_SCP (1 << 1) //Short circuit protection	
#define UDA1380_MASVOL_10 0x10 //Master Volume Control
#define UDA1380_MIXVOL_11 0x11 //Mixer Volume Control
#define UDA1380_TONCTL_12 0x12 //EQ control
	#define EQFLAT (0x00 << 14)
	#define EQMIN1 (0x01 << 14)
	#define EQMIN2 (0x02 << 14)
	#define EQMAX (0x03 << 14)
#define UDA1380_MUTDEM_13 0x13
	#define MTM (1 << 14) //Master mute
	#define MT2 (1 << 11) // Channel 2 mute
	#define DE2_CH2_OFF ( 0 << 8)
	#define DE2_CH2_32k	( 1 <<8)
	#define DE2_CH2_44_1k (2 << 8)
	#define DE2_CH2_48k	(3 << 8)
	#define DE2_CH2_96k (4 << 8)
	#define MT1 (1 << 3) //Channel 1 Mute
	#define DE2_CH1_OFF ( 0 << 0)
	#define DE2_CH1_32k	( 1 <<0)
	#define DE2_CH1_44_1k (2 << 0)
	#define DE2_CH1_48k	(3 << 0)
	#define DE2_CH1_96k (4 << 0)
#define UDA1380_MIXOVS_14 0x14	//Mixer, Silence detector and oversampling settings
	#define DA_POL_INV (1 << 15)
	#define SEL_NS ( 1 << 14)
	#define SILENCE (1 << 7)
	#define SDET_ON ( 1<< 6)
#define UDA1380_DECVOL_20 0x20 //Decimator volume control
#define UDA1380_PGAMUT_21 0x21
#define UDA1380_ADCSET_22 0x22
	#define ADCPOL_INV (1 << 12)
	#define SEL_LNA (1 << 3) //Line input select
	#define SEL_MIC (1 << 2) //Microphone input slect
	#define SKIP_DCFIL (1 << 1) //Bypass DC Filter (before decimator)
	#define EN_DCFIL ( 1 << 0) //Enable DC filter at output of decimator
#define UDA1380_AGCSET_23 0x23 //AGC settings

#define UDA1380_SW_RESET 0x7F
typedef struct{
	unsigned int Pclk_Div;
	unsigned int x;
	unsigned int y;
	unsigned int tx_bitrate;
	unsigned int nChan;
	unsigned int bw;
} I2Ssettings;


void vAudioInit();
void vAudioTask(void* pvParameters);

