#ifndef _24C08_H_
#define _24C08_H_

#include "stm8l15x.h"

void EEPPowerON(void);
void EEPPowerOFF(void);

uint32_t ReadEEP(uint16_t addr,u8 *p,uint32_t len);
uint32_t WriteEEP(uint16_t addr,u8 *p,uint32_t len);

#endif


