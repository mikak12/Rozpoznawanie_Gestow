#ifndef I2C_h
#define I2C_h

#include "MKL46Z4.h" 
#include "UART.h"
#include "Clock.h"


#define RIGHT_SHIFT(x,y)                   (x >> y)
#define LEFT_SHIFT(x,y)                    (x << y)
#define I2C0_SCL                           (24)
#define I2C0_SDA                           (25)
#define READ(x)                            ((x<<1)|(0x01))
#define WRITE(x)                           ((x<<1)&(0xFE))

extern unsigned char DATA_READ[6];

void I2C_Init(void);
unsigned char I2C_AddressRead(unsigned char DEV_ADR/*, unsigned char REG_ADR*/);
void I2C_Scanner(void);
unsigned char I2C_SingleByteRead(unsigned char DEV_ADR, unsigned char REG_ADR);
void I2C_MultipleByteWrite(unsigned char DEV_ADR, unsigned char REG_ADR, int max_count, unsigned char data_wr[]);
void I2C_SingleByteWrite(unsigned char DEV_ADR, unsigned char REG_ADR, unsigned char DATA);
void I2C_MultipleByteRead(unsigned char DEV_ADR,unsigned char REG_ADR, int max_count);
#endif
