
#include "MKL46Z4.h"

/*========================================================*/
/* Macros for Accelerometer Device*/
/* =======================================================*/
#define ACC_DEVICE_ADDRESS                 0x1D
#define ACC_DEVICE_ID_REGISTER_ADDRESS     0x0D
#define XYZ_CFG 						   							0x0E
#define ACC_CTRL_REG1                      0x2A
#define ACC_OUT_X_MSB                      0x01
#define SYSMODE_REG                        0x0B




uint8_t Status_Register(void);
uint8_t CheckCalibration(void);
void Accelerometer_data_transmition(void);
void ACC_Calibration (void);
void ACC_Init(void);


	