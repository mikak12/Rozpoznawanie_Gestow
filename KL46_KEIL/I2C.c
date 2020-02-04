#include "I2C.h"
#include <string.h>

unsigned char DATA_READ[6];

void I2C_Init(void)                     
{	
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;       //clock on portE
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK ;     	//clock on I2C

	PORTE->PCR[24]|= PORT_PCR_MUX(5);         // Setting Pins to I2C0 SCL and SDA
	PORTE->PCR[25]|= PORT_PCR_MUX(5);

	
	PORTE->PCR[24]|= PORT_PCR_PS_MASK |PORT_PCR_PE_MASK;         // Enabling pullup resistor sda and scl pins
	PORTE->PCR[25]|= PORT_PCR_PS_MASK |PORT_PCR_PE_MASK;       
	
	
	I2C0->F|=I2C_F_ICR(0x14);          //SDA = 2.125u       SCL start hold time = 4.25    SCL stop   - 5.125
	
	I2C0->C1|=I2C_C1_IICEN_MASK;       // Enable the module
}

void I2C_Scanner(void)              //funtion to search adress slave which are connect to I2C module
{
	uint8_t devAddr = 0;
	uint8_t addrCnt = 0;
	for(devAddr = 0; devAddr < 128; devAddr++)
	{
		if(I2C_AddressRead(devAddr) == 0)
		{
			addrCnt++;
		}
	}
}

void Status_ACK_NACK(void)            //funtion to return ACK or NACK after send Slave adress on I2C module
{
	unsigned char AdressSlave= 0x1D;         // adress do wysylki 
	uint16_t adresssslave= 0x1D;
	char AdressSlave2[]= {0,'x',1,'D'};   //ten sam adres rozbity na znaki
  int a, j, k;	  

	uint16_t i;
  unsigned char NACK[] = {78, 65, 67, 75};   //zakodowane N A C K  w ascii
	unsigned char ACK[] = {65, 67, 75};	       //zakodowane A C K w ascii

	for(k=0; k<strlen(AdressSlave2); k++)   // wysylka adresu
	{
		uint8_t p = AdressSlave2[k] - '0';
		UART0_Transmit(p);
	}
	
	UART0_Transmit(32);                         //space
	
	if(I2C_AddressRead(AdressSlave) == 255)     //conditional if  NACK - nie  ACK - tak
	{
		for(i=0; i<4; i++)
			{
				UART0_Transmit(NACK[i]);
			}
	}
	else
	{
				for(i=0; i<3; i++)
			{
				UART0_Transmit(ACK[i]);
			}
	}


	
}

/*========================================================*/
/* Input:  Device Address */
/*========================================================*/

unsigned char I2C_AddressRead(unsigned char DEV_ADR)
{   
	unsigned char DATA_DUMMY =0;
	unsigned char DATA =0;
	uint32_t timeout = 99999;
	
	I2C0->C1 |=I2C_C1_TX_MASK;             //Set I2C in Transmit mode
	
	I2C0->C1 |=I2C_C1_MST_MASK;            // Send Start bit
	
	I2C0->D = WRITE(DEV_ADR);              // Send Device Address Write
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)  //Wait for Transfer to complete
	{
		timeout--;
		if(timeout == 0)
		{
			return 255;
		}
	}
	timeout = 99999;
	
	I2C0->S |= I2C_S_IICIF_MASK;              //Clear IICIF Flag
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)    //Wait for ACK from slave
	{
		timeout--;
		if(timeout == 0)
		{
			return 255;
		}
	}
	
	I2C0->C1 &= (~I2C_C1_MST_MASK);
	
	I2C0->C1 &= ~(I2C_C1_TXAK_MASK);     // Clear Transmit Nack (setting TXAK to 0)
	
	
	return 0;
}

/*========================================================*/
/* Input:  Device Address, Register Address*/
/* Output: Register Data*/
/* =======================================================*/
unsigned char I2C_SingleByteRead(unsigned char DEV_ADR, unsigned char REG_ADR)
{   
	unsigned char DATA_DUMMY =0;
	unsigned char DATA =0;
	uint32_t timeout = 99999;
	
	I2C0->C1 |=I2C_C1_TX_MASK;    //Set I2C in Transmit mode
	
	I2C0->C1 |=I2C_C1_MST_MASK;   // Send Start bit
	
	I2C0->D = WRITE(DEV_ADR);     // Send Device Address Write
	timeout = 999999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)    //Wait for Transfer to complete
	{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
	}
	
	I2C0->S |= I2C_S_IICIF_MASK;          //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)    //Wait for ACK from slave
	{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
	}
	
	I2C0->D = REG_ADR;         // Send register address that we want to read
	timeout = 999999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)    //Wait for Transfer to complete
	{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
	}
	
	I2C0->S |= I2C_S_IICIF_MASK;         //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)     //Wait for ACK from slave
	{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
	}
	
	I2C0->C1 |= I2C_C1_RSTA_MASK;         // repeated Start

	I2C0->D = READ(DEV_ADR);               // Send Device Address Write
	
	I2C0->C1 |= (I2C_C1_TXAK_MASK);      //Send NAK
	timeout = 999999;
	
  while((I2C0->S & I2C_S_IICIF_MASK) == 0)        //Wait for Transfer to complete
	{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
	}
    
	I2C0->S|= I2C_S_IICIF_MASK;              //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)      //Wait for ACK from slave
		{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
		}
	
	I2C0->C1 &= (~I2C_C1_TX_MASK);          //Set on I2C receiver mode
	
	DATA_DUMMY = I2C0->D;              //Read Dummy acceler data
		timeout = 999999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)       //Wait for Transfer to complete
		{
		timeout--;
		if(timeout <= 1)
		{
			return 0;
		}
		}
	
	I2C0->S |= I2C_S_IICIF_MASK;      //Clear IICIF Flag
	
	I2C0->C1 &= (~I2C_C1_MST_MASK);   //Send Stop Bit
	 
	I2C0->C1 &= ~(I2C_C1_TXAK_MASK);  // Clear Transmit Nack (setting TXAK to 0)
	
	DATA= I2C0->D;                 //Read data
	
	SysTick_Delay(10);              
	
	return DATA;
}



/*========================================================*/
/* Input:  Device Address, Register Address of Starting Location ,Number of Bytes to be Read*/
/* Output: DATA_READ[] array with read data in our program we'll read 6byte with X,Y,Z axx data*/
/* =======================================================*/

void I2C_MultipleByteRead(unsigned char DEV_ADR,unsigned char REG_ADR, int max_count)
{   
	int count = 0;
	unsigned char DATA_DUMMY =0;unsigned char DATA =0;unsigned char data_output[6];
	uint32_t timeout = 99999;
	
	I2C0->C1|=I2C_C1_TX_MASK;         //Set I2C in Transmit mode
	
	I2C0->C1|=I2C_C1_MST_MASK;         // Send Start bit
	
	I2C0->D= WRITE(DEV_ADR);             // Send Device Address Write
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)     //Wait for Transfer to complete
		{		
			timeout--;
			if(timeout <= 1)
			{	
				return;				
			}
		}
	
	I2C0->S|= I2C_S_IICIF_MASK;             //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)    //Wait for ACK from slave
		{
			timeout--;
			if(timeout <= 1)
			{		
				return;
			}
		}
	
	I2C0->D=REG_ADR;                      // Send register address that we want to read
	timeout = 999999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)         //Wait for Transfer to complete
		{
			timeout--;
			if(timeout <= 1)
			{		
				return;
			}
		}
	
	I2C0->S|= I2C_S_IICIF_MASK;              //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)        //Wait for ACK from slave
		{
			timeout--;
			if(timeout <= 1)
			{		
				return;
			}
		}
	
	I2C0->C1|=I2C_C1_RSTA_MASK;               // repeated Start
	
	I2C0->D=READ(DEV_ADR);                // Send Device Address Write
	timeout = 999999;

	
    while((I2C0->S & I2C_S_IICIF_MASK) == 0)             //Wait for Transfer to complete
		{
			timeout--;
			if(timeout <= 1)
			{	
				return;				
			}
		}
    
	I2C0->S|= I2C_S_IICIF_MASK;                 //Clear IICIF Flag
	timeout = 999999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)          //Wait for ACK from slave
			{
			 timeout--;
			 if(timeout <= 1)
			 {		
				 return;
			 }
			}
	
	I2C0->C1&=(~I2C_C1_TX_MASK);                       //Set on I2C receiver mode
	
	DATA_DUMMY= I2C0->D;                               //Read Dummy acceler data
	timeout = 9999999;
	for(count = 0;count < max_count;count++)
			{
		if(count<(max_count-2))
				{
				 while((I2C0->S & I2C_S_IICIF_MASK) == 0)        //Wait for Transfer to complete
						{
							timeout--;
							if(timeout <= 1)
							{		
								return;
							}
						}
				 
				 I2C0->S|= I2C_S_IICIF_MASK;                //Clear IICIF Flag
				DATA_READ[count]=I2C0->D;
				}
		else
				{
					
					I2C0->C1|=(I2C_C1_TXAK_MASK);              // Clear Transmit Nack (setting TXAK to 0)
					timeout = 9999999;
					
					while((I2C0->S & I2C_S_IICIF_MASK) == 0)     //Wait for Transfer to complete
							{
								timeout--;
							 if(timeout <= 1)
							 {
									return;
							 } 
							}
					DATA_READ[count]=I2C0->D;
					count = count+1;
					timeout = 9999999;
					
					while((I2C0->S & I2C_S_IICIF_MASK) == 0)       //Wait for Transfer to complete
						{
							timeout--;
							if(timeout <= 1)
							{		
								return;
							}
						}
					
					I2C0->S|= I2C_S_IICIF_MASK;        //Clear IICIF Flag
					
					I2C0->C1&=(~I2C_C1_MST_MASK);      // Send the stop signal
					
					I2C0->C1&=(~I2C_C1_TXAK_MASK);      // Clear Transmit Nack (setting TXAK to 0)
					
					DATA_READ[count]=I2C0->D;          //Last byte read

				}
			}
	
	SysTick_Delay(10);
			

}
/*========================================================*/
/* Input:  Device Address, Register Address, Data*/
/* =======================================================*/
void I2C_SingleByteWrite(unsigned char DEV_ADR, unsigned char REG_ADR, unsigned char DATA)
{
		uint32_t timeout = 99999;
	
	I2C0->C1|=I2C_C1_TX_MASK;     //Set I2C in Transmit mode
	
	I2C0->C1|=I2C_C1_MST_MASK;     // Send Start bit
	 
	I2C0->D= WRITE(DEV_ADR);     // Send Device Address
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)    //Wait for Transfer to complete
		{
			timeout--;
			if(timeout <= 1)
		 {		
			return;
		 }
		}
	
	I2C0->S|= I2C_S_IICIF_MASK;            //Clear IICIF Flag
	timeout = 99999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)          //Wait for ACK from slave
		{
				timeout--;
			if(timeout <= 1)
		 {		
			return;
		 }
		}
		
	I2C0->D=REG_ADR;            // Send Register Address
	timeout = 99999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)    //Wait for Transfer to complete
		{
			timeout--;
			if(timeout <= 1)
		 {		
			return;
		 }
		}

	I2C0->S|= I2C_S_IICIF_MASK;        //Clear IICIF Flag
	timeout = 99999;

	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)      //Wait for ACK from slave
		{
			timeout--;
			if(timeout <= 1)
		 {		
			return;
		 }
		}
	
	I2C0->D=DATA;       // Send single byte of data
	timeout = 99999;
	
    while((I2C0->S & I2C_S_IICIF_MASK) == 0)       //Wait for Transfer to complete
		{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
		}
    
	I2C0->S|= I2C_S_IICIF_MASK;      //Clear IICIF Flag
	timeout = 99999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)      //Wait for ACK from slave
			{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
			}
	
	I2C0->C1&=(~I2C_C1_MST_MASK);         //Send Stop Bit

	SysTick_Delay(10);
}
/*========================================================*/
/* Input:  Device Address,   Register Address of Start Location,*/
/*         Number of Bytes to be Written,    Data*/
/* =======================================================*/
void I2C_MultipleByteWrite(unsigned char DEV_ADR, unsigned char REG_ADR, int max_count, unsigned char data_wr[])
{
	int count = 0;
	uint32_t timeout = 99999;
	
	I2C0->C1|=I2C_C1_TX_MASK;       
	
	I2C0->C1|=I2C_C1_MST_MASK;       
	
	I2C0->D= WRITE(DEV_ADR);
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)
		{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
		}
	
	I2C0->S|= I2C_S_IICIF_MASK;
	timeout = 99999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)
		{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
		}
	
	I2C0->D=REG_ADR;
	timeout = 99999;
	
	while((I2C0->S & I2C_S_IICIF_MASK) == 0)
		{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
		}
	
	I2C0->S|= I2C_S_IICIF_MASK;
	timeout = 99999;
	
	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)
		{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
		}
	
	for(count = 0;count < max_count;count++)
		{
		I2C0->D=data_wr[count];
		timeout = 99999;
		
    	while((I2C0->S & I2C_S_IICIF_MASK) == 0)
			{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
			}
    	
    	I2C0->S|= I2C_S_IICIF_MASK;
		  timeout = 99999;
    
    	while ((I2C0->S & I2C_S_RXAK_MASK) != 0)
			{
				timeout--;
				if(timeout <= 1)
			 {		
				return;
			 }
			}
		}

	I2C0->C1&=(~I2C_C1_MST_MASK);
	
  SysTick_Delay(10);
}

