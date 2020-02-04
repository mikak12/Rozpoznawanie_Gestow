/*----------------------------------------------------------------------------
      
 *      Author: Dominik Starzyk, Dawid Ilba
 *---------------------------------------------------------------------------*/

#include "MKL46Z4.h"                    
#include <string.h>
#include "I2C.h"
#include "Led.h"
#include "UART.h"
#include "Accelerometer.h"
#include "Clock.h"
#include "GPIO.h"


unsigned char single_byte;

char tmp[1];

int main (void) 
{
	char *stringsConsole[]= {"Start Config \r\n",
													"Finish Config \r\n",
													"Start Application \r\n"};

	SysClock_Init();
	uart0Init();
//	UART0_TransmitBuffer(stringsConsole[0], strlen(stringsConsole[0]));		 											
	LED_Init();
	GPIO_init();
	I2C_Init();
	buttonsInitialize();
//	UART0_TransmitBuffer(stringsConsole[1], strlen(stringsConsole[1]));
	
	ACC_Init();	
	ACC_Calibration();
													
//	UART0_TransmitBuffer(stringsConsole[2], strlen(stringsConsole[2]));
//	CheckCalibration();
//	Status_Register();
	
	
								
	while(1)
	{	

			if(!button1Read())
				{
					if(DataReady== 1)
					{
						DataReady=0;
						Accelerometer_data_transmition();	
						

					}
				}

 }
}

