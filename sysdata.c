
//////////////SYSDATA/////////////

#include "sysdata.h"

SYSDATA_STRUCT sysdata;
SYSFLAG_STRUCT sysflag;

//1000 = 5ms
void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount--);
}

void WriteRTCWakeUp(uint32_t Alarm)
{
  //RTC_AlarmCmd(DISABLE);
  RTC_WakeUpCmd(DISABLE);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);//32.768K/16=2.048k=0.488ms
  //RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//32.768K/16=2.048k=0.488ms
  RTC_ITConfig(RTC_IT_WUT, ENABLE);//开启中断
  //RTC_SetWakeUpCounter(120);//120S
  RTC_SetWakeUpCounter(Alarm);//2040/60=34Min
  RTC_ITConfig(RTC_IT_WUT, ENABLE);//唤醒定时器中断使能
  RTC_WakeUpCmd(ENABLE);//RTC唤醒使能
}

void WriteRTCAlarm(uint32_t Alarm)
{
  //RTC_WakeUpCmd(DISABLE);
  RTC_AlarmCmd(DISABLE);
  RTC_AlarmTypeDef RTC_AlarmType;
  RTC_AlarmStructInit(&RTC_AlarmType);
  RTC_AlarmType.RTC_AlarmTime.RTC_Hours = Alarm / 3600;
  RTC_AlarmType.RTC_AlarmTime.RTC_Minutes = Alarm / 60 % 60;
  RTC_AlarmType.RTC_AlarmTime.RTC_Seconds = Alarm % 60;
  RTC_AlarmType.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  RTC_AlarmType.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;// | RTC_AlarmMask_Hours| RTC_AlarmMask_Minutes;
  RTC_AlarmType.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmType.RTC_AlarmDateWeekDay = 2;
  RTC_SetAlarm(RTC_Format_BIN,&RTC_AlarmType);
  RTC_AlarmCmd(ENABLE);
}

void WriteRTCTime(uint32_t time)
{
  RTC_TimeTypeDef RTC_TimeType;
  RTC_TimeStructInit(&RTC_TimeType);
  RTC_TimeType.RTC_Hours = time / 3600;
  RTC_TimeType.RTC_Minutes = time / 60 % 60;
  RTC_TimeType.RTC_Seconds = time % 60;
  RTC_TimeType.RTC_H12 = RTC_H12_AM;
  RTC_SetTime(RTC_Format_BIN,&RTC_TimeType);
}

uint32_t ReadRTCTime(void)
{
  uint32_t time = 0;
  RTC_TimeTypeDef RTC_TimeRead;
  RTC_GetTime(RTC_Format_BIN,&RTC_TimeRead);
  
  time = RTC_TimeRead.RTC_Hours;
  time *= 3600;
  //time += RTC_TimeRead.RTC_Hours * 3600;
  time += RTC_TimeRead.RTC_Minutes * 60;
  time += RTC_TimeRead.RTC_Seconds;
  //time = ((RTC_TimeRead.RTC_Hours * 60 + RTC_TimeRead.RTC_Minutes) * 60) + RTC_TimeRead.RTC_Seconds;

  return time;
}

u8 write_eeprom(uint32_t address,uint8_t * data,u8 size)
{
    u8 check = 0;
    u8 i;
    address += 0x1000;
    if(IS_FLASH_DATA_EEPROM_ADDRESS(address))
    {
        FLASH_Unlock(FLASH_MemType_Data);
        for(i = 0; i < size ; i ++)
        {
                FLASH_ProgramByte(address + i,data[i]);
                check += data[i];
                while(!(FLASH->IAPSR & 0x04));
        }
        FLASH_ProgramByte(address + i,check);
        while(!(FLASH->IAPSR & 0x04));
        FLASH_Lock(FLASH_MemType_Data);
    }
    return 0;
}

u8 read_eeprom(uint32_t address,uint8_t * data,u8 size)
{
    u8 check = 0;
    u8 i;
    address += 0x1000;
    if(IS_FLASH_DATA_EEPROM_ADDRESS(address))
    {
        //FLASH_Unlock(FLASH_MemType_Data);
        for(i = 0; i < size ; i ++)
        {
            //data[i] = *(__IO uint8_t*)(address + i);
            data[i] = FLASH_ReadByte(address + i);
            check += data[i];
        }
        //FLASH_Lock(FLASH_MemType_Data);
        
        if(check == FLASH_ReadByte(address + i) && data[0] == 0xaa) return 0;
        else return 1;
    }
    return 1;
}

void PVDInit(void)                                        //PWR初始化
{ 
  PWR_PVDLevelConfig(PWR_PVDLevel_2V26);                  //PVD设置2.26V
  PWR_PVDCmd(ENABLE);                                     //PVD使能
  //PWR_PVDClearITPendingBit();
  //PWR_PVDITConfig(ENABLE);                                //PVD中断使能
}

void PVDDeInit(void)                                        //~PWR初始化
{ 
  PWR_PVDLevelConfig(PWR_PVDLevel_2V26);                  //PVD设置2.26V
  PWR_PVDCmd(DISABLE);                                     //PVD使能
  //PWR_PVDITConfig(DISABLE);                                //PVD中断使能
  //PWR_PVDClearITPendingBit();
}

//////////////////////////////User/////////////////////////////////

u8 SysdataRead(void);

void SysdataInit(void)
{
  unsigned char i = 0;
  
  if(SysdataRead())     //读数据错误
  {
    //初始化数据
    sysdata.State = 0xaa;
    sysdata.Versions = 0;
    sysdata.Tamper = 0;
    sysdata.Battery = 0;
    sysdata.MSID = 0x1E240;
    sysdata.MSV = 0;
    
    sysdata.DL_Times = 3;
    sysdata.DL_Sec = 18;
    sysdata.XL_Times = 72;
    sysdata.T_Month = 60;
    sysdata.T_XyMonth = 6;
    sysdata.T_Day = 0;
    sysdata.T_Hour = 0;
    
    sysdata.DL_Sec = 10;
    sysdata.DL_Times = 3;
    sysdata.DL_Times2 = 3;
    //sysdata.DL_Pulse = 1;
    //sysdata.DL_Pulse2 = 1;
    
    sysdata.XL_Enable = 1;
    sysdata.XL_Times = 24;
    sysdata.XL_Times2 = 24;
    sysdata.XL_Sec = 3600;
    sysdata.XL_Sec2 = 10800;
    
    sysdata.TSY_Sec = 155520000;
    sysdata.TBJ_Sec = 15552000;
    
    sysdata.Meter_Flag = 0;
    
    write_eeprom(EEPROM_ADDR_BANK1,&(sysdata.State),EEPROM_DATA_LEN);
    write_eeprom(EEPROM_ADDR_BANK2,&(sysdata.State),EEPROM_DATA_LEN);
    SysdataSave();              //检查再写一遍
  }
  
  for(i = 0 ; i < 10 ; i ++) sysdata.cQuanTimeBuf[i] = 255;
  
  /*
  read_eeprom(EEPROM_ADDR,&(sysdata.State),16);
  if(sysdata.State != 0xaa)
  {
    sysdata.State = 0xaa;
    sysdata.Versions = 2;
    sysdata.Tamper = 0;
    sysdata.Battery = 0;
    sysdata.MSID = 0x1E240;
    sysdata.MSV = 0;
    write_eeprom(EEPROM_ADDR,&(sysdata.State),16);
    
    if(read_eeprom(EEPROM_ADDR,&(sysdata.State),16)) while(1);
    //read_eeprom(EEPROM_ADDR,&(sysdata.State),16);
    //if(sysdata.State != 0xaa) while(1);
  }
  */
  //WriteRTCAlarm(0);
}

u8 WriteDataWithCheckBackup(u16 address1,u16 address2,uint8_t * data,u16 size)
{
  u8 buf1[64];
  u8 buf2[64];
  u8 res1;
  u8 res2;
  u8 i;
  u8 bank2 = 0;
  
//  address1 += 0x1000;
//  address2 += 0x1000;
  
  res1 = read_eeprom(address1,buf1,size);
  res2 = read_eeprom(address2,buf2,size);
  if(res1 == 0)         //数据正确
  {
    if(res2 == 0)       //备份数据正确
    {
      for(i = 0 ; i < size ; i ++)        //比对同步
      {
        if(buf1[i] != buf2[i]) break;
      }
    }
    else i = 0;
    
    if(i != size)         //数据不同步，直接给备份写最新的数据
    {
      write_eeprom(address2,data,size);
      if(read_eeprom(address2,buf2,size)) return 1;          //备份写错误，等待看门狗复位
      
      bank2 = 1;        //备份数据最新
    }
    
  }
  else if(res2) return 1;        //备份都错误，等待初始数据
  
  //备份正确

  //可以写数据
  write_eeprom(address1,data,size);
  if(read_eeprom(address1,buf1,size)) return 1;          //数据写错误，等待看门狗复位
  
  //反正下次写的时候备份不同步，会写最新的备份
  if(bank2 == 0)          //数据正确，备份不是最新，准备写备份
  {
//    write_eeprom(EEPROM_ADDR_BANK2,&(sysdata.State),16);
//    if(read_eeprom(EEPROM_ADDR_BANK2,buf2,16)) while(1);          //备份写错误，等待看门狗复位
  }
  
  return 0;
}
u8 ReadDataWithCheckBackup(u16 address1,u16 address2,uint8_t * data,u16 size)
{
//  address1 += 0x1000;
//  address2 += 0x1000;
  
  if(read_eeprom(address1,data,size))              //数据错误
  {
    if(read_eeprom(address1,data,size)) return 1;               //备份错误
  }
  return 0;
}
//void WriteDataWithCheckBackup(u16 address1,u16 address2,uint8_t * data,u16 size)
//{
//  CheckBackup(address1,address2,size);
//}

u8 SysdataRead(void)
{
  if(read_eeprom(EEPROM_ADDR_BANK1,&(sysdata.State),EEPROM_DATA_LEN))              //数据错误
  {
    if(read_eeprom(EEPROM_ADDR_BANK2,&(sysdata.State),EEPROM_DATA_LEN)) return 1;              //备份错误
  }
  return 0;
}

void SysdataSave(void)
{
  u8 buf1[EEPROM_DATA_LEN+1];
  u8 buf2[EEPROM_DATA_LEN+1];
  u8 res1;
  u8 res2;
  u8 i;
  u8 bank2 = 0;
  
  res1 = read_eeprom(EEPROM_ADDR_BANK1,buf1,EEPROM_DATA_LEN);
  res2 = read_eeprom(EEPROM_ADDR_BANK2,buf2,EEPROM_DATA_LEN);
  if(res1 == 0)         //数据正确
  {
    if(res2 == 0)       //备份数据正确
    {
      for(i = 0 ; i < EEPROM_DATA_LEN ; i ++)        //比对同步
      {
        if(buf1[i] != buf2[i]) break;
      }
    }
    else i = 0;
    
    if(i != EEPROM_DATA_LEN)         //数据不同步，直接给备份写最新的数据
    {
      write_eeprom(EEPROM_ADDR_BANK2,&(sysdata.State),EEPROM_DATA_LEN);
      if(read_eeprom(EEPROM_ADDR_BANK2,buf2,EEPROM_DATA_LEN)) while(1);          //备份写错误，等待看门狗复位
      
      bank2 = 1;        //备份数据最新
    }
    
  }
  else if(res2) while(1);        //备份都错误，等待初始数据
  
  //备份正确

  //可以写数据
  write_eeprom(EEPROM_ADDR_BANK1,&(sysdata.State),EEPROM_DATA_LEN);
  if(read_eeprom(EEPROM_ADDR_BANK1,buf1,EEPROM_DATA_LEN)) while(1);          //数据写错误，等待看门狗复位
  
  //反正下次写的时候备份不同步，会写最新的备份
  if(bank2 == 0)          //数据正确，备份不是最新，准备写备份
  {
//    write_eeprom(EEPROM_ADDR_BANK2,&(sysdata.State),16);
//    if(read_eeprom(EEPROM_ADDR_BANK2,buf2,16)) while(1);          //备份写错误，等待看门狗复位
  }
  
}

void SysdataWriteVersions(uint32_t data)
{
  sysdata.Versions = data;
  SysdataSave();
}
/*
void SysdataWriteTamper(uint32_t data)
{
  sysdata.Tamper = data;
  SysdataSave();
}
void SysdataWriteBattery(uint32_t data)
{
  sysdata.Battery = data;
  SysdataSave();
}
*/
void SysdataWriteMSID(uint32_t id)
{
  sysdata.MSID = id;
  SysdataSave();
}
void SysdataWriteMSV(uint32_t data)
{
  sysdata.MSV = data;
  SysdataSave();
}
void SysdataUpMSV(void)
{
  if(sysdata.VTTime != 0) sysdata.VTFlag = 1;
  else sysdata.VTTime = 3;
  //int32_t data;
  //data = ReadRTCTime() - sysdata.S1TIME;
  //if(!(data > -5 && data < 5))
  //{
  //  sysdata.S1TIME = ReadRTCTime();
  //  sysdata.MSV ++;
  //  if(sysdata.MSV == 1000000) sysdata.MSV = 0;
  //  //SysdataSave();
  //}
}



void SysdataReadBattery1(void)
{
  PWR_PVDCmd(DISABLE); 
  sysdata.PVDBatteryFlag = 0;
  sysdata.PVDLBatteryFlag1 = 0;
  PWR_PVDLevelConfig(PWR_PVDLevel_2V45);                  //pvd修改为2.45V 
  PWR_PVDCmd(ENABLE);                                     //PVD使能
  PWR_PVDClearITPendingBit();
  PWR_PVDITConfig(ENABLE);                                //PVD中断使能
}
void SysdataReadBattery2(void)   //阀门低电压
{
  PWR_PVDCmd(DISABLE); 
  sysdata.PVDLBatteryFlag2 = 0;
  PWR_PVDLevelConfig(PWR_PVDLevel_2V05);                  // pvd修改为2.05V
  PWR_PVDCmd(ENABLE);                                     //PVD使能
  PWR_PVDClearITPendingBit();
  PWR_PVDITConfig(ENABLE);                                //PVD中断使能
}

void SysdataReadBatteryEnd(void)
{
/*****************关电量检测功能******************/
  PWR_PVDCmd(DISABLE);                                     //PVD使能
  PWR_PVDITConfig(DISABLE);                                //PVD中断使能
  PWR_PVDClearITPendingBit();
/*************************************************/
}

void SysdataUpdateTamper(void)
{
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0) MS.Tamper = 1;
  else MS.Tamper = 0;
}






