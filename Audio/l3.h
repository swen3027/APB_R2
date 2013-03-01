void mcp_init(uint8_t static_mode);
void static_pin_init();
void l3init();
void l3SendCmd(uint8_t address, uint8_t data);
void uda1345TS_init();

#define SETL3MODE {LPC_GPIO1->FIOPIN |= (1 << 22);}
#define CLRL3MODE {LPC_GPIO1->FIOPIN &= ~(1 << 22);}
