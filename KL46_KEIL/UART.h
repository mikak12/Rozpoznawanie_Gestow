#ifndef UART_h
#define UART_h

#include "MKL46Z4.h"

#define baude_rate 9600
#define bus_clock 8000000

void uart0Init(void);
void UART0_Transmit(uint8_t data);
uint8_t UART0_Receive(void);
void UART0_TransmitBuffer(char *buffer, uint8_t length);

#endif
