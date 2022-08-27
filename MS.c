
#include "spi.h"

#include "sysdata.h"

#include "ax5043.h"

sMS MS;


#define MS_DATA_ADDR1 0X00
#define MS_DATA_ADDR2 0X40
#define MS_DATA_SIZE 14

void MSDataInit(void)
{
  MS.SaveFlag = 0xaa;          //存储数据有效位-起始位
  
  MS.Versions = 0;          //MS版本号
  
  MS.MSID = 87654321;             //MSID
  MS.MSV = 0;              //MS值
  MS.MST = 666666;              //MS时间
}
void MSDataSave(void)
{
  WriteDataWithCheckBackup(MS_DATA_ADDR1,MS_DATA_ADDR2,&(MS.SaveFlag),MS_DATA_SIZE);
}
void MSDataRead(void)//
{
  if(ReadDataWithCheckBackup(MS_DATA_ADDR1,MS_DATA_ADDR2,&(MS.SaveFlag),MS_DATA_SIZE))
  {
    sysdata.SyadataError = 1;
    
    MSDataInit();
    write_eeprom(MS_DATA_ADDR1,&(MS.SaveFlag),MS_DATA_SIZE);
    write_eeprom(MS_DATA_ADDR2,&(MS.SaveFlag),MS_DATA_SIZE);
    MSDataSave();
  }
}

void MSPulseLogic(void)
{
  MS.MSV ++;
  if(MS.MSV == 1000000) MS.MSV = 0;
}


#define DATA_START 10

#define THSTART         0

#define THVERSIONS_LEN  2

#define THTAMPER_LEN    1

#define THBATTERY_LEN   1

#define THMSID_LEN      20

#define THMSV_LEN       20


uint8_t send_buf[32]=
{

  0xe1,0x1A,0x03,0X55,0X55,0X55,0X55,0X55,0X55,0X5A,
  //0x6a,0xa9,0xa9,0x95,0x66,0x69,
  //0xa6,0xa9,0xaa,0xaa,0xaa,0xaa,
  //0xa9,0xa9,0xaa,0x55,0xaa,0x55,
};
/*uint8_t send_buf[]=
{
  0X55,0X55,0X55,0X55,0X55,0X5A,
  0x99,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
  0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
};*/
uint8_t send_01[256]=
{
  0xe1,201,0x03,
};

unsigned char data_buf[9] = 
{
  0,
};

void UpDataToSendBuf(void)
{
  unsigned char addr = DATA_START;
  unsigned char addr_i = 0;
  for(unsigned char n = 0 ; n < 9 ; n ++)
  {
    unsigned char data = data_buf[n];
    for(unsigned char i = 0 ; i < 8 ; i ++)
    {
      send_buf[addr] <<= 2;
      if(data & 0x80) send_buf[addr] |= 1;
      else send_buf[addr] |= 2;
      
      data <<= 1;
      
      addr_i ++;
      if(addr_i == 4)
      {
        addr_i = 0;
        addr ++;
      }
    }
  }
}

void DataCheck(void)
{
  for(unsigned char n = 0 ; n < 7 ; n ++)
  {
    unsigned char buf_data = data_buf[n];
    unsigned char data = 0;
    for(unsigned char i = 0 ; i < 7 ; i ++)
    {
      if(buf_data & 0x80) data ++;
      buf_data <<= 1;
    }
    if((data % 2) != 0) data_buf[n] |= 0x01;
  }
  
  data_buf[7] = 0;
  unsigned char wei_data = 0x01;
  for(unsigned char n = 0 ; n < 8 ; n ++)
  {
    unsigned char data = 0;
    for(unsigned char i = 0 ; i < 7 ; i ++)
    {
      if(data_buf[i] & wei_data) data ++;
    }
    if((data % 2) != 0) data_buf[7] |= wei_data;
    
    wei_data <<= 1;
  }
  //data_buf[7] ^= 0x03;
  data_buf[8] = data_buf[7];
  
  UpDataToSendBuf();
}

void UpDate(void)
{
  signed char i;
  uint32_t data;
  unsigned char len;
  unsigned char addr = 0;
  unsigned char addr_i = 0;
  /*
  sysdata.Versions = 2;
  sysdata.Tamper = 0;
  sysdata.Battery = 1;
  sysdata.MSID = 456789;
  sysdata.MSV = 999999;
  */
  for(unsigned char n = 0 ; n < 6 ; n ++)
  {
    if(n == 0)
    {
      data = sysdata.Versions;
      len = THVERSIONS_LEN;
    }
    else if(n == 1)
    {
      data = sysdata.Tamper;
      len = THTAMPER_LEN;
    }
    else if(n == 2)
    {
      data = sysdata.Battery;
      len = THBATTERY_LEN;
    }
    else if(n == 3)
    {
      data = sysdata.MSID;
      len = THMSID_LEN;
    }
    else if(n == 4)
    {
      data = sysdata.MSV;
      len = THMSV_LEN;
    }
    else
    {
      if(sysflag.CloseFlag == 1)
      {
        if(sysflag.DLFlag) data = 5;
        else if(sysflag.XLFlag) data = 6;
        else if(sysflag.MFFlag) data = 7;
        else if(sysflag.TimeFlag) data = 3;
        else data = 4;
      }
      else if(sysflag.CloseFlag == 2)
      {
        //if(sysflag.XLFlag) data = 2;
        if(sysflag.TimeFlag) data = 1;
      }
      else if(sysflag.CloseFlag == 0)
      {
        if(sysdata.TSY_Sec < sysdata.TBJ_Sec) data = 1;
        else if(sysflag.XLFlag) data = 6;
        else data = 0;
      }
      if(sysflag.ZlErrorFlag) data = 2;
      
      data = (data << 2);
      //data = 8;
      len = 5;
    }
    data <<= (32 - len);
    for(i = 0 ; i < len ; i ++)
    {
      data_buf[addr] <<= 1;
      if(data & 0x80000000)
      {
        data_buf[addr] |= 1;
      }
      data <<= 1;
      
      addr_i ++;
      if(addr_i == 7)
      {
        data_buf[addr] <<= 1;
        addr_i = 0;
        addr ++;
      }
    }
  }
  DataCheck();
}

void UpDateR2_1(void)
{
  signed char i;
  uint32_t data;
  unsigned char len;
  unsigned char addr = 0;
  unsigned char addr_i = 0;
  /*
  sysdata.Versions = 2;
  sysdata.Tamper = 0;
  sysdata.Battery = 1;
  sysdata.MSID = 456789;
  sysdata.MSV = 999999;
  */
  for(unsigned char n = 0 ; n < 3 ; n ++)
  {
    if(n == 0)
    {
      data = MS.Versions;
      len = 2;
    }
    else if(n == 1)
    {
      data = MS.MSID;
      len = 27;
    }
    else if(n == 2)
    {
      data = MS.MSV;
      len = 20;
    }
    
    data <<= (32 - len);
    for(i = 0 ; i < len ; i ++)
    {
      data_buf[addr] <<= 1;
      if(data & 0x80000000)
      {
        data_buf[addr] |= 1;
      }
      data <<= 1;
      
      addr_i ++;
      if(addr_i == 7)
      {
        data_buf[addr] <<= 1;
        addr_i = 0;
        addr ++;
      }
    }
  }
  DataCheck();
}

void UpDateR2_2(void)
{
  signed char i;
  uint32_t data;
  unsigned char len;
  unsigned char addr = 0;
  unsigned char addr_i = 0;
  /*
  sysdata.Versions = 2;
  sysdata.Tamper = 0;
  sysdata.Battery = 1;
  sysdata.MSID = 456789;
  sysdata.MSV = 999999;
  */
  for(unsigned char n = 0 ; n < 5 ; n ++)
  {
    if(n == 0)
    {
      data = MS.Versions;
      len = 2;
    }
    else if(n == 1)
    {
      data = MS.MSID;
      len = 27;
    }
    else if(n == 2)
    {
      data = 0x1f;
      len = 5;
    }
    else if(n == 3)
    {
      data = 0x00;
      if(MS.Tamper) data |= 0x200;
      if(MS.Battery)data|= 0x100;  //低电量
      if(MS.LBattery)data|= 0x080;  //超低电量

      if(Zenner.ValveStatus == 1) data|= 0x040;
      if(Zenner.DLFlag1 || Zenner.DLFlag2) data|= 0x020;
      if(Zenner.XLFlag1 || Zenner.XLFlag2) data|= 0x010;
      if(Zenner.TimeFlag ||Zenner.TimeBaoJingFlag/* && Zenner.ValveStatus*/) data|= 0x008;
      if(Zenner.ValveErrorStatus == 2 || Zenner.ValveCheckErrorStatus == 2) data|= 0x004;//断开
      if(Zenner.ValveErrorStatus == 1) data|= 0x002;//堵转
      if(Zenner.QMFlag) data|= 0x001;
      len = 10;
    }
    else if(n == 4)
    {
      data = 0x00;
      len = 5;
    }
    
    data <<= (32 - len);
    for(i = 0 ; i < len ; i ++)
    {
      data_buf[addr] <<= 1;
      if(data & 0x80000000)
      {
        data_buf[addr] |= 1;
      }
      data <<= 1;
      
      addr_i ++;
      if(addr_i == 7)
      {
        data_buf[addr] <<= 1;
        addr_i = 0;
        addr ++;
      }
    }
  }
  DataCheck();
}

void MsSend(void)
{
  SPI_init();       //SPI初始化

  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);         //开晶振
  Delay(6000);
  
  AX5043_set_registers();           //配置发射芯片
  
  simple_autorange_pll();           //发送芯片自动锁频
  
  UpDateR2_1();         //打包数据到发送缓存
  //UpDate();
  
  simple_transmit(send_buf,28);             //装填发送数据，并发送
  
  Delay(6000);
  UpDateR2_2();         //打包数据到发送缓存
  simple_transmit(send_buf,28);             //装填发送数据，并发送
  
  SpiWriteSingleAddressRegister(0x01, 0x01);
  SpiWriteSingleAddressRegister(0x02, 0x00);
  

  GPIOC->ODR &= ~GPIO_Pin_4; 
  
  Delay(300);
  
  SpiWriteSingleAddressRegister(0x02, 0x01);                //发送芯片进入低功耗
}

void MSSendWithSave(void)
{
  RecorderSaveOne();
  IWDG_ReloadCounter();             //喂狗
  SysdataReadBattery1();
  
  MsSend();         //发射
 
  SysdataReadBatteryEnd();              //关电量检测 
  if(sysdata.PVDBatteryFlag) MS.Battery = 1;         //如果没电，就置位没电标志位
  else MS.Battery = 0;             //否则等于0，复位没电标志位
  
  /******************************/
  if(sysdata.PVDLBatteryFlag1 || sysdata.PVDLBatteryFlag2 )MS.LBattery = 1;         //超低电量标志位
  else MS.LBattery = 0;             
  /**************************************/
  
  SysdataUpdateTamper();            //读取干扰位

  //SysdataSave();            //每发送一次保存一次数据
  MSDataSave();
}

void MsSend2Min(void)
{
  SysdataReadBattery1();
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
  SPI_init();
  Delay(6000);
  
  AX5043_Reset();
  AX5043_set_registers_0db();//发射0db
  simple_autorange_pll();
    
  send2min();
  
  SpiWriteSingleAddressRegister(0x01, 0x01);
  SpiWriteSingleAddressRegister(0x02, 0x00);
  
  GPIOC->ODR &= ~GPIO_Pin_4;
  
  Delay(300);
  SpiWriteSingleAddressRegister(0x02, 0x01);
  
  SysdataReadBatteryEnd();              //关电量检测
}


