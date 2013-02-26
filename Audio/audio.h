
void audio_init(void);
void mcp_init(uint8_t);
void static_pin_init();
void l3init();
void uda1345TS_init();
void I2Sinit();
void enableI2S();
void disableI2S();
uint8_t unsignedtosigned(uint8_t us);
void SetDMA(uint32_t * array);
void DMAinit();
uint32_t samplesToWord(uint8_t* samples);

uint32_t samplesToWord(uint8_t *);
void vAudioTask(void *);
void vAudioRXTask(void *);

