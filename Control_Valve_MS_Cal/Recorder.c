#include "recorder.h"
#include "sysdata.h"
#include "24c08.h"
#include "uart.h"

#define RESTATEARR 0
#define REDATAARR 32

#define EEPNUM 1000

void IWDG_ReloadCounter(void);

u8 RecorderRBuf[16];
u8 RecorderWBuf[16];

u8 RecorderDays;
u16 RecorderNumber;

void RecorderInit(void)
{
  EEPPowerON();
  
  ReadEEP(RESTATEARR,RecorderRBuf,4);
  if(RecorderRBuf[0] != 0xa5)
  {
    RecorderRBuf[0] = 0xa5;
    RecorderRBuf[1] = 0;
    RecorderRBuf[2] = 0;
    RecorderRBuf[3] = 0;
    
    RecorderNumber = 0;
    
    WriteEEP(RESTATEARR,RecorderRBuf,4);
    
    ReadEEP(RESTATEARR,RecorderRBuf,4);
    if(RecorderRBuf[0] != 0xa5)
    {
      //while(1);
    }
  }
  else RecorderNumber = RecorderRBuf[2] << 8 | RecorderRBuf[3];
    
  //if(RecorderNumber > EEPNUM) RecorderNumber = 0;
  
  EEPPowerOFF();
}

void RecorderClear(void)
{
  EEPPowerON();
  u8 CleanBuf[16] = {0};
  u16 i;
  //RecorderNumber = 0;
  //RecorderWBuf[0] = RecorderNumber >> 8;
  //RecorderWBuf[1] = RecorderNumber &0xff;
  //WriteEEP(RESTATEARR + 2,RecorderWBuf,2);
  
  //eepromÇå³ýÈ«²¿Ð´0
  for(i = 0 ; i < EEPNUM ; i ++)
  {
    WriteEEP(REDATAARR + (i << 3),CleanBuf,8);   
    IWDG_ReloadCounter();               //Î¹¹·
  }
  //¸ÄÐ´µØÖ·
  RecorderNumber = 0;
  RecorderWBuf[0] = RecorderNumber >> 8;
  RecorderWBuf[1] = RecorderNumber &0xff;
  WriteEEP(RESTATEARR + 2,RecorderWBuf,2);
  EEPPowerOFF();
}

void RecorderSaveOne(void)
{
  u8 da;
  EEPPowerON();
  
  *(u32 *)RecorderWBuf = MS.MSV;
  da = 0;
  if(MS.Tamper) da |= 0x80;
  if(MS.Battery) da |= 0x40;
  if(MS.LBattery) da |= 0x20;
  if(Zenner.ValveStatus) da |= 0x10;
  if(Zenner.DLFlag1 || Zenner.DLFlag2) da |= 0x08;
  if(Zenner.XLFlag1 || Zenner.XLFlag2) da |= 0x04;
  if(Zenner.TimeFlag) da |= 0x02;
  if(Zenner.ValveErrorStatus == 2 || Zenner.ValveCheckErrorStatus == 2) da |= 0x01;
  RecorderWBuf[4] = da;
  
  da = 0;
  if(Zenner.ValveErrorStatus == 1) da |= 0x80;
  if(Zenner.QMFlag) da |= 0x40;
  da |= RecorderDays & 0x3f;
  RecorderWBuf[5] = da;
  
  RecorderWBuf[6] = data_pllrange1;
  RecorderWBuf[7] = data_pllrange;
  
  WriteEEP(REDATAARR + ((RecorderNumber%EEPNUM) << 3),RecorderWBuf,8);
  
  //if(RecorderNumber >= EEPNUM) RecorderNumber =0;
  //else
    RecorderNumber++;
  RecorderWBuf[0] = RecorderNumber >> 8;
  RecorderWBuf[1] = RecorderNumber &0xff;
  WriteEEP(RESTATEARR + 2,RecorderWBuf,2);
  
  EEPPowerOFF();
}

void RecorderRead(void)
{
  
}

void RecorderPrint(void)
{
  u16 i;
  
  EEPPowerON();
  
  if(RecorderNumber>EEPNUM)
  {
    for(i = RecorderNumber%EEPNUM ; i < EEPNUM ; i ++)
    {
      ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
      UartSendStr(RecorderRBuf,6);
      
      IWDG_ReloadCounter();               //Î¹¹·
    }
    for(i = 0 ; i < RecorderNumber%EEPNUM ; i ++)
    {
      ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
      UartSendStr(RecorderRBuf,6);
      
      IWDG_ReloadCounter();               //Î¹¹·
    }  
  }
  else
  {
    for(i = 0 ; i < RecorderNumber ; i ++)
      {
        ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
        UartSendStr(RecorderRBuf,6);
        
        IWDG_ReloadCounter();               //Î¹¹·
      }     
  }
  
  
  
  EEPPowerOFF();
}

void RecorderPrintMore(void)
{
  u16 i;
  
  EEPPowerON();
  if(RecorderNumber>EEPNUM)
  {
    for(i = RecorderNumber%EEPNUM ; i < EEPNUM ; i ++)
    {
      ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
      UartSendStr(RecorderRBuf,8);
      
      IWDG_ReloadCounter();               //Î¹¹·
    }
    for(i = 0 ; i < RecorderNumber%EEPNUM ; i ++)
    {
      ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
      UartSendStr(RecorderRBuf,8);
      
      IWDG_ReloadCounter();               //Î¹¹·
    }
  }
  else
  {
    for(i = 0 ; i < RecorderNumber ; i ++)
    {
      ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
      UartSendStr(RecorderRBuf,8);
      
      IWDG_ReloadCounter();               //Î¹¹·
    }
  }
  
  EEPPowerOFF();
}

/*
#include "recorder.h"
#include "sysdata.h"
#include "24c08.h"
#include "uart.h"

#define RESTATEARR 0
#define REDATAARR 32

void IWDG_ReloadCounter(void);

u8 RecorderRBuf[16];
u8 RecorderWBuf[16];

u8 RecorderDays;
u16 RecorderNumber;

void RecorderInit(void)
{
  EEPPowerON();
  
  ReadEEP(RESTATEARR,RecorderRBuf,4);
  if(RecorderRBuf[0] != 0xa5)
  {
    RecorderRBuf[0] = 0xa5;
    RecorderRBuf[1] = 0;
    RecorderRBuf[2] = 0;
    RecorderRBuf[3] = 0;
    
    RecorderNumber = 0;
    
    WriteEEP(RESTATEARR,RecorderRBuf,4);
    
    ReadEEP(RESTATEARR,RecorderRBuf,4);
    if(RecorderRBuf[0] != 0xa5)
    {
      while(1);
    }
  }
  else RecorderNumber = RecorderRBuf[2] << 8 | RecorderRBuf[3];
    
  if(RecorderNumber > 1000) RecorderNumber = 1000;
  
  EEPPowerOFF();
}

void RecorderClear(void)
{
  EEPPowerON();
  
  RecorderNumber = 0;
  RecorderWBuf[0] = RecorderNumber >> 8;
  RecorderWBuf[1] = RecorderNumber &0xff;
  WriteEEP(RESTATEARR + 2,RecorderWBuf,2);
  
  EEPPowerOFF();
}

void RecorderSaveOne(void)
{
  u8 da;
  EEPPowerON();
  
  *(u32 *)RecorderWBuf = MS.MSV;
  da = 0;
  if(MS.Tamper) da |= 0x80;
  if(MS.Battery) da |= 0x40;
  if(MS.LBattery) da |= 0x20;
  if(Zenner.ValveStatus) da |= 0x10;
  if(Zenner.DLFlag1 || Zenner.DLFlag2) da |= 0x08;
  if(Zenner.XLFlag1 || Zenner.XLFlag2) da |= 0x04;
  if(Zenner.TimeFlag) da |= 0x02;
  if(Zenner.ValveErrorStatus == 2 || Zenner.ValveCheckErrorStatus == 2) da |= 0x01;
  RecorderWBuf[4] = da;
  
  da = 0;
  if(Zenner.ValveErrorStatus == 1) da |= 0x80;
  if(Zenner.QMFlag) da |= 0x40;
  da |= RecorderDays & 0x3f;
  RecorderWBuf[5] = da;
  
  RecorderWBuf[6] = data_pllrange1;
  RecorderWBuf[7] = data_pllrange;
  
  WriteEEP(REDATAARR + (RecorderNumber << 3),RecorderWBuf,8);
  
  if(RecorderNumber < 1000) RecorderNumber ++;
  RecorderWBuf[0] = RecorderNumber >> 8;
  RecorderWBuf[1] = RecorderNumber &0xff;
  WriteEEP(RESTATEARR + 2,RecorderWBuf,2);
  
  EEPPowerOFF();
}

void RecorderRead(void)
{
  
}

void RecorderPrint(void)
{
  u16 i;
  
  EEPPowerON();
  
  for(i = 0 ; i < RecorderNumber ; i ++)
  {
    ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
    UartSendStr(RecorderRBuf,6);
    
    IWDG_ReloadCounter();               //Î¹¹·
  }
  
  EEPPowerOFF();
}

void RecorderPrintMore(void)
{
  u16 i;
  
  EEPPowerON();
  
  for(i = 0 ; i < RecorderNumber ; i ++)
  {
    ReadEEP(REDATAARR + (i << 3),RecorderRBuf,8);
    UartSendStr(RecorderRBuf,8);
    
    IWDG_ReloadCounter();               //Î¹¹·
  }
  
  EEPPowerOFF();
}*/




