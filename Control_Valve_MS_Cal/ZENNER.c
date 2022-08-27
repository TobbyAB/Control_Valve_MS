#include "sysdata.h"

sZenner Zenner;

#define ZENNER_OPEN 0
#define ZENNER_CLOSE 1

void Adc_Init(void);
void Adc_DeInit(void);
u16 Adc_ReadState(void);
void ZennerChangeStart(void);
u16 Adc_ReadState1(void);
u16 Adc_ReadState2(void);

#define ZENNER_DATA_ADDR1 0X80
#define ZENNER_DATA_ADDR2 0XC0
#define ZENNER_DATA_SIZE 54+4

#define PEN1_ON (GPIOE->ODR &= ~0X10)
#define PEN1_OFF (GPIOE->ODR |= 0X10)

#define PEN3_OFF (GPIOD->ODR &= ~0X01)
#define PEN3_ON (GPIOD->ODR |= 0X01)
/*
void ZennerDataInitForTest(void)
{
  Zenner.SaveFlag = 0xaa;          //存储数据有效位-起始位
  
  Zenner.DLFlag1 = 0;           //超流量-条件1-触发标志
  Zenner.DLSecSet1 = 15;         //超流量-条件1-检测间隔计时
  Zenner.DLTimesSet1 = 5;       //超流量-条件1-检测次数设置
  Zenner.DLFlag2 = 0;           //超流量-条件2-触发标志
  Zenner.DLSecSet2 = 10;         //超流量-条件2-检测间隔计时
  Zenner.DLTimesSet2 = 3;       //超流量-条件2-检测次数设置
  
  Zenner.XLFlag1 = 0;           //连续流量-条件1-触发标志
  Zenner.XLSecSet1 = 3600;        //连续流量-条件1-检测间隔设置
  Zenner.XLTimesSet1 = 24;       //连续流量-条件1-检测次数设置
  Zenner.XLFlag2 = 0;           //连续流量-条件2-触发标志
  Zenner.XLSecSet2 = 10800;        //连续流量-条件2-检测间隔设置
  Zenner.XLTimesSet2 = 24;       //连续流量-条件2-检测次数设置
  Zenner.XLCloseEnableFlag = 1; //连续流量-报警关阀使能设置
  
  Zenner.TimeFlag = 0;                  //安检-触发标志
  Zenner.TimeShengYuSec = 194400000;           //安检-剩余时间
  Zenner.TimeBaoJingSec = 15552000;           //安检-提醒时间
  Zenner.TimeFlagSendTimeSet = 30;      //安检-循环报警发送时间设置
  Zenner.TimeFlagSendTimesSet = 24;     //安检-循环报警发送次数设置
  Zenner.TimeQuanTime = 7190;
  
  Zenner.QMFlag = 0;                    //气密性-触发标志
  Zenner.QMFlagSendTimeSet = 30;        //气密性-循环报警发送时间设置
  Zenner.QMFlagSendTimesSet = 24;       //气密性-循环报警发送次数设置
  
  Zenner.ValveStatus = 0;       //阀门状态-0：开启 1：关闭
  Zenner.ValveErrorStatus = 0;  //阀门故障状态-0：正常 1：堵转 2：断开
  Zenner.ValveCheckErrorStatus = 0;     //阀门每日检测状态-0：正常 2：断开
  
  Zenner.ValveData1 = 0;
  Zenner.ValveData2 = 0;
  Zenner.Reserved1 = 0;
  Zenner.Reserved2 = 0;
  
  Zenner.MeterFlag = 0;         //炉具标识
}*/

void ZennerDataInit(void)
{
  Zenner.SaveFlag = 0xaa;          //存储数据有效位-起始位
  
  Zenner.DLFlag1 = 0;           //超流量-条件1-触发标志
  Zenner.DLSecSet1 = 22;         //超流量-条件1-检测间隔计时
  Zenner.DLTimesSet1 = 190;       //超流量-条件1-检测次数设置
  Zenner.DLFlag2 = 0;           //超流量-条件2-触发标志
  Zenner.DLSecSet2 = 10;         //超流量-条件2-检测间隔计时
  Zenner.DLTimesSet2 = 6;       //超流量-条件2-检测次数设置//
  Zenner.DL_Pulse = 1;  //使能
  Zenner.DL_Pulse2 = 1;  //使能
  
  Zenner.XLFlag1 = 0;           //连续流量-条件1-触发标志
  Zenner.XLSecSet1 = 3600;        //连续流量-条件1-检测间隔设置
  Zenner.XLTimesSet1 = 24;       //连续流量-条件1-检测次数设置
  Zenner.XLFlag2 = 0;           //连续流量-条件2-触发标志
  Zenner.XLSecSet2 = 10800;        //连续流量-条件2-检测间隔设置
  Zenner.XLTimesSet2 = 24;       //连续流量-条件2-检测次数设置
  Zenner.XLCloseEnableFlag = 1; //连续流量-报警关阀使能设置
  
  Zenner.TimeFlag = 0;                  //安检-触发标志
  Zenner.TimeShengYuSec = 194400000;           //安检-剩余时间
  Zenner.TimeBaoJingSec = 15552000;           //安检-提醒时间
  Zenner.TimeFlagSendTimeSet = 3600;      //安检-循环报警发送时间设置
  Zenner.TimeFlagSendTimesSet = 72;     //安检-循环报警发送次数设置        取消
  
  Zenner.QMFlag = 0;                    //气密性-触发标志
  Zenner.QMFlagSendTimeSet = 3600;        //气密性-循环报警发送时间设置
  Zenner.QMFlagSendTimesSet = 72;       //气密性-循环报警发送次数设置  取消
  
  Zenner.ValveStatus = 0;       //阀门状态-0：开启 1：关闭
  Zenner.ValveErrorStatus = 0;  //阀门故障状态-0：正常 1：堵转 2：断开
  
  Zenner.ValveData1 = 0;
  Zenner.ValveData2 = 0;
  Zenner.Reserved1 = 0;
  Zenner.Reserved2 = 0;
  
  Zenner.MeterFlag = 0;         //炉具标识
}

void ZennerWorkDataInit(void)//标志位清除
{
  
  Zenner.DLFlag1 = 0;           //超流量-条件1-触发标志
  Zenner.DLFlag2 = 0;           //超流量-条件2-触发标志
  Zenner.XLFlag1 = 0;           //连续流量-条件1-触发标志
  Zenner.XLFlag2 = 0;           //连续流量-条件2-触发标志
  Zenner.TimeFlag = 0;                  //安检-触发标志
  Zenner.QMFlag = 0;                    //气密性-触发标志
  Zenner.ValveErrorStatus = 0;  //阀门故障状态-0：正常 1：堵转 2：断开
  Zenner.ValveCheckErrorStatus = 0;     //阀门每日检测状态-0：正常 2：断开
  /**************************************************************/
  Zenner.DLStartFlag1 = 0;//超流量-条件1-检测开始标志
  Zenner.DLSecConut1 = 0;//超流量-条件1-检测间隔计时
  Zenner.DLTimesCount1 = 0;//超流量-条件1-检测次数计数
  Zenner.DLPulseCount1 = 0;//超流量-条件1-单周期脉冲计数
  Zenner.DLChangeFlag1 = 0;//超流量-条件1-提前启动充电标志
  Zenner.DLChangeTimeCount1 = 0;//超流量-条件1-提前充电超时计时
  
  Zenner.DLStartFlag2 = 0;//超流量-条件2-检测开始标志
  Zenner.DLSecConut2 = 0;//超流量-条件2-检测间隔计时
  Zenner.DLTimesCount2 = 0;//超流量-条件2-检测次数计数
  Zenner.DLPulseCount2 = 0;//超流量-条件2-单周期脉冲计数
  Zenner.DLChangeFlag2 = 0;//超流量-条件2-提前启动充电标志
  Zenner.DLChangeTimeCount2 = 0;//超流量-条件2-提前充电超时计时
  /**************************************************************/

  /**************************************************************/
  Zenner.XLSecConut1 = 0;//连续流量-条件1-检测间隔计时
  Zenner.XLTimesCount1 = 0;//连续流量-条件1-检测次数计数
  Zenner.XLStartFlag1 = 0;//连续流量-条件1-检测启动标志
  Zenner.XLPulseCount1 = 0;//连续流量-条件1-单周期脉冲计数
  
  Zenner.XLSecConut2 = 0;//连续流量-条件2-检测间隔计时
  Zenner.XLTimesCount2 = 0;//连续流量-条件2-检测次数计数
  Zenner.XLStartFlag2 = 0;//连续流量-条件2-检测启动标志
  Zenner.XLPulseCount2 = 0;//连续流量-条件2-单周期脉冲计数
  /**************************************************************/
  
  /**************************************************************/
  Zenner.TimeFlagSendTimeCount = 0;//安检-循环报警发送时间计时
  Zenner.TimeFlagSendTimesCount = 0;//安检-循环报警发送次数计数
  Zenner.TimeQuanTime = 0;//安检-空闲两小时计时
  Zenner.TimeBaoJingFlag = 0;//安检-提醒标志
  /**************************************************************/
  
  /**************************************************************/
  Zenner.QMFlagSendTimeCount = 0;//气密性-循环报警发送时间计时
  Zenner.QMFlagSendTimesCount = 0;//气密性-循环报警发送次数计数
  Zenner.ValveCheckErrorStatus = 0;     //阀门每日检测状态-0：正常 2：断开
  Zenner.ErrorLevel3Flag = 0;//等级3报警标志
  
}
void ZennerDataSave(void)
{
  WriteDataWithCheckBackup(ZENNER_DATA_ADDR1,ZENNER_DATA_ADDR2,&(Zenner.SaveFlag),ZENNER_DATA_SIZE);
}
void ZennerDataRead(void)
{
  if(ReadDataWithCheckBackup(ZENNER_DATA_ADDR1,ZENNER_DATA_ADDR2,&(Zenner.SaveFlag),ZENNER_DATA_SIZE))
  {
    sysdata.SyadataError = 1;
    
    ZennerDataInit();
    write_eeprom(ZENNER_DATA_ADDR1,&(Zenner.SaveFlag),ZENNER_DATA_SIZE);
    write_eeprom(ZENNER_DATA_ADDR2,&(Zenner.SaveFlag),ZENNER_DATA_SIZE);
    ZennerDataSave();
  }
}

float ZennerADC1,ZennerADC2,vcc1,vcc2;

u8 ZennerValveDoWhile(unsigned char da)
{
  return ZennerValveDo(da);
}

u8 ZennerValveCheck(void)
{
  u8 res = 0;
  //GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);//T1
  GPIOE->ODR |= 0X04;//mos
  //Delay(10000);//50ms
  
  //2
  GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Slow);//T1
  Delay(100);//0.5ms
  GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);//T1
  Delay(200);//1ms
  
  
  res = GPIOE->IDR;
  res &= 0x08;
  if(res == 0) Zenner.ValveCheckErrorStatus = 0;
  else Zenner.ValveCheckErrorStatus = 2;
  Delay(200);//1ms
  //if((GPIOE->IDR & 0X08) == 0) res = 1;
  GPIOE->ODR &= ~0X04;//mos
  GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow);//T1
  
  //if(res == 0) Zenner.ValveCheckErrorStatus = 2;
  //else Zenner.ValveCheckErrorStatus = 0;
  
  return 0;
}

//u8 ZennerValveCheck(void)
//{
//  u8 count1=0,i;//记录高低电压次数
//  int adc_value=0,adc_value2=0;
//  SysdataReadBattery2(); //阀门电压检测 
//  Adc_Init();
//  PEN1_ON;
//  PEN3_ON;
//  Delay(1000);//5ms
//  GPIOD->ODR &= ~0X08;
//  GPIOD->ODR |= 0X04;
//  Delay(1000);//5ms
//  IWDG_ReloadCounter();               //喂狗
//  adc_value = Adc_ReadState1();//vcc对应寄存器
//  vcc1 = 1.225*4096/adc_value;
//  for(i = 0;i<10;i++)
//  {
//    adc_value2 = Adc_ReadState2();  //当时的电压寄存器值
//    ZennerADC1 =vcc1 * adc_value2/4096 ;  //反馈电压
//    if((ZennerADC1>=0.2)) count1 ++;
//    Delay(200);//1ms
//  }
//  //动作结束
//  Adc_DeInit(); 
//  GPIOD->ODR &= ~0X04;
//  GPIOD->ODR &= ~0X08;  
//  PEN1_OFF;
//  PEN3_OFF;
//  Delay(6000);//30ms
//  if(count1 >= 5) return 1;
//  else return 0;
//}

u8 ZennerValveDo(unsigned char da)
{
  //u8 data[10] = {0};
  u8 count1=0,i;//记录高低电压次数
  u8 count2=0;//记录高低电压次数
  u8 count3=0;//记录高低电压次数
  int adc_value=0,adc_value2=0;
  SysdataReadBattery2(); //阀门电压检测 
  Adc_Init();
  PEN1_ON;
  PEN3_ON;
  Delay(1000);

  if(da == ZENNER_OPEN)        //open
  {
    GPIOD->ODR &= ~0X08;
    GPIOD->ODR |= 0X04;
  }
  else          //close
  {
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR |= 0X08;
  }
  Delay(60000);//300ms
  Delay(60000);//600ms
  Delay(60000);//900ms
  for(i = 0;i<21;i++)
  {
      IWDG_ReloadCounter();               //喂狗
      adc_value = Adc_ReadState1();//vcc对应寄存器
      vcc1 = 1.225*4096/adc_value;
      adc_value2 = Adc_ReadState2();  //当时的电压寄存器值
      ZennerADC1 =vcc1 * adc_value2/4096 ;  //反馈电压
      
      if((ZennerADC1>=1.2&&ZennerADC1<=1.8))//ZENNER
      {
        count1++;
      }
      
       if((ZennerADC1>=2.0&&ZennerADC1<=2.8))//Itron
      {
        count2++;
      }
       if((ZennerADC1<=1))
      {
        count3++;
      }
      
      
        Delay(30000);//150ms   0.15*20=3s
      
  }
  //动作结束
  Adc_DeInit(); 
  GPIOD->ODR &= ~0X04;
  GPIOD->ODR &= ~0X04;
  GPIOD->ODR &= ~0X08;  
  PEN1_OFF;
  PEN3_OFF;
  SysdataReadBatteryEnd();//阀门电压检测关
   
  if((count1>20)||(count2>20))
  {
    return 1;//堵转
  }
  else if(count3>=20)
  {
    return 2;//断开
  }
  else
    return 0;
  }

/*********客户阀门增加适应更改begin****************
 float first;
u8 ZennerValveDo(unsigned char da)
{
  //u8 data[10] = {0};//测试打印字符串
  float ZennerADC1,vcc1;
  int adc_value=0,adc_value2=0;
  SysdataReadBattery2(); //阀门电压检测
  Adc_Init();
  PEN1_ON;
  PEN3_ON;
  
  Delay(1000);
  
  if(da == ZENNER_OPEN)        //open
  {
    GPIOD->ODR &= ~0X08;
    GPIOD->ODR |= 0X04;
  }
  else          //close
  {
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR |= 0X08;
  }
  Delay(60000);//300ms
  Delay(60000);//600ms
  Delay(20000);//700ms
  IWDG_ReloadCounter(); 
  
  adc_value = Adc_ReadState1();//vcc对应寄存器
  vcc1 = 1.225*4096/adc_value;
  adc_value2 = Adc_ReadState2();  //当时的电压寄存器值
  first =vcc1 * adc_value2/4096 ;  //反馈电压
  IWDG_ReloadCounter();
  ZennerADC1=first;
  if(ZennerADC1>2.0) //大于2v   堵转状态直接退出
  {
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//阀门电压检测关
    return 1;//堵转
  }
  else if(ZennerADC1<1.0&&ZennerADC1>0.35)
  {
      //延时0.8s
      Delay(60000);//1000ms
      Delay(60000);//1300ms
      Delay(40000);//1500ms
      IWDG_ReloadCounter();
      for(u8 i=1 ;i<=25;i++)
      {
        adc_value = Adc_ReadState1();//vcc对应寄存器
        vcc1 = 1.225*4096/adc_value;
        adc_value2 = Adc_ReadState2();  //当时的电压寄存器值
        ZennerADC1 =vcc1 * adc_value2/4096 ;  //反馈电压
        IWDG_ReloadCounter();
        
        if(ZennerADC1>1.0)
        {
          Adc_DeInit(); 
          GPIOD->ODR &= ~0X04;
          GPIOD->ODR &= ~0X08;  
          PEN1_OFF;
          PEN3_OFF;
          SysdataReadBatteryEnd();//阀门电压检测关
          return 0;
        }  
        Delay(40000);//每次延时200ms   24*200ms=4800ms
      }
      Adc_DeInit(); 
      GPIOD->ODR &= ~0X04;
      GPIOD->ODR &= ~0X08;  
      PEN1_OFF;
      PEN3_OFF;
      SysdataReadBatteryEnd();//阀门电压检测关
      return 1;
  }
  else if(ZennerADC1<1.8&&ZennerADC1>1.0)
  {
    //延时0.8s
    Delay(60000);//1000ms
    Delay(60000);//1300ms
    Delay(40000);//1500ms    
    for(u8 i=1 ;i<=18;i++)
    {
       adc_value = Adc_ReadState1();//vcc对应寄存器
        vcc1 = 1.225*4096/adc_value;
        adc_value2 = Adc_ReadState2();  //当时的电压寄存器值
        ZennerADC1 =vcc1 * adc_value2/4096 ;  //反馈电压
        IWDG_ReloadCounter();
      if(ZennerADC1>2.0)
      {
        Adc_DeInit(); 
        GPIOD->ODR &= ~0X04;
        GPIOD->ODR &= ~0X08;  
        PEN1_OFF;
        PEN3_OFF;
        SysdataReadBatteryEnd();//阀门电压检测关
        return 0;
      }  
      Delay(40000);//每次延时200ms   18*200ms=3600ms
    }
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//阀门电压检测关
    return 1;  
  }  
  else //第一次测试小于0.2v   断开状态直接退出
  {
          
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//阀门电压检测关
    return 2;//断开
  }
}*/
/*********客户阀门增加适应更改end***************/

void ZennerUsartChangeStart(void)
{
  Zenner.UsartChangeFlag = 1;
}
void ZennerUsartChangeStop(void)
{
  Zenner.UsartChangeFlag = 0;
}
void ZennerChangeStart(void)
{
}

void ZennerChangeWork(void)
{
  //充电标志
 if(Zenner.UsartChangeFlag || ((Zenner.DLChangeFlag1 || Zenner.DLChangeFlag2 || Zenner.DLFlag1 || Zenner.DLFlag2 || ((Zenner.XLFlag1 || Zenner.XLFlag2) && Zenner.XLCloseEnableFlag) || Zenner.TimeFlag) && Zenner.ValveStatus == 0))
  {
    ZennerChangeStart();
    
    //充电
    if(Zenner.ChangeFlag)
    {
      if(Zenner.ChangeTime < 200) Zenner.ChangeTime ++;
      
      if(Zenner.ChangeTime == ZENNER_CHANGE_TIME)
      {
        Zenner.ChangeOK = 1;
      }
    }
  }
}

void ZennerErrorLevel3(void)
{
  if(Zenner.ErrorLevel3Flag == 0)
  {
    Zenner.ErrorLevel3Flag = 1;
    Zenner.TimeFlagSendTimeCount = 0;
    Zenner.TimeFlagSendTimesCount = 0;
    sysflag.send = 1;
    sysflag.FastSleep = 0;
    if(Zenner.TimeFlagSendTimesSet && Zenner.TimeFlagSendTimeSet) Zenner.QMFlagSendTimesCount = Zenner.QMFlagSendTimesSet;
  }
}

u32 XL1_MSVtime;//记录相邻msv间隔的时间
u32 DL1_MSVtime;//记录相邻msv间隔的时间
void ZennerPulseLogic(void)
{
  XL1_MSVtime = 0;//脉冲到来，时间清除
  DL1_MSVtime = 0;//脉冲到来，时间清除
  //强制关阀pulse计时复位
  Zenner.TimeQuanTime = 0;
  
  //气密性报警
  if(Zenner.ValveStatus == 1)
  {
    if(Zenner.QMFlag == 0)
    {
      Zenner.QMFlag = 1;
      
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
  }
  
  //连续流量-小漏-检测启动1
  if(Zenner.XLTimesSet1)
  {
    if(Zenner.XLPulseCount1 < 255)
    {
      if(Zenner.XLStartFlag1 == 0) Zenner.XLStartFlag1 = 1;
      else Zenner.XLPulseCount1 ++;
    }
  }
  //连续流量-小漏-检测启动2
  if(Zenner.XLTimesSet2)
  {
    if(Zenner.XLPulseCount2 < 255)
    {
      if(Zenner.XLStartFlag2 == 0) Zenner.XLStartFlag2 = 1;
      else Zenner.XLPulseCount2 ++;
    }
  }
  
  //超流量-大漏-检测启动1
  if(Zenner.DLTimesSet1&&Zenner.DLSecSet1&&Zenner.DL_Pulse2)
  {
    if(Zenner.DLPulseCount1 < 255)
    {
      if(Zenner.DLStartFlag1 == 0) Zenner.DLStartFlag1 = 1;
      else Zenner.DLPulseCount1 ++;
    }
  }
  //超流量-大漏-检测启动2
  if(Zenner.DLTimesSet2&&Zenner.DLSecSet2&&Zenner.DL_Pulse)
  {
    if(Zenner.DLStartFlag2 == 0) Zenner.DLStartFlag2 = 1;
    else
    {
      if(Zenner.DLSecConut2 <= Zenner.DLSecSet2)
      {
        Zenner.DLSecConut2 = 0;
        Zenner.DLTimesCount2 ++;
        if(Zenner.DLTimesCount2 >= Zenner.DLTimesSet2)
        {
          Zenner.DLFlag2 = 1;
          if(Zenner.QMFlag)
          {
            ZennerErrorLevel3();
          }
          else Zenner.DLTimesCount2 = 0;
        }
        if(Zenner.DLTimesCount2 + 2 >= Zenner.DLTimesSet2) Zenner.DLChangeFlag2 = 1;
      }
      else
      {
        Zenner.DLSecConut2 = 0;
        Zenner.DLTimesCount2 = 0;
        Zenner.DLStartFlag2 = 0;
        Zenner.DLChangeFlag2 = 0;
      }
    }
  }
}

void ZennerDLLogicWork(void)
{
  //超流量-大漏1
  /*if(Zenner.DLTimesSet1&&Zenner.DLSecSet1)
  {
    //超流量-大漏充电超时1
    if(Zenner.DLChangeFlag1)
    {
      if(Zenner.DLChangeTimeCount1 < 60) Zenner.DLChangeTimeCount1 ++;
      if(Zenner.DLChangeTimeCount1 >= 50)
      {
        Zenner.DLChangeTimeCount1 = 0;
        Zenner.DLChangeFlag1 = 0;
      }
    }
    else Zenner.DLChangeTimeCount1 = 0;
*/
    //超流量-大漏1
    if(Zenner.DLStartFlag1) Zenner.DLSecConut1 ++;
    else Zenner.DLSecConut1 = 0;
        /***********begin**************/
    if(DL1_MSVtime>Zenner.DLSecSet1)
    {
      //超出时间，认为没有触发DL条件
      Zenner.DLTimesCount1 = 0;//大周期成功计数
      Zenner.DLStartFlag1 = 0;
      Zenner.DLPulseCount1 = 0;//周期内脉冲计数
      Zenner.DLSecConut1 = 0;
    }
    /***********end**************/
    if(Zenner.DLSecConut1 >= Zenner.DLSecSet1)//DL1触发条件
    {
      Zenner.DLSecConut1 = 0;
      if(Zenner.DLPulseCount1)
      {
        Zenner.DLPulseCount1 = 0;       
        Zenner.DLTimesCount1 ++;
        if(Zenner.DLTimesCount1 >= Zenner.DLTimesSet1)
        {
          Zenner.DLFlag1 = 1;
          if(Zenner.QMFlag)
          {
            ZennerErrorLevel3();
          }
          else Zenner.DLTimesCount1 = 0;
        }
      }
      else
      {
        Zenner.DLTimesCount1 = 0;
        Zenner.DLStartFlag1 = 0;
        Zenner.DLChangeFlag1 = 0;
      }
      Zenner.DLPulseCount1 = 0;
    }
  //超流量-大漏2
  if(Zenner.DLTimesSet2 && Zenner.DLSecSet2)
  {
    //超流量-大漏充电超时2
    if(Zenner.DLChangeFlag2)
    {
      if(Zenner.DLChangeTimeCount2 < 60) Zenner.DLChangeTimeCount2 ++;
      if(Zenner.DLChangeTimeCount2 >= 40)
      {
        Zenner.DLChangeTimeCount2 = 0;
        Zenner.DLChangeFlag2 = 0;
      }
    }
    else Zenner.DLChangeTimeCount2 = 0;
    //超流量-大漏2
    if(Zenner.DLStartFlag2)
    {
      if(Zenner.DLSecConut2 < 255) Zenner.DLSecConut2 ++;
    }
    else Zenner.DLSecConut2 = 0;
  }
}

void ZennerXLLogicWork(void)
{
  //连续流量-小漏1
  if(Zenner.XLSecSet1)//时间间隔阙值
  {
    if(Zenner.XLStartFlag1) Zenner.XLSecConut1 ++;
    else 
    {
       Zenner.XLSecConut1 = 0;//时间间隔实际值清除
    }
    
    //判断XL1_MSVtime是否超出时间
    if(XL1_MSVtime>=Zenner.XLSecSet1)
    {
      //超出时间，认为没有触发XL条件
      //Zenner.XLSecConut1 = 0;固定的3小时  不需要清除
      Zenner.XLTimesCount1 = 0;
      Zenner.XLStartFlag1 = 0;
      Zenner.XLPulseCount1 = 0;  
    }
    
    if(Zenner.XLSecConut1 >= Zenner.XLSecSet1)//间隔时间判断
    {
      Zenner.XLSecConut1 = 0;
      if(Zenner.XLPulseCount1)
      {
        Zenner.XLTimesCount1 ++;
        if(Zenner.XLTimesCount1 >= Zenner.XLTimesSet1 && Zenner.XLTimesSet1)
        {
          Zenner.XLFlag1 = 1;
          if(Zenner.QMFlag)
          {
            ZennerErrorLevel3();
          }
          else Zenner.XLTimesCount1 = 0;
        }
      }
      else
      {
        Zenner.XLTimesCount1 = 0;
        Zenner.XLStartFlag1 = 0;
      }
      Zenner.XLPulseCount1 = 0;
    }
  }
  
  //连续流量-小漏2
  if(Zenner.XLSecSet2)
  {
    if(Zenner.XLStartFlag2) Zenner.XLSecConut2 ++;
    else Zenner.XLSecConut2 = 0;
    /***********begin**************/
    if(XL1_MSVtime>=Zenner.XLSecSet2)
    {
      //超出时间，认为没有触发XL条件
      Zenner.XLTimesCount2 = 0;
      Zenner.XLStartFlag2 = 0;
      Zenner.XLPulseCount2 = 0;
    }
    /***********end**************/
    if(Zenner.XLSecConut2 >= Zenner.XLSecSet2)
    {
      Zenner.XLSecConut2 = 0;
      if(Zenner.XLPulseCount2)
      {
        Zenner.XLTimesCount2 ++;
        if(Zenner.XLTimesCount2 >= Zenner.XLTimesSet2 && Zenner.XLTimesSet2)
        {
          Zenner.XLFlag2 = 1;
          if(Zenner.QMFlag)
          {
            ZennerErrorLevel3();
          }
          else Zenner.XLTimesCount2 = 0;
        }
      }
      else
      {
        Zenner.XLTimesCount2 = 0;
        Zenner.XLStartFlag2 = 0;
      }
      Zenner.XLPulseCount2 = 0;
    }
  }
}

void ZennerTimeLogicWork(void)
{
  //强制关阀
  if(Zenner.TimeShengYuSec)
  {
    Zenner.TimeShengYuSec --;
    if(Zenner.TimeShengYuSec == Zenner.TimeBaoJingSec)
    {
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
    if(Zenner.TimeShengYuSec == 0)
    {
      Zenner.TimeQuanTime = 0;
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
  }
  if(Zenner.TimeShengYuSec == 0 && Zenner.TimeQuanTime >= ZENNER_TIME_2HOURS && Zenner.TimeFlag == 0 && Zenner.TimeBaoJingSec) Zenner.TimeFlag = 1;
  //强制关阀报警
  if(Zenner.TimeShengYuSec <= Zenner.TimeBaoJingSec && Zenner.TimeBaoJingSec) Zenner.TimeBaoJingFlag = 1;
}

void ZennerAlarmLogicWork(void)
{
  //阀门故障循环报警
  //if(Zenner.ValveErrorStatus)
  if(Zenner.ValveStatus&&(Zenner.DLFlag1||Zenner.DLFlag2||Zenner.XLFlag1||Zenner.XLFlag2||Zenner.TimeFlag||Zenner.ValveErrorStatus))
  {
    Zenner.TimeFlagSendTimeCount ++;
    if(Zenner.TimeFlagSendTimeCount >= Zenner.TimeFlagSendTimeSet && Zenner.TimeFlagSendTimesCount < Zenner.TimeFlagSendTimesSet && Zenner.TimeFlagSendTimeSet && Zenner.TimeFlagSendTimesSet && (Zenner.QMFlag == 0 || Zenner.ErrorLevel3Flag))
    {
      Zenner.TimeFlagSendTimeCount = 0;
      Zenner.TimeFlagSendTimesCount ++;
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
  }
  else if(Zenner.ValveErrorStatus == 0)
  {
    Zenner.TimeFlagSendTimeCount = 0;
    Zenner.TimeFlagSendTimesCount = 0;
  }
  //气密性故障循环报警
  if(Zenner.QMFlag)
  {
    Zenner.QMFlagSendTimeCount ++;
    if(Zenner.QMFlagSendTimeCount >= Zenner.QMFlagSendTimeSet && Zenner.QMFlagSendTimesCount < Zenner.QMFlagSendTimesSet && Zenner.QMFlagSendTimeSet && Zenner.QMFlagSendTimesSet)
    {
      Zenner.TimeFlagSendTimesCount = Zenner.TimeFlagSendTimesSet;//取消阀门故障循环报警
      Zenner.QMFlagSendTimeCount = 0;
      Zenner.QMFlagSendTimesCount ++;
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
  }
  else if(Zenner.QMFlag == 0)
  {
    Zenner.QMFlagSendTimeCount = 0;
    Zenner.QMFlagSendTimesCount = 0;
  }
}

void ZennerLogicWork(void)
{
  XL1_MSVtime++;
  DL1_MSVtime++;
  //强制关阀pulse计时
  if(Zenner.TimeQuanTime < 10000) Zenner.TimeQuanTime ++;
  
  //超流量-大漏-逻辑服务函数
  ZennerDLLogicWork();
  
  //连续流量-小漏-逻辑服务函数
  ZennerXLLogicWork();
  
  //安检-逻辑服务函数
  ZennerTimeLogicWork();
  
  //循环报警-逻辑服务函数
  ZennerAlarmLogicWork();
}

void ZennerOneSecWork(void)
{
  ZennerLogicWork();            //逻辑服务
  
  ZennerChangeWork();           //充电服务
}

void ZennerCloseWork(void)
{
  //关阀标志
  if((Zenner.TimeFlag || Zenner.DLFlag1 || Zenner.DLFlag2 || ((Zenner.XLFlag1 || Zenner.XLFlag2) && Zenner.XLCloseEnableFlag)))
  {
    if(Zenner.ValveStatus == 0)         //充电完成
    {      
      Zenner.ValveErrorStatus = ZennerValveDo(ZENNER_CLOSE);       //关阀
      
      Zenner.ValveStatus = 1;
      
      sysflag.send = 1;
      sysflag.FastSleep = 0;
      //Zenner.TimeFlag=0;
    }
  }
}