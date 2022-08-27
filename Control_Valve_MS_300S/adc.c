
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
    
 ADC_VrefintCmd(DISABLE); //内部参考电压
  
  ADC_Cmd(ADC1,DISABLE);//ADC使能
  
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);//开启ADC1时钟
}


void Adc_Init(void)
{ 
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,ENABLE);//开启ADC1时钟
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
  ADC_VrefintCmd(ENABLE); //使能内部参考电压
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//连续转换，12位，转换时钟1分频
  ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,ENABLE );
  ADC_Cmd(ADC1,ENABLE);//ADC使能
}

u16 Adc_ReadState1(void)
{
  int adc;
  //GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);//ADC
  ADC_Cmd(ADC1,DISABLE);//ADC

  ADC_VrefintCmd(ENABLE); //使能内部参考电压
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//连续转换，12位，转换时钟1分频
  ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,ENABLE);//使能内部参考电压通道
  ADC_Cmd(ADC1,ENABLE);//ADC使能
  
  ADC_SoftwareStartConv(ADC1); //开启软件转换
  while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//等待转换结束
  ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//清除对应标志
  adc = ADC_GetConversionValue(ADC1); //获取转换值
  ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,DISABLE);//电压通道  
  //ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint,DISABLE);//内部参考电压通道
  //ADC_VrefintCmd(DISABLE); //内部参考电压
  //ADC_Cmd(ADC1,DISABLE);//ADC禁能
  return adc;
}
u16 Adc_ReadState2(void)
{
  int adc;

  ADC_VrefintCmd(DISABLE); //内部参考电压
  ADC_Cmd(ADC1,DISABLE);//ADC
  
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);//ADC
  ADC_Init(ADC1,ADC_ConversionMode_Continuous,ADC_Resolution_12Bit,ADC_Prescaler_1);//连续转换，12位，转换时钟1分频
  ADC_ChannelCmd(ADC1,ADC_Channel_7,ENABLE);//使能电压通道
  ADC_Cmd(ADC1,ENABLE);//ADC使能

  
  ADC_SoftwareStartConv(ADC1); //开启软件转换
  while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//等待转换结束
  ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//清除对应标志
  adc = ADC_GetConversionValue(ADC1); //获取转换值
  ADC_ChannelCmd(ADC1,ADC_Channel_7,DISABLE);//内部参考电压通道
  
  return adc;
}