
#include "Accelerometer.h"
#include "I2C.h"
#include "Clock.h" 
#include "UART.h"


#define CTRL_REG1 				0x2A
#define CTRL_REG2					0x2B
#define CTRL_REG3					0x2C
#define CTRL_REG4					0x2D
#define CTRL_REG5					0x2E
#define XYZ_DATA_CFG_REG 	0x0E
#define SENSITIVITY_2G		4096

#define OFF_Z_REG					0x31
#define OFF_Y_REG					0x30
#define OFF_X_REG					0x2F

#define OUT_X_MSB_REG			0x01
#define STATUS_REG				0x00

#define ASLP_Count        0x29

short Xout_14_bit;
short Yout_14_bit;
short Zout_14_bit;
uint8_t Xoffset;
uint8_t Yoffset;
uint8_t Zoffset;

float xAx;
float yAx;
float zAx;

short xAxx;
short yAxx;
short zAxx;


// Defining variables to be used for acquiring and processing Magnetometer Data
extern unsigned char DATA_READ[6];

// Defining variables to be used for acquiring and processing Accelerometer Data
short int ACC_DATA_READ_AXIS[3];
short int ACC_DATA_OFFSET_AXIS[3];
short int ACC_DATA_CALIBRATED[3];
unsigned char DR_STATUS_DATA_ACC;

typedef union
{
	short int data;
	char bytes[2];
}shortIntToChar;

typedef union
{
	char bytes[4];
	float data;
}floatToBytes;

typedef union
{
	char bytes[4];
	float data;
}floatToBytesY;

typedef union
{
	char bytes[4];
	float data;
}floatToBytesZ;




uint8_t CheckCalibration(void)
{
	uint8_t status = 0;
	if(ACC_DATA_CALIBRATED[2] < 4096*0.9 || ACC_DATA_CALIBRATED[2] > 4096*1.1)
	{
		status = 1;
		UART0_TransmitBuffer("Ncalib", 6);
	}
	else 
	{
		status = 0;
		UART0_TransmitBuffer("Calib", 5);
	}
	return status;
}

uint8_t Status_Register(void)
{
	unsigned char sysmode_answer;
			// Reading data from accelerometer along X, Y, Z axes
	sysmode_answer = I2C_SingleByteRead(ACC_DEVICE_ADDRESS, SYSMODE_REG);
		
	if(sysmode_answer == 0)
	{
		UART0_TransmitBuffer("standby", 6);
		return 0;
	}
	else if(sysmode_answer == 1)
	{
		UART0_TransmitBuffer("wake", 4);
		return 1;
	}
	else 
	{
		UART0_TransmitBuffer("off", 3);
		return 2;
	}
}

uint8_t Check_Register(uint8_t adres)
{
	unsigned char answer;
	answer = I2C_SingleByteRead(ACC_DEVICE_ADDRESS, adres);
	
	return answer;
	
}


void Accelerometer_data_transmition(void) 
{	
	int loop_count = 0;
	char data[6];
	floatToBytes conv;
	floatToBytesY convy;
	floatToBytesZ convz;
	
		
	I2C_MultipleByteRead(ACC_DEVICE_ADDRESS, ACC_OUT_X_MSB, 6);    // Reading data from accelerometer  X, Y, Z axes
		
	for(loop_count=0;loop_count<6; loop_count++)    
	{
		data[loop_count] = (DATA_READ[loop_count]);
	}
	
	xAxx = ((short)(data[0] << 8) | data[1]) >> 2;    //parser data to 14bit
	xAx = (float)((float)xAxx/4096);                  // convert to float 
	conv.data=xAx;                                     //convert to 4byte
	UART0_TransmitBuffer(conv.bytes, 4);              //send
	
	SysTick_Delay(10);
	yAxx = ((short)(data[2] << 8) | data[3]) >> 2;
	yAx = (float)((float)yAxx/4096);
	convy.data=yAx;
	UART0_TransmitBuffer(convy.bytes, 4);
	
	SysTick_Delay(10);
	zAxx = ((short)(data[4] << 8) | data[5]) >> 2;
	zAx = (float)((float)zAxx/4096);
	convz.data=zAx;
	UART0_TransmitBuffer(convz.bytes, 4);
	SysTick_Delay(10);
	
}

void ACC_Init(void)
{
	unsigned char reg_val = 0;
	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG2, 0x40);           // Reset all registers to POR values

	do            // Wait for the RST bit to clear
	{
		reg_val = I2C_SingleByteRead(ACC_DEVICE_ADDRESS, CTRL_REG2) & 0x40;
	}	while (reg_val); 

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, XYZ_DATA_CFG_REG, 0x00);    // +/-2g range
	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG2, 0x02);           // High Resolution mode 
	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG1, 0x3D);           // ODR = 1.56Hz, Reduced noise, Active mode  	
}


void ACC_Calibration (void)
{
unsigned char reg_val = 0;

	while (!reg_val)                                                          // Wait for a first set of data         
	{
		reg_val = I2C_SingleByteRead(ACC_DEVICE_ADDRESS, STATUS_REG) & 0x08;  
	}

	I2C_MultipleByteRead(ACC_DEVICE_ADDRESS, OUT_X_MSB_REG, 6);              // Read data output registers 0x01-0x06
																		
	Xout_14_bit = ((short) (DATA_READ[0]<<8 | DATA_READ[1])) >> 2;           // 14-bit X-axis

	Yout_14_bit = ((short) (DATA_READ[2]<<8 | DATA_READ[3])) >> 2;           // 14-bit Y-axis 

	Zout_14_bit = ((short) (DATA_READ[4]<<8 | DATA_READ[5])) >> 2;           // 14-bit Z-axis

															 
	Xoffset = Xout_14_bit / 8 * (-1);                                      //  X-axis offset 

	Yoffset = Yout_14_bit / 8 * (-1);                                      //  Y-axis offset 

	Zoffset = (Zout_14_bit - SENSITIVITY_2G) / 8 * (-1);                  //   Z-axis offset

															 
	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG1, 0x00);             // Standby mode to allow writing to the offset registers 

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, OFF_X_REG, Xoffset);        

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, OFF_Y_REG, Yoffset); 

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, OFF_Z_REG, Zoffset); 
	
	
	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG3, 0x00);             // wake from sleep low interrupt

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG4, 0x01);             // Enable interrupt    0x01

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG5, 0x01);             // interrupt line to INT1 - PTC15

	I2C_SingleByteWrite(ACC_DEVICE_ADDRESS, CTRL_REG1, 0x2D);             //0x3D-- 0dr 1.56            // ODR = 12Hz, Reduced noise, Active mode 
	
} 
