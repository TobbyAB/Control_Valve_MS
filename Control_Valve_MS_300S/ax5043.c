
#include "spi.h"
#include "uart.h"
#include "ax5043def.h"
#include "stdio.h"
#include "sysdata.h"

/***********************************************************************
* Funcation £º
* Description£º
* InPut    £º
* OutPut   £º
* Return   £º
* Notice   £º
***********************************************************************/
void AX5043_Reset(void)
{
    unsigned char ubAddres;

    CSN(1);
    CSN(0);
    while (SDI == 0);
    //write comm  bit7=1
    SpiWriteSingleAddressRegister(REG_AX5043_PWRMODE, 0x80);
    CSN(1);
    
    IWDG_ReloadCounter();
    
    SpiWriteSingleAddressRegister(REG_AX5043_PWRMODE, 0x00);        //RADIO_PWRMODE = PWRMODE_PWRDOWN=0x00

    SpiWriteSingleAddressRegister(REG_AX5043_SCRATCH, 0x55);
    do
    {
        //delay(1);
        asm("NOP");
        asm("NOP");
        ubAddres = SpiReadSingleAddressRegister(REG_AX5043_SCRATCH);
    }
    while (ubAddres != 0x55);

    SpiWriteSingleAddressRegister(REG_AX5043_PINFUNCIRQ, 0x01);  // IRQ Line 1   001 IRQ Output ¡¯1¡¯
    asm("NOP");
    asm("NOP");
    SpiWriteSingleAddressRegister(REG_AX5043_PINFUNCIRQ, 0x00);  //IRQ Line 0  000 IRQ Output ¡¯0¡¯
    SpiWriteSingleAddressRegister(REG_AX5043_PINFUNCIRQ, 0x03);  //011 IRQ Output Interrupt Request
}


void simple_wait_for_xtal(void)
{
  uint8_t data;
    SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 ,SpiReadAutoAddressRegister(REG_AX5043_IRQMASK1) | 0x01); // enable xtal ready interrupt
    for(;;)
    {
      data = SpiReadAutoAddressRegister(REG_AX5043_XTALSTATUS);
        if (data)
            break;
    }
    SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0);
}

//u8 BCDtoHEX(u8 nubmer)
//{
//  u8 number_ask;
//  switch(nubmer)
//  {
//   case 0:
//    number_ask=0x30;
//   break;
//   case 1:
//    number_ask=0x31;
//   break; 
//   case 2:
//    number_ask=0x32;
//   break;
//   case 3:
//    number_ask=0x33;
//   break;
//   case 4:
//    number_ask=0x34;
//   break;
//   case 5:
//    number_ask=0x35;
//   break;
//   case 6:
//    number_ask=0x36;
//   break;
//   case 7:
//    number_ask=0x37;
//   break;
//   case 8:
//    number_ask=0x38;
//   break;
//   case 9:
//    number_ask=0x39;
//   break;
//   case 10:
//    number_ask=0x41;
//   break;    
//   case 11:
//    number_ask=0x42; 
//       break;
//   case 12:
//    number_ask=0x43;
//       break;
//   case 13:
//    number_ask=0x44;
//       break;
//   case 14:
//    number_ask=0x45;
//       break;
//   case 15:
//    number_ask=0x46;    
//   break;   
//  }
//  return number_ask;
//}
  

volatile uint8_t data_pllrange2;
volatile uint8_t data_pllrange;
volatile uint8_t data_pllrange1;

unsigned char simple_autorange_pll(void)
{

      
      uint8_t  pllrng ;
      uint8_t  VcoiF,VcoiF_max = 0,RangeMin = 0;
      uint8_t  VcoiF_num = 0;
      uint8_t VcoiF_table[30],Range_table[30],VcoiF_table_chose[30];


      VcoiF_table_chose[0]=0;
      VcoiF_table[0] = 0;
      Range_table[0] = 0;

    
      SpiReadAutoAddressRegister(REG_AX5043_PLLLOOP);
      SpiReadAutoAddressRegister(REG_AX5043_PLLCPI);
      SpiWriteAutoAddressRegister(REG_AX5043_PLLLOOP , 0x09); // default 100kHz loop BW for ranging
      SpiWriteAutoAddressRegister(REG_AX5043_PLLCPI , 0x08);

    //IE_4 = 1; // enable radio interrupt
      SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE , AX5043_PWRSTATE_XTAL_ON); // start crystal
      simple_wait_for_xtal();
      
      IWDG_ReloadCounter();
    
    //ÔÚÏÂÃæ¼ìË÷0x80-0xbf
    pllrng = 0x20;
    
    VcoiF = 0xa2 ;
    while(VcoiF < 0xbf)
    {
	  SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0x00);

	  SpiWriteAutoAddressRegister(REG_AX5043_PLLVCOI,VcoiF);                //vcoi
      
          SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0x10); // enable pll autoranging done interrupt
  
	  SpiWriteAutoAddressRegister(REG_AX5043_PLLRANGINGA , 0x18); // init ranging process starting from range 8
          for (;;)
            {
            if (SpiReadAutoAddressRegister(REG_AX5043_IRQREQUEST1) & 0x10)
                  break;
            }
      
          SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0x00);
          pllrng = SpiReadAutoAddressRegister(REG_AX5043_PLLRANGINGA);            
           if(pllrng & 0x20)
            {
                VcoiF++;
                continue;
            }   
          pllrng = pllrng & 0x0f; 
          VcoiF_table[VcoiF_num] = VcoiF;
          Range_table[VcoiF_num] = pllrng;      
          VcoiF_num ++;
          VcoiF++;
    } //while end

    /*±éÀúÑ¡³ö×îĞ¡µÄRangÖµ*/
    RangeMin = Range_table[0];

    uint8_t i = 0;
    while(i < VcoiF_num)
    {
        if (Range_table[i] < RangeMin)
        {
            RangeMin = Range_table[i];
        }
        i+=1;
    }
  
    /*Ñ¡³ö×îĞ¡µÄRangÖµÖĞËùÓĞµÄVCOIµÄÖµ */
    uint8_t j = 0,VcoiFmaxNum = 0;
    while(j < VcoiF_num)
    {
        if (Range_table[j] == RangeMin)
        {
            VcoiF_table_chose[VcoiFmaxNum] = VcoiF_table[j];
            VcoiFmaxNum++;
        }
        j++;
    }

    /*Ñ¡³ö×îĞ¡µÄRangÖµÖĞËùÓĞµÄVCOIµÄÖµÖĞÈ¡×î´óµÄVCOI */
    VcoiF_max = VcoiF_table_chose[0];
    uint8_t k = 0;
    while(k < VcoiFmaxNum)
    {
        if (VcoiF_table_chose[k] > VcoiF_max)
        {
            VcoiF_max = VcoiF_table_chose[k];
        }
        k+=1;
    }
       
    /*Ñ¡³ö×îĞ¡µÄRangÖµÖĞËùÓĞµÄVCOIµÄÖµÖĞÈ¡×î´óµÄVCOIĞ´Èë */
   
    if (RangeMin == 0)
      SpiWriteAutoAddressRegister(REG_AX5043_PLLVCOI,0xbe);  
    else
      SpiWriteAutoAddressRegister(REG_AX5043_PLLVCOI,VcoiF_max); 
    
    SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0x10); // enable pll autoranging done interrupt
    SpiWriteAutoAddressRegister(REG_AX5043_PLLRANGINGA , 0x18); // init ranging process starting from range 8
    for (;;)
    {
        if (SpiReadAutoAddressRegister(REG_AX5043_IRQREQUEST1) & 0x10)
            break;
    }
    SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK1 , 0x00);
    pllrng = SpiReadAutoAddressRegister(REG_AX5043_PLLRANGINGA);


/*BACK START*/    
/*    IE |= iesave;

    SpiWriteAutoAddressRegister(REG_AX5043_PLLLOOP , pllloop_save); // restore loop settings
    SpiWriteAutoAddressRegister(REG_AX5043_PLLCPI , pllcpi_save);
    
    data_pllrange = pllrng;

    if( pllrng & 0x20 ) // ranging error, power down AX5043
    {
        SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE , AX5043_PWRSTATE_POWERDOWN);
        return 0xff;
    }
*/    
/*BACK OVER*/         
    
    SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE , 0x0c);
    
    data_pllrange = pllrng;
    data_pllrange1 = VcoiF_max;

    return 0; // ranging ok, keep crystal running
}


#define SYNCWORD 0xAACCAACC
#define PREAMBLE_LEN_BYTES 4
#define PREAMBLE_CHAR 0x55
#define PREAMBLE_FLAGS 0x38

#define SYNCWORD_FLAGS 0x18
/********************************************************************************************
* transmit packet                                                                           *
*********************************************************************************************/
void 
simple_transmit(uint8_t *pkt, uint16_t pktlen)
{
// assumes that the chip is initialized and the PLL has been autoranged
// *pkt points to the packet without sync word, but including len byte (if desired)
// CRC (computed over the entire *pkt) is added by the hardware automatically if configured in the register settings

  //uint8_t syncword[4] = {(SYNCWORD)>>0 & 0xFF, (SYNCWORD)>>8 & 0xFF, (SYNCWORD)>>16 & 0xFF, (SYNCWORD)>>24 & 0xFF};

    SpiWriteAutoAddressRegister(REG_AX5043_FIFOSTAT , 3); // clear FIFO
    SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE , AX5043_PWRSTATE_FULL_TX); // FULL_TX mode (note: actual packet transmission only starts after commiting the FIFO)

    while (!(SpiReadAutoAddressRegister(REG_AX5043_POWSTAT) & 0x08)); // wait until VDDMODEM ready so writing the FIFO is safe
/*
    
    // write preamble to the FIFO using a REPEATDATA chunk
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , AX5043_FIFOCMD_REPEATDATA | (3 << 5));
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , PREAMBLE_FLAGS);
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , PREAMBLE_LEN_BYTES);
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , PREAMBLE_CHAR);


    //write SYNC WORD to FIFO
    if ((SpiReadAutoAddressRegister(REG_AX5043_FRAMING) & 0x0E) == 0x06) {// write SYNC word if framing mode is raw_patternmatch
        uint8_t i;
        SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , AX5043_FIFOCMD_DATA | ((sizeof(syncword) + 1) << 5));
        SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , SYNCWORD_FLAGS);
        for (i = 0; i < sizeof(syncword); ++i) {
            // better put a brace, it might prevent SDCC from optimizing away the assignement...
            SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , syncword[i]);
        }
    }


    // write packet to FIFO using a DATA chunk
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , AX5043_FIFOCMD_DATA | (7 << 5));
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , pktlen + 1); // write FIFO chunk length byte (length includes the flag byte, thus the +1)
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , 0x03); // pkt_start | pkt_end
    
    */
    
    IWDG_ReloadCounter();
    
    // write preamble to the FIFO using a REPEATDATA chunk
    for(uint16_t len = 0 ; len < pktlen ; len ++) SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , pkt[len]);

    SpiReadAutoAddressRegister(REG_AX5043_RADIOEVENTREQ0); // clear flag
    simple_wait_for_xtal(); // wait until cystal oscillator ready
    SpiWriteAutoAddressRegister(REG_AX5043_FIFOSTAT , 4); // commit the FIFO --> TX starts


    // wait until TX is finished
    {
        SpiWriteAutoAddressRegister(REG_AX5043_RADIOEVENTMASK0 , 0x01); // TX done
        SpiWriteAutoAddressRegister(REG_AX5043_IRQMASK0 , 0x40); // radio controller irq
        for(;;)
        {
            if(SpiReadAutoAddressRegister(REG_AX5043_RADIOEVENTREQ0) & 0x01)
                break;
        }
    }
//    SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE , AX5043_PWRSTATE_POWERDOWN);

    //uint8_t data;
    //data = data;
}

void send2min(void)
{
/*  AX5043_FSKDEV2 = 0x00;                              //FSK
  AX5043_FSKDEV1 = 0x00;                             //Deviation =0
  AX5043_FSKDEV0 = 0x00;                             //Deviation =0
  AX5043_TXRATE2 = 0x00;                             //use the smallest data rate
  AX5043_TXRATE1 = 0x00;                             //use the smallest data rate
  AX5043_TXRATE0 = 0x01;                             //use the smallest data rate
  */
  SpiWriteAutoAddressRegister(AX5043_FSKDEV2,0x00);
  SpiWriteAutoAddressRegister(AX5043_FSKDEV1,0x00);
  SpiWriteAutoAddressRegister(AX5043_FSKDEV0,0x00);
  SpiWriteAutoAddressRegister(AX5043_TXRATE2,0x00);
  SpiWriteAutoAddressRegister(AX5043_TXRATE1,0x00);
  SpiWriteAutoAddressRegister(AX5043_TXRATE0,0x01);

  SpiWriteAutoAddressRegister(REG_AX5043_FIFOSTAT,3);
  SpiWriteAutoAddressRegister(REG_AX5043_PWRMODE,AX5043_PWRSTATE_FULL_TX);
  while (!(SpiReadAutoAddressRegister(REG_AX5043_POWSTAT) & 0x08));
/*
  // 1. clear FIF0
  Spi_Write_Radio(AX5043_FIFOSTAT, 3);
  // 2. Change to FULL_TX mode
   Spi_Write_Radio(AX5043_PWRMODE, AX5043_PWRSTATE_FULL_TX);
  // 3. wait untill VDDMODEM ready
   while (!(Spi_Read_Radio(AX5043_POWSTAT) & 0x08)); // wait until VDDMODEM ready so writing the FIFO is safe
 
  //AX5043_ENCODING |=4;  (no need)*/
  
  //SpiWriteAutoAddressRegister(REG_AX5043_ENCODING,0x06);
  
  
  /*Á¬Ğø·¢Éä*/
  for(uint16_t i = 0 ; i <3 ; i ++)//i=3Ê±£¬Ô¼Îª7S  
  {
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , 0X62);//0x61
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , 3);//150
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , 0Xff);
    SpiWriteAutoAddressRegister(REG_AX5043_FIFODATA , 0xaa);
    IWDG->KR = IWDG_KEY_REFRESH;
  }
  simple_wait_for_xtal();
  SpiWriteAutoAddressRegister(REG_AX5043_FIFOSTAT, 4);
  
  while(( SpiReadAutoAddressRegister(REG_AX5043_FIFOFREE1)<<8 + SpiReadAutoAddressRegister(REG_AX5043_FIFOFREE0)) < 255)
  {
    IWDG->KR = IWDG_KEY_REFRESH;
    Delay(6000);
  }

}




