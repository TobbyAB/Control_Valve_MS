//��е���� ����־
/*******************************************************************************/
/*Software Version��SUS0010�C010090*/
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
  Delay(6000);          //��ʱ���ȴ�ϵͳ��Դ�ȶ�
  
  CLK_init();           //�ڲ�����ʱ�ӳ�ʼ��
 
  SysdataReadBattery1();         //PVD����һ�Σ���ʼ��PVD���򿪵����������ܣ���ȡ����״̬
  
  //SysdataInit();                //ϵͳ���ݳ�ʼ������eeprom��ȡ��������Ĭ��ֵ
  MSDataRead();
  ZennerDataRead();
  
  RTC_Config();         //�ⲿ32.768Kʼ�ճ�ʼ�� + RTCʵʱʱ������
  
  WriteRTCAlarm(MS.MSID % 1024 * 84);              //����RTC����ʱ�䣬���Ƿ���ʱ�䣬1024��Ϊһ�飬ÿ�����÷��ͼ��ʱ��Ϊ84�룬һ��Լ1��
  LowPowerInit();               //�͹������� + IO������
  
/*******************5043��ʼ��������͹���**************************/
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);             
  SPI_init();           //SPI��ʼ��
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
  
  SysdataReadBatteryEnd();              //�ص������
    
  WriteRTCWakeUp(2047);         //RTC��ʱ���ѣ�2048 = 1�뻽��һ��
  
//  UartInit();
  
  RecorderInit();
  IWDGInit();           //���Ź���ʼ��
  
  sysflag.send = 1;
  while (1)
  {
    IWDG_ReloadCounter();               //ι��

    if(sysflag.uart == 0 && sysflag.send == 0 && sysflag.send2min == 0)         //���ڣ�����һ�Σ��������ͣ�������Ҫִ��
    {
      FastLowPowerInit();               //���ٵ͹��ĺ���
      do
      {
        sysflag.FastSleep = 1;          //����˯�߱�־����

        halt();         //����͹���˯��
        
        IWDG->KR = IWDG_KEY_REFRESH;    //���Ѻ�ι��
        
        ZennerCloseWork();
        
        sysdata.SaveNum ++;             //��ʱ40000�룬��������һ�Ρ�
        if(sysdata.SaveNum >= 40000)
        {
          sysdata.SaveNum = 0;

          //SysdataSave();                //��������
          MSDataSave();
          //MSSendWithSave();
        }
      }
      while(sysflag.FastSleep);         //����˯�߱�־���������Ҫ���������黽�Ѻ�ֱ��˯�ߣ�flag = 1��
      
      CLK_init();               //�����ڲ�����ʱ��
    }
    else if(sysflag.send)               //��Ҫִ�з���һ�η���
    {
      
      ZennerValveCheck();

      MSSendWithSave();
      
      ZennerDataSave();

      sysflag.send = 0;         //�巢�ͱ�־
    }
    else if(sysflag.uart)               //��Ҫִ�д��ڷ���
    {
       ZennerCloseWork();
      if(UartWork()) sysflag.uart = 0;          //ִ�д��ڷ��񣬷���1��ʾ��ִ����ɣ����־λ
      Delay(6000);
    }
    else if(sysflag.send2min)           //�����Ҫִ���������ͷ���
    {
      sysflag.send2min = 0;             //���־λ
      
      MsSend2Min();          //ִ���������ͷ�����
    }
  }
}


void RTC_Config(void)
{
  CLK_LSEConfig(CLK_LSE_ON);//ʹ��LSE
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);//RTCʱ��ԴLSE��1��Ƶ=32.768K
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);//�ȴ�LSE����

  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);//RTCʱ���ſ�ʹ��

  
  RTC_WaitForSynchro();
  
  
  RTC_ClearITPendingBit(RTC_IT_ALRA);
  RTC_ITConfig(RTC_IT_ALRA,ENABLE);//�����ж�
  
  
  //ITC_SetSoftwarePriority(RTC_CSSLSE_IRQn, ITC_PriorityLevel_3);//���ȼ�
  
  //enableInterrupts();
}

void CLK_init(void)
{
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8);         // 16M�ڲ�RC��8��Ƶ��ϵͳʱ��Ϊ2M 
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
  
    PWR_FastWakeUpCmd(ENABLE);  //���ٻ���ʹ��
    //CLK_HaltConfig(CLK_Halt_FastWakeup, ENABLE); 
    
    //����IO����͵�ƽ
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
    
  
    //GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_IT);//��ʼ���������������룬���ж�         //���ڸߵ�ƽ
    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_IT);//��ʼ���������������룬���ж�           //���ڵ͵�ƽ
    
    GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT);//��ʼ���������������룬���ж�
    //GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_IT);//��ʼ���������������룬���ж�
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_FL_IT);//��ʼ���������������룬���ж�
    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_IT);//��ʼ���������������룬���ж�
    
    EXTI_DeInit (); //�ָ��жϵ��������� 
    EXTI_SetPinSensitivity (EXTI_Pin_0,EXTI_Trigger_Rising_Falling);//�ⲿ�ж�1���½��ش�����������9
    EXTI_SetPinSensitivity (EXTI_Pin_1,EXTI_Trigger_Falling);//�ⲿ�ж�1���½��ش�����������9
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Falling);//�ⲿ�ж�1���½��ش�����������9

    //EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);//�ⲿ�ж�1���½��ش�����������9         //���ڸߵ�ƽ
    EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Rising);//�ⲿ�ж�1���½��ش�����������9        //���ڵ͵�ƽ
  
    PWR_UltraLowPowerCmd(ENABLE);//���͹���
    
    //RTC_ITConfig(RTC_IT_WUT, ENABLE);//���Ѷ�ʱ���ж�ʹ��
    //RTC_WakeUpCmd(ENABLE);//RTC����ʹ��
    
  //EXTI_ClearITPendingBit (EXTI_IT_Pin0);//����жϱ�־
  EXTI_ClearITPendingBit (EXTI_IT_Pin1);//����жϱ�־
  EXTI_ClearITPendingBit (EXTI_IT_Pin2);//����жϱ�־
  //EXTI_ClearITPendingBit (EXTI_IT_Pin3);//����жϱ�־
  
    
    //RTC_AlarmCmd(ENABLE);
    enableInterrupts();
}


void FastLowPowerInit(void)
{
    PWR_FastWakeUpCmd(ENABLE);  //���ٻ���ʹ��
    //GPIO_Init(GPIOB, GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_High_Slow);
    
    GPIOB->ODR |= GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    
    //GPIO_Init(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);
    //GPIOB->ODR |= GPIO_Pin_3;
    
    PWR_UltraLowPowerCmd(ENABLE);//���͹���
    //EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);//�ⲿ�ж�1���½��ش�����������9
}








