//机械阀门 旧日志
/*******************************************************************************/
/*Software Version：SUS0010–010090*/
#include "stm8l15x.h"
#include "stdio.h"
#include "string.h"

#include "spi.h"

#include "sysdata.h"
#include "uart.h"

#include "ax5043.h"

#include "recorder.h"

void PVDInit(void);
void PVDDeInit(void);
void RTC_Config(void);
void CLK_init(void);
void FastLowPowerInit(void);
void LowPowerInit(void);
void IWDGInit(void);

u32 TIME123=0 ;

int main(void)
{
  Delay(6000);          //延时，等待系统电源稳定
  
  CLK_init();           //内部高速时钟初始化
 
  SysdataReadBattery1();         //PVD工作一次，初始化PVD，打开电量测量功能，读取点量状态
  
  //SysdataInit();                //系统数据初始化，从eeprom读取，或设置默认值
  MSDataRead();
  ZennerDataRead();
  
  RTC_Config();         //外部32.768K始终初始化 + RTC实时时钟配置
  
  WriteRTCAlarm(MS.MSID % 1024 * 84);              //设置RTC闹钟时间，就是发送时间，1024个为一组，每个设置发送间隔时间为84秒，一共约1天
  LowPowerInit();               //低功耗设置 + IO口配置
  
/*******************5043初始化，进入低功耗**************************/
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);             
  SPI_init();           //SPI初始化
  Delay(6000);
  
  SpiWriteSingleAddressRegister(0x01, 0x01);
  SpiWriteSingleAddressRegister(0x02, 0x00);
  
  GPIOC->ODR &= ~GPIO_Pin_4;
  
  Delay(300);
  SpiWriteSingleAddressRegister(0x02, 0x01);
    
  Delay(6000);
  Delay(6000);
  Delay(6000);

  sysflag.send = 0;
  
  SysdataReadBatteryEnd();              //关电量检测
    
  WriteRTCWakeUp(2047);         //RTC定时唤醒，2048 = 1秒唤醒一次
  
//  UartInit();
  
  RecorderInit();
  IWDGInit();           //看门狗初始化
  
  sysflag.send = 1;
  while (1)
  {
    IWDG_ReloadCounter();               //喂狗

    if(sysflag.uart == 0 && sysflag.send == 0 && sysflag.send2min == 0)         //串口，发送一次，连续发送，都不需要执行
    {
      FastLowPowerInit();               //快速低功耗函数
      do
      {
        sysflag.FastSleep = 1;          //快速睡眠标志清零

        halt();         //进入低功耗睡眠
        
        IWDG->KR = IWDG_KEY_REFRESH;    //唤醒后喂狗
        
        ZennerCloseWork();
        
        sysdata.SaveNum ++;             //计时40000秒，保存数据一次。
        if(sysdata.SaveNum >= 40000)
        {
          sysdata.SaveNum = 0;

          //SysdataSave();                //保存数据
          MSDataSave();
          //MSSendWithSave();
        }
      }
      while(sysflag.FastSleep);         //快速睡眠标志，如果不需要处理别的事情唤醒后直接睡眠，flag = 1；
      
      CLK_init();               //开启内部高速时钟
    }
    else if(sysflag.send)               //需要执行发送一次服务
    {
      
      ZennerValveCheck();

      MSSendWithSave();
      
      ZennerDataSave();

      sysflag.send = 0;         //清发送标志
    }
    else if(sysflag.uart)               //需要执行串口服务
    {
       ZennerCloseWork();
      if(UartWork()) sysflag.uart = 0;          //执行串口服务，返回1表示，执行完成，清标志位
      Delay(6000);
    }
    else if(sysflag.send2min)           //如果需要执行连续发送服务
    {
      sysflag.send2min = 0;             //清标志位
      
      MsSend2Min();          //执行连续发送服务函数
    }
  }
}


void RTC_Config(void)
{
  CLK_LSEConfig(CLK_LSE_ON);//使能LSE
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);//RTC时钟源LSE，1分频=32.768K
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);//等待LSE就绪

  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);//RTC时钟门控使能

  
  RTC_WaitForSynchro();
  
  
  RTC_ClearITPendingBit(RTC_IT_ALRA);
  RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启中断
  
  
  //ITC_SetSoftwarePriority(RTC_CSSLSE_IRQn, ITC_PriorityLevel_3);//优先级
  
  //enableInterrupts();
}

void CLK_init(void)
{
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8);         // 16M内部RC经8分频后系统时钟为2M 
}

void IWDGInit(void)
{
  /*if (RST_GetFlagStatus(RST_FLAG_IWDGF) != RESET)
  {
    RST_ClearFlag(RST_FLAG_IWDGF);
  }*/
  
  IWDG_Enable();
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(255);
  IWDG_ReloadCounter();
  
}

void LowPowerInit(void)
{
  //disableInterrupts();
    //PWR_INC();
  
    PWR_FastWakeUpCmd(ENABLE);  //快速唤醒使能
    //CLK_HaltConfig(CLK_Halt_FastWakeup, ENABLE); 
    
    //所有IO输出低电平
    GPIO_Init(GPIOA, GPIO_Pin_All & ~(GPIO_Pin_3), GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_All & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2), GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOC, GPIO_Pin_All & ~GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOD, GPIO_Pin_All & 0x70, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOE, GPIO_Pin_All & ~(GPIO_Pin_4 | GPIO_Pin_5), GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOF, 1, GPIO_Mode_Out_PP_Low_Slow);
    
    GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
    GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);//PEN1
    GPIO_Init(GPIOE, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Slow);//PEN2
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);//PEN3
    GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow);//LBO
    GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);//M2
    GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow);//M1
    
    GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);//Power
    //GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Slow);//UART-TX
    
  
    //GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_IT);//初始化按键，上拉输入，带中断         //串口高电平
    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_IT);//初始化按键，上拉输入，带中断           //串口低电平
    
    GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT);//初始化按键，上拉输入，带中断
    //GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_IT);//初始化按键，上拉输入，带中断
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_FL_IT);//初始化按键，上拉输入，带中断
    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_IT);//初始化按键，上拉输入，带中断
    
    EXTI_DeInit (); //恢复中断的所有设置 
    EXTI_SetPinSensitivity (EXTI_Pin_0,EXTI_Trigger_Rising_Falling);//外部中断1，下降沿触发，向量号9
    EXTI_SetPinSensitivity (EXTI_Pin_1,EXTI_Trigger_Falling);//外部中断1，下降沿触发，向量号9
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Falling);//外部中断1，下降沿触发，向量号9

    //EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);//外部中断1，下降沿触发，向量号9         //串口高电平
    EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Rising);//外部中断1，下降沿触发，向量号9        //串口低电平
  
    PWR_UltraLowPowerCmd(ENABLE);//超低功耗
    
    //RTC_ITConfig(RTC_IT_WUT, ENABLE);//唤醒定时器中断使能
    //RTC_WakeUpCmd(ENABLE);//RTC唤醒使能
    
  //EXTI_ClearITPendingBit (EXTI_IT_Pin0);//清除中断标志
  EXTI_ClearITPendingBit (EXTI_IT_Pin1);//清除中断标志
  EXTI_ClearITPendingBit (EXTI_IT_Pin2);//清除中断标志
  //EXTI_ClearITPendingBit (EXTI_IT_Pin3);//清除中断标志
  
    
    //RTC_AlarmCmd(ENABLE);
    enableInterrupts();
}


void FastLowPowerInit(void)
{
    PWR_FastWakeUpCmd(ENABLE);  //快速唤醒使能
    //GPIO_Init(GPIOB, GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_High_Slow);
    
    GPIOB->ODR |= GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    
    //GPIO_Init(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);
    //GPIOB->ODR |= GPIO_Pin_3;
    
    PWR_UltraLowPowerCmd(ENABLE);//超低功耗
    //EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);//外部中断1，下降沿触发，向量号9
}








