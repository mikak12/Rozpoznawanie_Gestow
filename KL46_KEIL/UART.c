#include "UART.h"
#include "MKL46Z4.h"

void uart0Init()
{
	uint32_t divisor;
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA -> PCR[1] |= PORT_PCR_MUX(2);
	PORTA -> PCR[2] |= PORT_PCR_MUX(2);
	
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(2);

	UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	
	UART0 -> C4 |= UART0_C4_OSR(31);                  //31
	
	divisor = bus_clock/((31+1)*baude_rate);
	UART0->BDH = UART_BDH_SBR(divisor >> 8);
	UART0->BDL = UART_BDL_SBR(divisor );
	
	UART0->C1 = UART0->S2 = UART0->C3 = 0;
	
	UART0 ->BDH |= UART0_BDH_SBNS_MASK;

	UART0->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void UART0_Transmit(unsigned char data)
{
	while (!(UART0->S1 & UART_S1_TDRE_MASK));
	UART0->D = data;
}

void UART0_TransmitBuffer(char *buffer, uint8_t length)
{
	uint8_t lenCnt = 0;
	char endLine[] = "\r\n";
	for(lenCnt = 0; lenCnt < length; lenCnt++)
	{
		UART0_Transmit(buffer[lenCnt]);
	}
	//UART0_Transmit(endLine[0]);  
	//UART0_Transmit(endLine[1]);
}

uint8_t UART0_Receive(void)
{
	while (!(UART0->S1 & UART_S1_RDRF_MASK));
	return UART0->D;
}
