#ifndef buttons_h
#define buttons_h

#include "MKL46Z4.h"                    // Device header
#include "Led.h"

void buttonsInitialize(void);
void buttonsService(void);							//buttons' interrupt service procedure
int32_t buttonRead(void);


#endif

