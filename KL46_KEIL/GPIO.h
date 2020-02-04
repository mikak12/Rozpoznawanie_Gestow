#include "MKL46Z4.h"
#include "I2C.h"


extern volatile uint8_t DataReady;


void GPIO_init(void);
void PORTC_IRQHandler(void);

void buttonsInitialize(void);
