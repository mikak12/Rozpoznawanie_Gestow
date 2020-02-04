#include "buttons.h"


#define SW1_PIN 3												
#define SW3_PIN 12


#define PORTC_D_IRQ_NBR 31 //(IRQn_Type) -1			

volatile uint8_t ButtonOn;   //interrupt flag


/*----------------------------------------------------------------------------
	Interrupt service routine
	Button will cause PORTC_PORTD interrupt 
 *----------------------------------------------------------------------------*/
void PORTC_PORTD_IRQHandler(void){							
	
	while((FPTC->PDIR&(1<<SW1_PIN))==0);					
	
	PORTC->PCR[SW1_PIN] |= PORT_PCR_ISF_MASK; 

	ButtonOn = 1;
}

/*----------------------------------------------------------------------------
	Function initializes port C pin for switch 1 (SW1) handling
	and enables PORT_C_D interrupts
 *----------------------------------------------------------------------------*/
void buttonsInitialize(void){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTC_MASK; 					
	PORTC->PCR[SW1_PIN] |= PORT_PCR_MUX(1);      	
	

	PORTC->PCR[SW1_PIN] |=  PORT_PCR_PE_MASK |		
											PORT_PCR_PS_MASK;					
	PORTC->PCR[SW1_PIN] |= 	PORT_PCR_IRQC(0x9);		
		
	
	NVIC_ClearPendingIRQ(PORTC_D_IRQ_NBR);				
	NVIC_EnableIRQ(PORTC_D_IRQ_NBR);							
	
	
	NVIC_SetPriority (PORTC_D_IRQ_NBR, 2);						

}

/*----------------------------------------------------------------------------
	Function reads SW 1 state
*----------------------------------------------------------------------------*/
//int32_t buttonRead(){
//	return FPTC->PDIR & (1UL<<SW1_PIN);						/* Get port data input register (PDIR) */
//}	

///*----------------------------------------------------------------------------
//	Function services PORT_C_D interrupt
//*----------------------------------------------------------------------------*/
//void buttonsService(void){
//	//fastSlowFSM();																/* Toggle leds FSM spee mode */ 
//}

