void PWM_Init(uint32_t prescale, uint32_t cycle_time, uint32_t cycle_high, unsigned char pclk_div);

void PWM_set(uint32_t prescale, uint32_t cycle_time, uint32_t cycle_high);
void enablePWM();
void disablePWM();
//SC->PCONP
#define PWM_PWR (1 << 6)
//PINCON->PINSEL4
#define PWM_PCLKSEL_OFFSET 12
#define PWM_PINSEL_MASK (0x0FFF)
#define PWM_PINSEL_P11 (0x01 << 0)
#define PWM_PINSEL_P12 (0x01 << 2)
#define PWM_PINSEL_P13 (0x01 << 4)
#define PWM_PINSEL_P14 (0x01 << 6)
#define PWM_PINSEL_P15 (0x01 << 8)
#define PWM_PINSEL_P16 (0x01 << 10)
//PWM Interrupt register (PWM1IR 0x40018000)
#define PWMMR0_INT	(1 << 0)
#define PWMMR1_INT	(1 << 1)
#define PWMMR2_INT	(1 << 2)
#define PWMMR3_INT	(1 << 3)
#define PWMMR4_INT	(1 << 8)
#define PWMMR5_INT	(1 << 9)
#define PWMMR6_INT	(1 << 10)
#define PWMCAP0_INT	(1 << 4)
#define PWMCAP1_INT	(1 << 5)
//PWM Timer Control Register (PWM1TCR 0x40018004)
#define COUNTER_ENABLE	(1 << 0)
#define COUNTER_RESET	(1 << 1)
#define PWM_ENABLE	(1 << 3)
//PWM Count Control Register (PWM1CTCR 0x40018070)
#define TIMER_MODE	(0x00)
#define COUNTER_MODE_RISING	(0x01)
#define COUNTER_MODE_FALLING	(0x02)
#define COUNTER_MODE_BOTH	(0x03)
#define COUNT_INPUT_SEL_PCAP0	(0x00 << 2)
#define COUNT_INPUT_SEL_PCAP1	(0x01 << 2)
//PWM Match Control Register (PWM1MCR 0x40018014)
#define PWMMR0I (1 << 0)
#define PWMMR0R	(1 << 1)
#define PWMMR0S (1 << 2)
#define PWMMR1I (1 << 3)
#define PWMMR1R	(1 << 4)
#define PWMMR1S (1 << 5)
#define PWMMR2I (1 << 6)
#define PWMMR2R	(1 << 7)
#define PWMMR2S (1 << 8)
#define PWMMR3I (1 << 9)
#define PWMMR3R	(1 << 10)
#define PWMMR3S (1 << 11)
#define PWMMR4I (1 << 12)
#define PWMMR4R	(1 << 13)
#define PWMMR4S (1 << 14)
#define PWMMR5I (1 << 15)
#define PWMMR5R	(1 << 16)
#define PWMMR5S (1 << 17)
#define PWMMR6I (1 << 18)
#define PWMMR6R	(1 << 19)
#define PWMMR6S (1 << 20)

//PMW Capture Control Register (PWM1CCR - 0x40018028)
#define CAPn0_RISING	(1 << 0)
#define CAPn0_FALLING	(1 << 1)
#define CAPn0_EVENT	(1 << 2)
#define CAPn1_RISING	(1 << 3)
#define CAPn1_FALLING	(1 << 4)
#define CAPn1_EVENT	(1 << 5)

//PWM Control Register (PWM1PCR - 0x4001804C)
#define PWMSEL2_DOUBLE_EDGE	(1 << 2)
#define PWMSEL3_DOUBLE_EDGE	(1 << 3)
#define PWMSEL4_DOUBLE_EDGE	(1 << 4)
#define PWMSEL5_DOUBLE_EDGE	(1 << 5)
#define PWMSEL6_DOUBLE_EDGE	(1 << 6)
#define PWMENA1			(1 << 9)
#define PWMENA2			(1 << 10)
#define PWMENA3			(1 << 11)
#define PWMENA4			(1 << 12)
#define PWMENA5			(1 << 13)
#define PWMENA6			(1 << 14)

//PWM Latch Enable Register (PWM1LER - 0x40018050)
#define ENB_PWM_MATCH0		(1 << 0)
#define ENB_PWM_MATCH1		(1 << 1)
#define ENB_PWM_MATCH2		(1 << 2)
#define ENB_PWM_MATCH3		(1 << 3)
#define ENB_PWM_MATCH4		(1 << 4)
#define ENB_PWM_MATCH5		(1 << 5)
#define ENB_PWM_MATCH6		(1 << 6)

#define PWM_CYCLE		1000
#define PWM_HIGH		250
