uint8_t SetPinInput(uint8_t port, uint8_t index);
uint8_t SetPinOutput(uint8_t port, uint8_t index);
uint8_t SetPin(uint8_t port, uint8_t index);
uint8_t ClrPin(uint8_t port, uint8_t index);
uint8_t TogPin(uint8_t port, uint8_t index);
uint8_t ReadPin(uint8_t port, uint8_t index);
uint8_t vGPIOIntPending(uint8_t port, uint32_t pin);
uint8_t vGPIOIntClr(uint8_t port, uint32_t pin);
uint8_t GPIOInterruptEnable(uint8_t port, uint32_t pin, uint8_t rising);
uint8_t getGPIOIntStatusR(uint8_t port, uint32_t pin);
uint8_t getGPIOIntStatusF(uint8_t port, uint32_t pin);

