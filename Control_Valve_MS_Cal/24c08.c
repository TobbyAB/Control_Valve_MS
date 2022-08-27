#include "24c08.h"
#include "stm8l15x.h"

void delay_us(u8 nCount)
{
  while (nCount--);
}
void delay_ms(u8 nCount)
{
  nCount *= 4;
  while (nCount--)
  {
    delay_us(250);
    delay_us(250);
    delay_us(250);
    delay_us(250);
  }
}

void EEPPowerON(void)
{
  GPIOC->ODR |= GPIO_Pin_2;
  delay_ms(1);
}
void EEPPowerOFF(void)
{
  delay_ms(1);
  GPIOC->ODR &= ~GPIO_Pin_2;
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_High_Slow);
  GPIOC->ODR &= ~GPIO_Pin_1;
}

void IICInit(void)
{
  
}

void IIC_IO_OUT()
{
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_High_Slow);
}
void IIC_IO_IN()
{
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
  GPIOC->ODR |= GPIO_Pin_0;
}
u8 IIC_IORead(void)
{
  return (GPIOC->IDR & 0x01);
}

////////////////////////////////////////////////

u8 IICReadAsk()
{	
	u8 ask;
        
	IIC_IO_IN();
	
	delay_us(1);
	if(IIC_IORead()) ask=1;
	else ask=0;
	GPIOC->ODR |= GPIO_Pin_1;
	delay_us(1);
	GPIOC->ODR &= ~GPIO_Pin_1;
	delay_us(1);
        
        IIC_IO_OUT();
        
	return(ask);
}

void IICWriteAsk(u8 ask)
{	
	if(ask) GPIOC->ODR |= GPIO_Pin_0;
	else GPIOC->ODR &= ~GPIO_Pin_0;
	delay_us(1);
	GPIOC->ODR |= GPIO_Pin_1;
	delay_us(1);
	GPIOC->ODR &= ~GPIO_Pin_1;
	delay_us(1);
}

void IICStart()
{
	GPIOC->ODR |= GPIO_Pin_1;
	GPIOC->ODR |= GPIO_Pin_0;
	delay_us(1);
	GPIOC->ODR &= ~GPIO_Pin_0;
	delay_us(1);
	GPIOC->ODR &= ~GPIO_Pin_1;
}

void IICStop()
{
	GPIOC->ODR &= ~GPIO_Pin_0;
	GPIOC->ODR |= GPIO_Pin_1;
	delay_us(1);
	GPIOC->ODR |= GPIO_Pin_0;
	delay_ms(5);
}

u8 IICWriteByte(uint8_t da)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(da&0x80) GPIOC->ODR |= GPIO_Pin_0;
		else GPIOC->ODR &= ~GPIO_Pin_0;
		da<<=1;
		delay_us(1);
		GPIOC->ODR |= GPIO_Pin_1;
		delay_us(1);
		GPIOC->ODR &= ~GPIO_Pin_1;
		delay_us(1);
	}
	if(IICReadAsk())
	{
		IICStop();
		return(1);
	}
	else return(0);
}

uint8_t IICReadBbyte(u8 ask)
{
	u8 i;
	uint8_t da;
        
	IIC_IO_IN();
	
	for(i=0;i<8;i++)
	{
		delay_us(1);
		
		da<<=1;
		if(IIC_IORead()) da++;
		
		GPIOC->ODR |= GPIO_Pin_1;
		delay_us(1);
		GPIOC->ODR &= ~GPIO_Pin_1;
		
		delay_us(1);
	}
        
	IIC_IO_OUT();
        
	IICWriteAsk(ask);
	return(da);
}

#define DEFINE_24C64 1
#define _24C0X1_ID 0XA0

#if DEFINE_24C02

	#define	_24CXX_ADDR_LEN 1
	#define _24CXX_PAGE_BYTE	8
	
#elif DEFINE_24C04

	#define	_24CXX_ADDR_LEN 1
	#define _24CXX_PAGE_BYTE	8
	
#elif DEFINE_24C08

	#define	_24CXX_ADDR_LEN 1
	#define _24CXX_PAGE_BYTE	8
	
#elif DEFINE_24C32

	#define	_24CXX_ADDR_LEN 2
	#define _24CXX_PAGE_BYTE	8
	
#elif DEFINE_24C64

	#define	_24CXX_ADDR_LEN 2
	#define _24CXX_PAGE_BYTE	8
	
#else

	#define	_24CXX_ADDR_LEN 1
	#define _24CXX_PAGE_BYTE	8
	
#endif

void _24cxx_init(uint16_t clk,uint32_t clk_pin,uint16_t dat,uint32_t dat_pin)
{
	
}

uint32_t Read24cxx(uint8_t id,uint32_t addr,u8 *p,uint32_t len)
{
	uint32_t i;
	IICStart();
	if(IICWriteByte(id)) return(1);
#if _24CXX_ADDR_LEN > 1
		if(IICWriteByte(addr>>8)) return(2);
#endif
		if(IICWriteByte(addr&0xff)) return(2);
	IICStart();
	if(IICWriteByte(id+1)) return(3);
	for(i=0;i<len-1;i++) p[i]=IICReadBbyte(0);
	p[i]=IICReadBbyte(1);
	IICStop();
	return(0);
}

uint32_t Write24cxx(uint8_t id,uint32_t addr,u8 *p,uint32_t len)
{
	uint32_t i;
	for(uint32_t j = 0 ; j < len ; )
	{
		IICStart();
		if(IICWriteByte(id)) return(1);
#if _24CXX_ADDR_LEN > 1
		if(IICWriteByte((addr + j)>>8)) return(2);
#endif
		if(IICWriteByte((addr + j)&0xff)) return(2);
		for(i = 0 ; i < _24CXX_PAGE_BYTE && j < len ; i ++) if(IICWriteByte(p[j ++])) break;
		IICStop();
		if(i != _24CXX_PAGE_BYTE) return(3);
	}
	return(0);
}


uint32_t ReadEEP(uint16_t addr,u8 *p,uint32_t len)
{
	return Read24cxx(_24C0X1_ID,addr,p,len);
}

uint32_t WriteEEP(uint16_t addr,u8 *p,uint32_t len)
{
	return Write24cxx(_24C0X1_ID,addr,p,len);
}




