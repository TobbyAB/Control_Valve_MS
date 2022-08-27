
#include "stm8l15x.h"

//void Adc_Init(void)
//{
//  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
//}
//void Adc_DeInit(void)
//{
//  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);//ADC
//}
//
//u8 Adc_ReadState(void)
//{
//  if(GPIOD->IDR & 0x80) return 1;
//  else return 0;
//}

void Adc_DeInit(void)
{
  
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);//ADC
    
 ADC_VrefintCmd(DISABLE); //�ڲ��ο���ѹ
  
  ADC_Cmd(ADC1,DISABLE);//ADCʹ��
  
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);//����ADC1ʱ��
}


void Adc_Init(void)
{ 
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,ENABLE);//����ADC1ʱ��
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
  ADC_VrefintCmd(ENABLE); //ʹ���ڲ��ο���ѹ
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//����ת����12λ��ת��ʱ��1��Ƶ
  ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,ENABLE );
  ADC_Cmd(ADC1,ENABLE);//ADCʹ��
}

u16 Adc_ReadState1(void)
{
  int adc;
  //GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);//ADC
  ADC_Cmd(ADC1,DISABLE);//ADC

  ADC_VrefintCmd(ENABLE); //ʹ���ڲ��ο���ѹ
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//����ת����12λ��ת��ʱ��1��Ƶ
  ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,ENABLE);//ʹ���ڲ��ο���ѹͨ��
  ADC_Cmd(ADC1,ENABLE);//ADCʹ��
  
  ADC_SoftwareStartConv(ADC1); //�������ת��
  while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//�ȴ�ת������
  ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//�����Ӧ��־
  adc = ADC_GetConversionValue(ADC1); //��ȡת��ֵ
  ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,DISABLE);//��ѹͨ��  
  //ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,DISABLE);//�ڲ��ο���ѹͨ��
  //ADC_VrefintCmd(DISABLE); //�ڲ��ο���ѹ
  //ADC_Cmd(ADC1,DISABLE);//ADC����
  return adc;
}
u16 Adc_ReadState2(void)
{
  int adc;

  ADC_VrefintCmd(DISABLE); //�ڲ��ο���ѹ
  ADC_Cmd(ADC1,DISABLE);//ADC
  
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//����ת����12λ��ת��ʱ��1��Ƶ
  ADC_ChannelCmd(ADC1,ADC_Channel_7,ENABLE);//ʹ�ܵ�ѹͨ��
  ADC_Cmd(ADC1,ENABLE);//ADCʹ��

  
  ADC_SoftwareStartConv(ADC1); //�������ת��
  while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//�ȴ�ת������
  ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//�����Ӧ��־
  adc = ADC_GetConversionValue(ADC1); //��ȡת��ֵ
  ADC_ChannelCmd(ADC1,ADC_Channel_7,DISABLE);//�ڲ��ο���ѹͨ��
  
  return adc;
}