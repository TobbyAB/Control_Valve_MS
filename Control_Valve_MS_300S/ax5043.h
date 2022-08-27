#ifndef _AX5043_H_
#define _AX5043_H_



void AX5043_Reset(void);

void AX5043_set_registers(void);
void AX5043_set_registers_0db(void);
unsigned char simple_autorange_pll(void);

/********************************************************************************************
* transmit packet                                                                           *
*********************************************************************************************/
void simple_transmit(unsigned char *pkt, unsigned int pktlen);

void send2min(void);

#endif






