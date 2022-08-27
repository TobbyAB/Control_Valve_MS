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
  Zenner.SaveFlag = 0xaa;          //�洢������Чλ-��ʼλ
  
  Zenner.DLFlag1 = 0;           //������-����1-������־
  Zenner.DLSecSet1 = 15;         //������-����1-�������ʱ
  Zenner.DLTimesSet1 = 5;       //������-����1-����������
  Zenner.DLFlag2 = 0;           //������-����2-������־
  Zenner.DLSecSet2 = 10;         //������-����2-�������ʱ
  Zenner.DLTimesSet2 = 3;       //������-����2-����������
  
  Zenner.XLFlag1 = 0;           //��������-����1-������־
  Zenner.XLSecSet1 = 3600;        //��������-����1-���������
  Zenner.XLTimesSet1 = 24;       //��������-����1-����������
  Zenner.XLFlag2 = 0;           //��������-����2-������־
  Zenner.XLSecSet2 = 10800;        //��������-����2-���������
  Zenner.XLTimesSet2 = 24;       //��������-����2-����������
  Zenner.XLCloseEnableFlag = 1; //��������-�����ط�ʹ������
  
  Zenner.TimeFlag = 0;                  //����-������־
  Zenner.TimeShengYuSec = 194400000;           //����-ʣ��ʱ��
  Zenner.TimeBaoJingSec = 15552000;           //����-����ʱ��
  Zenner.TimeFlagSendTimeSet = 30;      //����-ѭ����������ʱ������
  Zenner.TimeFlagSendTimesSet = 24;     //����-ѭ���������ʹ�������
  Zenner.TimeQuanTime = 7190;
  
  Zenner.QMFlag = 0;                    //������-������־
  Zenner.QMFlagSendTimeSet = 30;        //������-ѭ����������ʱ������
  Zenner.QMFlagSendTimesSet = 24;       //������-ѭ���������ʹ�������
  
  Zenner.ValveStatus = 0;       //����״̬-0������ 1���ر�
  Zenner.ValveErrorStatus = 0;  //���Ź���״̬-0������ 1����ת 2���Ͽ�
  Zenner.ValveCheckErrorStatus = 0;     //����ÿ�ռ��״̬-0������ 2���Ͽ�
  
  Zenner.ValveData1 = 0;
  Zenner.ValveData2 = 0;
  Zenner.Reserved1 = 0;
  Zenner.Reserved2 = 0;
  
  Zenner.MeterFlag = 0;         //¯�߱�ʶ
}*/

void ZennerDataInit(void)
{
  Zenner.SaveFlag = 0xaa;          //�洢������Чλ-��ʼλ
  
  Zenner.DLFlag1 = 0;           //������-����1-������־
  Zenner.DLSecSet1 = 22;         //������-����1-�������ʱ
  Zenner.DLTimesSet1 = 190;       //������-����1-����������
  Zenner.DLFlag2 = 0;           //������-����2-������־
  Zenner.DLSecSet2 = 10;         //������-����2-�������ʱ
  Zenner.DLTimesSet2 = 6;       //������-����2-����������//
  Zenner.DL_Pulse = 1;  //ʹ��
  Zenner.DL_Pulse2 = 1;  //ʹ��
  
  Zenner.XLFlag1 = 0;           //��������-����1-������־
  Zenner.XLSecSet1 = 3600;        //��������-����1-���������
  Zenner.XLTimesSet1 = 24;       //��������-����1-����������
  Zenner.XLFlag2 = 0;           //��������-����2-������־
  Zenner.XLSecSet2 = 10800;        //��������-����2-���������
  Zenner.XLTimesSet2 = 24;       //��������-����2-����������
  Zenner.XLCloseEnableFlag = 1; //��������-�����ط�ʹ������
  
  Zenner.TimeFlag = 0;                  //����-������־
  Zenner.TimeShengYuSec = 194400000;           //����-ʣ��ʱ��
  Zenner.TimeBaoJingSec = 15552000;           //����-����ʱ��
  Zenner.TimeFlagSendTimeSet = 3600;      //����-ѭ����������ʱ������
  Zenner.TimeFlagSendTimesSet = 72;     //����-ѭ���������ʹ�������        ȡ��
  
  Zenner.QMFlag = 0;                    //������-������־
  Zenner.QMFlagSendTimeSet = 3600;        //������-ѭ����������ʱ������
  Zenner.QMFlagSendTimesSet = 72;       //������-ѭ���������ʹ�������  ȡ��
  
  Zenner.ValveStatus = 0;       //����״̬-0������ 1���ر�
  Zenner.ValveErrorStatus = 0;  //���Ź���״̬-0������ 1����ת 2���Ͽ�
  
  Zenner.ValveData1 = 0;
  Zenner.ValveData2 = 0;
  Zenner.Reserved1 = 0;
  Zenner.Reserved2 = 0;
  
  Zenner.MeterFlag = 0;         //¯�߱�ʶ
}

void ZennerWorkDataInit(void)//��־λ���
{
  
  Zenner.DLFlag1 = 0;           //������-����1-������־
  Zenner.DLFlag2 = 0;           //������-����2-������־
  Zenner.XLFlag1 = 0;           //��������-����1-������־
  Zenner.XLFlag2 = 0;           //��������-����2-������־
  Zenner.TimeFlag = 0;                  //����-������־
  Zenner.QMFlag = 0;                    //������-������־
  Zenner.ValveErrorStatus = 0;  //���Ź���״̬-0������ 1����ת 2���Ͽ�
  Zenner.ValveCheckErrorStatus = 0;     //����ÿ�ռ��״̬-0������ 2���Ͽ�
  /**************************************************************/
  Zenner.DLStartFlag1 = 0;//������-����1-��⿪ʼ��־
  Zenner.DLSecConut1 = 0;//������-����1-�������ʱ
  Zenner.DLTimesCount1 = 0;//������-����1-����������
  Zenner.DLPulseCount1 = 0;//������-����1-�������������
  Zenner.DLChangeFlag1 = 0;//������-����1-��ǰ��������־
  Zenner.DLChangeTimeCount1 = 0;//������-����1-��ǰ��糬ʱ��ʱ
  
  Zenner.DLStartFlag2 = 0;//������-����2-��⿪ʼ��־
  Zenner.DLSecConut2 = 0;//������-����2-�������ʱ
  Zenner.DLTimesCount2 = 0;//������-����2-����������
  Zenner.DLPulseCount2 = 0;//������-����2-�������������
  Zenner.DLChangeFlag2 = 0;//������-����2-��ǰ��������־
  Zenner.DLChangeTimeCount2 = 0;//������-����2-��ǰ��糬ʱ��ʱ
  /**************************************************************/

  /**************************************************************/
  Zenner.XLSecConut1 = 0;//��������-����1-�������ʱ
  Zenner.XLTimesCount1 = 0;//��������-����1-����������
  Zenner.XLStartFlag1 = 0;//��������-����1-���������־
  Zenner.XLPulseCount1 = 0;//��������-����1-�������������
  
  Zenner.XLSecConut2 = 0;//��������-����2-�������ʱ
  Zenner.XLTimesCount2 = 0;//��������-����2-����������
  Zenner.XLStartFlag2 = 0;//��������-����2-���������־
  Zenner.XLPulseCount2 = 0;//��������-����2-�������������
  /**************************************************************/
  
  /**************************************************************/
  Zenner.TimeFlagSendTimeCount = 0;//����-ѭ����������ʱ���ʱ
  Zenner.TimeFlagSendTimesCount = 0;//����-ѭ���������ʹ�������
  Zenner.TimeQuanTime = 0;//����-������Сʱ��ʱ
  Zenner.TimeBaoJingFlag = 0;//����-���ѱ�־
  /**************************************************************/
  
  /**************************************************************/
  Zenner.QMFlagSendTimeCount = 0;//������-ѭ����������ʱ���ʱ
  Zenner.QMFlagSendTimesCount = 0;//������-ѭ���������ʹ�������
  Zenner.ValveCheckErrorStatus = 0;     //����ÿ�ռ��״̬-0������ 2���Ͽ�
  Zenner.ErrorLevel3Flag = 0;//�ȼ�3������־
  
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
//  u8 count1=0,i;//��¼�ߵ͵�ѹ����
//  int adc_value=0,adc_value2=0;
//  SysdataReadBattery2(); //���ŵ�ѹ��� 
//  Adc_Init();
//  PEN1_ON;
//  PEN3_ON;
//  Delay(1000);//5ms
//  GPIOD->ODR &= ~0X08;
//  GPIOD->ODR |= 0X04;
//  Delay(1000);//5ms
//  IWDG_ReloadCounter();               //ι��
//  adc_value = Adc_ReadState1();//vcc��Ӧ�Ĵ���
//  vcc1 = 1.225*4096/adc_value;
//  for(i = 0;i<10;i++)
//  {
//    adc_value2 = Adc_ReadState2();  //��ʱ�ĵ�ѹ�Ĵ���ֵ
//    ZennerADC1 =vcc1 * adc_value2/4096 ;  //������ѹ
//    if((ZennerADC1>=0.2)) count1 ++;
//    Delay(200);//1ms
//  }
//  //��������
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
  u8 count1=0,i;//��¼�ߵ͵�ѹ����
  u8 count2=0;//��¼�ߵ͵�ѹ����
  u8 count3=0;//��¼�ߵ͵�ѹ����
  int adc_value=0,adc_value2=0;
  SysdataReadBattery2(); //���ŵ�ѹ��� 
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
      IWDG_ReloadCounter();               //ι��
      adc_value = Adc_ReadState1();//vcc��Ӧ�Ĵ���
      vcc1 = 1.225*4096/adc_value;
      adc_value2 = Adc_ReadState2();  //��ʱ�ĵ�ѹ�Ĵ���ֵ
      ZennerADC1 =vcc1 * adc_value2/4096 ;  //������ѹ
      
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
  //��������
  Adc_DeInit(); 
  GPIOD->ODR &= ~0X04;
  GPIOD->ODR &= ~0X04;
  GPIOD->ODR &= ~0X08;  
  PEN1_OFF;
  PEN3_OFF;
  SysdataReadBatteryEnd();//���ŵ�ѹ����
   
  if((count1>20)||(count2>20))
  {
    return 1;//��ת
  }
  else if(count3>=20)
  {
    return 2;//�Ͽ�
  }
  else
    return 0;
  }

/*********�ͻ�����������Ӧ����begin****************
 float first;
u8 ZennerValveDo(unsigned char da)
{
  //u8 data[10] = {0};//���Դ�ӡ�ַ���
  float ZennerADC1,vcc1;
  int adc_value=0,adc_value2=0;
  SysdataReadBattery2(); //���ŵ�ѹ���
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
  
  adc_value = Adc_ReadState1();//vcc��Ӧ�Ĵ���
  vcc1 = 1.225*4096/adc_value;
  adc_value2 = Adc_ReadState2();  //��ʱ�ĵ�ѹ�Ĵ���ֵ
  first =vcc1 * adc_value2/4096 ;  //������ѹ
  IWDG_ReloadCounter();
  ZennerADC1=first;
  if(ZennerADC1>2.0) //����2v   ��ת״ֱ̬���˳�
  {
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//���ŵ�ѹ����
    return 1;//��ת
  }
  else if(ZennerADC1<1.0&&ZennerADC1>0.35)
  {
      //��ʱ0.8s
      Delay(60000);//1000ms
      Delay(60000);//1300ms
      Delay(40000);//1500ms
      IWDG_ReloadCounter();
      for(u8 i=1 ;i<=25;i++)
      {
        adc_value = Adc_ReadState1();//vcc��Ӧ�Ĵ���
        vcc1 = 1.225*4096/adc_value;
        adc_value2 = Adc_ReadState2();  //��ʱ�ĵ�ѹ�Ĵ���ֵ
        ZennerADC1 =vcc1 * adc_value2/4096 ;  //������ѹ
        IWDG_ReloadCounter();
        
        if(ZennerADC1>1.0)
        {
          Adc_DeInit(); 
          GPIOD->ODR &= ~0X04;
          GPIOD->ODR &= ~0X08;  
          PEN1_OFF;
          PEN3_OFF;
          SysdataReadBatteryEnd();//���ŵ�ѹ����
          return 0;
        }  
        Delay(40000);//ÿ����ʱ200ms   24*200ms=4800ms
      }
      Adc_DeInit(); 
      GPIOD->ODR &= ~0X04;
      GPIOD->ODR &= ~0X08;  
      PEN1_OFF;
      PEN3_OFF;
      SysdataReadBatteryEnd();//���ŵ�ѹ����
      return 1;
  }
  else if(ZennerADC1<1.8&&ZennerADC1>1.0)
  {
    //��ʱ0.8s
    Delay(60000);//1000ms
    Delay(60000);//1300ms
    Delay(40000);//1500ms    
    for(u8 i=1 ;i<=18;i++)
    {
       adc_value = Adc_ReadState1();//vcc��Ӧ�Ĵ���
        vcc1 = 1.225*4096/adc_value;
        adc_value2 = Adc_ReadState2();  //��ʱ�ĵ�ѹ�Ĵ���ֵ
        ZennerADC1 =vcc1 * adc_value2/4096 ;  //������ѹ
        IWDG_ReloadCounter();
      if(ZennerADC1>2.0)
      {
        Adc_DeInit(); 
        GPIOD->ODR &= ~0X04;
        GPIOD->ODR &= ~0X08;  
        PEN1_OFF;
        PEN3_OFF;
        SysdataReadBatteryEnd();//���ŵ�ѹ����
        return 0;
      }  
      Delay(40000);//ÿ����ʱ200ms   18*200ms=3600ms
    }
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//���ŵ�ѹ����
    return 1;  
  }  
  else //��һ�β���С��0.2v   �Ͽ�״ֱ̬���˳�
  {
          
    Adc_DeInit(); 
    GPIOD->ODR &= ~0X04;
    GPIOD->ODR &= ~0X08;  
    PEN1_OFF;
    PEN3_OFF;
    SysdataReadBatteryEnd();//���ŵ�ѹ����
    return 2;//�Ͽ�
  }
}*/
/*********�ͻ�����������Ӧ����end***************/

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
  //����־
 if(Zenner.UsartChangeFlag || ((Zenner.DLChangeFlag1 || Zenner.DLChangeFlag2 || Zenner.DLFlag1 || Zenner.DLFlag2 || ((Zenner.XLFlag1 || Zenner.XLFlag2) && Zenner.XLCloseEnableFlag) || Zenner.TimeFlag) && Zenner.ValveStatus == 0))
  {
    ZennerChangeStart();
    
    //���
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

u32 XL1_MSVtime;//��¼����msv�����ʱ��
u32 DL1_MSVtime;//��¼����msv�����ʱ��
void ZennerPulseLogic(void)
{
  XL1_MSVtime = 0;//���嵽����ʱ�����
  DL1_MSVtime = 0;//���嵽����ʱ�����
  //ǿ�ƹط�pulse��ʱ��λ
  Zenner.TimeQuanTime = 0;
  
  //�����Ա���
  if(Zenner.ValveStatus == 1)
  {
    if(Zenner.QMFlag == 0)
    {
      Zenner.QMFlag = 1;
      
      sysflag.send = 1;
      sysflag.FastSleep = 0;
    }
  }
  
  //��������-С©-�������1
  if(Zenner.XLTimesSet1)
  {
    if(Zenner.XLPulseCount1 < 255)
    {
      if(Zenner.XLStartFlag1 == 0) Zenner.XLStartFlag1 = 1;
      else Zenner.XLPulseCount1 ++;
    }
  }
  //��������-С©-�������2
  if(Zenner.XLTimesSet2)
  {
    if(Zenner.XLPulseCount2 < 255)
    {
      if(Zenner.XLStartFlag2 == 0) Zenner.XLStartFlag2 = 1;
      else Zenner.XLPulseCount2 ++;
    }
  }
  
  //������-��©-�������1
  if(Zenner.DLTimesSet1&&Zenner.DLSecSet1&&Zenner.DL_Pulse2)
  {
    if(Zenner.DLPulseCount1 < 255)
    {
      if(Zenner.DLStartFlag1 == 0) Zenner.DLStartFlag1 = 1;
      else Zenner.DLPulseCount1 ++;
    }
  }
  //������-��©-�������2
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
  //������-��©1
  /*if(Zenner.DLTimesSet1&&Zenner.DLSecSet1)
  {
    //������-��©��糬ʱ1
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
    //������-��©1
    if(Zenner.DLStartFlag1) Zenner.DLSecConut1 ++;
    else Zenner.DLSecConut1 = 0;
        /***********begin**************/
    if(DL1_MSVtime>Zenner.DLSecSet1)
    {
      //����ʱ�䣬��Ϊû�д���DL����
      Zenner.DLTimesCount1 = 0;//�����ڳɹ�����
      Zenner.DLStartFlag1 = 0;
      Zenner.DLPulseCount1 = 0;//�������������
      Zenner.DLSecConut1 = 0;
    }
    /***********end**************/
    if(Zenner.DLSecConut1 >= Zenner.DLSecSet1)//DL1��������
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
  //������-��©2
  if(Zenner.DLTimesSet2 && Zenner.DLSecSet2)
  {
    //������-��©��糬ʱ2
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
    //������-��©2
    if(Zenner.DLStartFlag2)
    {
      if(Zenner.DLSecConut2 < 255) Zenner.DLSecConut2 ++;
    }
    else Zenner.DLSecConut2 = 0;
  }
}

void ZennerXLLogicWork(void)
{
  //��������-С©1
  if(Zenner.XLSecSet1)//ʱ������ֵ
  {
    if(Zenner.XLStartFlag1) Zenner.XLSecConut1 ++;
    else 
    {
       Zenner.XLSecConut1 = 0;//ʱ����ʵ��ֵ���
    }
    
    //�ж�XL1_MSVtime�Ƿ񳬳�ʱ��
    if(XL1_MSVtime>=Zenner.XLSecSet1)
    {
      //����ʱ�䣬��Ϊû�д���XL����
      //Zenner.XLSecConut1 = 0;�̶���3Сʱ  ����Ҫ���
      Zenner.XLTimesCount1 = 0;
      Zenner.XLStartFlag1 = 0;
      Zenner.XLPulseCount1 = 0;  
    }
    
    if(Zenner.XLSecConut1 >= Zenner.XLSecSet1)//���ʱ���ж�
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
  
  //��������-С©2
  if(Zenner.XLSecSet2)
  {
    if(Zenner.XLStartFlag2) Zenner.XLSecConut2 ++;
    else Zenner.XLSecConut2 = 0;
    /***********begin**************/
    if(XL1_MSVtime>=Zenner.XLSecSet2)
    {
      //����ʱ�䣬��Ϊû�д���XL����
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
  //ǿ�ƹط�
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
  //ǿ�ƹط�����
  if(Zenner.TimeShengYuSec <= Zenner.TimeBaoJingSec && Zenner.TimeBaoJingSec) Zenner.TimeBaoJingFlag = 1;
}

void ZennerAlarmLogicWork(void)
{
  //���Ź���ѭ������
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
  //�����Թ���ѭ������
  if(Zenner.QMFlag)
  {
    Zenner.QMFlagSendTimeCount ++;
    if(Zenner.QMFlagSendTimeCount >= Zenner.QMFlagSendTimeSet && Zenner.QMFlagSendTimesCount < Zenner.QMFlagSendTimesSet && Zenner.QMFlagSendTimeSet && Zenner.QMFlagSendTimesSet)
    {
      Zenner.TimeFlagSendTimesCount = Zenner.TimeFlagSendTimesSet;//ȡ�����Ź���ѭ������
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
  //ǿ�ƹط�pulse��ʱ
  if(Zenner.TimeQuanTime < 10000) Zenner.TimeQuanTime ++;
  
  //������-��©-�߼�������
  ZennerDLLogicWork();
  
  //��������-С©-�߼�������
  ZennerXLLogicWork();
  
  //����-�߼�������
  ZennerTimeLogicWork();
  
  //ѭ������-�߼�������
  ZennerAlarmLogicWork();
}

void ZennerOneSecWork(void)
{
  ZennerLogicWork();            //�߼�����
  
  ZennerChangeWork();           //������
}

void ZennerCloseWork(void)
{
  //�ط���־
  if((Zenner.TimeFlag || Zenner.DLFlag1 || Zenner.DLFlag2 || ((Zenner.XLFlag1 || Zenner.XLFlag2) && Zenner.XLCloseEnableFlag)))
  {
    if(Zenner.ValveStatus == 0)         //������
    {      
      Zenner.ValveErrorStatus = ZennerValveDo(ZENNER_CLOSE);       //�ط�
      
      Zenner.ValveStatus = 1;
      
      sysflag.send = 1;
      sysflag.FastSleep = 0;
      //Zenner.TimeFlag=0;
    }
  }
}