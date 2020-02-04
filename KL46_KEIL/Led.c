#include "Led.h"

void LED_Init(void)
{
  int d=0; 
	SIM->SCGC5 |=  SIM_SCGC5_PORTE_MASK; 	
	PORTE->PCR[29] = PORT_PCR_MUX(1);    
	FPTE->PDDR = 1UL<<29;
	FPTE->PCOR = 1UL<<29;
	for(d=0; d<1200000; d++);
	FPTE->PSOR = 1UL<<29	;          
	d=0;

	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; 	
	PORTD->PCR[5] = PORT_PCR_MUX(1);    
	FPTD->PDDR = 1UL<<5;
	FPTD->PCOR = 1UL<<5;
	for(d=0; d<1200000; d++);
	FPTD->PSOR = 1UL<<5	;          
	
}
