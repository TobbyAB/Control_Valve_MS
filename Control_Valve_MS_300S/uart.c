

/*******************************************************************************/
#include "stm8l15x.h"

#include "uart.h"
#include "stdio.h"
#include "sysdata.h"

#include "recorder.h"

SUART_STRUCT SUART;

int fputc(int ch, FILE *f)
{
  USART_SendData8(USART1, (unsigned char) ch);// USART1 可以换成 USART2 等
  while (!(USART1->SR & USART_FLAG_TXE));
  return (ch);
}

void UartInit(void)
{
  SUART.buf_len = 0;
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Slow);//UART-TX
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//开启USART时钟
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);//端口重映射，去掉注释之后USART1为PA2-TX、PA3-RX；注释之后USART1为TX-PC5、RX-PC6；复位之后USART会自动恢复至PC5、PC6
  
  USART_Init(USART1,600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));//设置USART参数600，8N1，接收/发送
  
  USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//使能接收中断
  USART_Cmd (USART1,ENABLE);//使能USART
  
  USART_ClearITPendingBit (USART1,USART_IT_RXNE);//清中断标志
  USART_ReceiveData8 (USART1);
  
  ZennerUsartChangeStart();
  
  //SUART.AIFlag = 0;
  //Zenner.QMFlag = 0;
  SUART.AIFlag = 1;
  //Zenner.QMFlag = 0;
  
  enableInterrupts(); 
}

void UartDeInit(void)
{
  //USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//~使能接收中断
  USART_Cmd (USART1,DISABLE);//~使能USART
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,DISABLE);//~开启USART时钟
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,DISABLE);
  
  //GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_IT);//初始化按键，上拉输入，带中断             //串口高电平
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_IT);//初始化按键，上拉输入，带中断           //串口低电平
  
  GPIOA->ODR &= ~GPIO_Pin_2;
  
  ZennerUsartChangeStop();
  
  //sysflag.send = 1;
  //Zenner.QMFlag = 0;
}

void UartSendStr(unsigned char * buf,unsigned char len)
{
  for(unsigned char i = 0 ; i < len ; i ++)
  {
    USART_SendData8 (USART1,buf[i]);
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
  }
}

/////////////////////////////////////////////////////

/*unsigned cahr UartTable[][] = 
{
  {0x40 ,0x41 ,0x42 ,0x38 ,0x33 ,0x2A ,0x0D},
  {0x40 ,0x41 ,0x46 ,0x38 ,0x37 ,0x2A ,0x0D},
  {0x40 ,0x41 ,0x48 ,0x38 ,0x39 ,0x2A ,0x0D},
  {0x40 ,0x41 ,0x49 ,0x38 ,0x41 ,0x2A ,0x0D},
  {0x40 ,0x41 ,0x44 ,0x38 ,0x35 ,0x2A ,0x0D},
  {0x40 ,0x41 ,0x4A ,0x38 ,0x42 ,0x2A ,0x0D},
};*/

uint16_t UartGetCheck(unsigned char * buf , unsigned char len)
{
  uint16_t Chack = 0;
  unsigned char Sum = 0;
  unsigned char str = 0;
  unsigned char i;
  for(i = 0 ; i < len ; i ++) Sum += buf[i];  
  
  str = Sum >> 4;
  if(str < 10) str += '0';
  else str += 'A' - 10;
  Chack = str << 8;
  
  str = Sum & 0xf;
  if(str < 10) str += '0';
  else str += 'A' - 10;
  Chack |= str;
  
  return Chack;
}

uint16_t UartWriteCheck(unsigned char * buf , unsigned char len)  //校验和结束符
{
  uint16_t Chack = UartGetCheck(buf + 1,len - 1);
  buf[len] = Chack >> 8;
  buf[len + 1] = Chack & 0xff;
  buf[len + 2] = 0x2a;
  buf[len + 3] = 0x0d;
  
  return Chack;
}

unsigned char UartCheckOk(unsigned char * buf , unsigned char len)
{
  uint16_t Chack = UartGetCheck(buf + 1,len - 1 - 4);
  
  if((Chack >> 8) == buf[len - 4])
  {
    if((Chack & 0xff) == buf[len - 3])
    {
      return 1;
    }
  }
  
  return 0;
}

unsigned char UartGetBCD(unsigned char * buf,uint32_t data,unsigned char len)
{
  unsigned char temp;
  for(signed char i = len - 1 ; i >= 0 ; i--)
  {
    temp = data & 0xf;
    if(temp < 10) buf[i] = temp + '0';
    else buf[i] = temp - 10 + 'A';
    data >>= 4;
  }
  return 1;
}

uint32_t UartGetHEX(unsigned char * buf,uint32_t * data,unsigned char len)
{
  unsigned char temp;
  uint32_t hex = 0;
  for(unsigned char i = 0 ; i < len ; i ++)
  {
    hex <<= 4;
    temp = buf[i];
    if(temp < 'A') hex += temp - '0';
    else hex += temp - 'A' + 10;
  }
  *data = hex;
  return hex;
}

uint32_t Uart_lData;
unsigned char Uart_VErrorFlag;
unsigned char Uart_RFlag;
unsigned char Uart_ret;
unsigned char UartWork(void)
{
  
  if((GPIOA->IDR & 0x08) == 0) SUART.time ++;
  else SUART.time = 0;
  
  if(SUART.start == 0)
  {
    SUART.start = 1;
    UartInit();
  }
  
  if(SUART.buf_len > 2)
  { 
    /****************串口命令：连续发送********************
    if(SUART.buf[0]=='@'&&SUART.buf[1]=='S'&&SUART.buf[2]=='A')  //连续发射调试
    {
      MsSend2Min();
      UartSendStr("FS OK\n",6);  //调试
      SUART.buf_len = 0;
    }
    else if(SUART.buf[0]=='@'&&SUART.buf[1]=='S'&&SUART.buf[2]=='B')  //关阀调试
    {
      ZennerValveDo(ZENNER_CLOSE);
      char buf[3];
      buf[0]=MS.LBattery+48;
      buf[1]=sysdata.PVDLBatteryFlag2+48;
      UartSendStr("GF OK\n",6);  //调试
      UartSendStr(buf,2);  //调试
      SUART.buf_len = 0;
    }
 /////////////////////////////////////////*/
    if( SUART.buf[2] >= 'A' && SUART.buf[2] <= 'Z' )
    {
      if(SUART.buf_len == 7 && SUART.buf[5] == 0x2a&& SUART.buf[6] == 0x0d )//
      {
        SUART.buf_len = 0;
        SUART.time = 0;
        if(UartCheckOk(SUART.buf,7))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          switch(SUART.buf[2])
          {
            case 'B':
              UartGetBCD(&SUART.send_buf[SUART.send_i],MS.MSID,7);
              SUART.send_i += 7;
              break;
            case 'F':
              UartGetBCD(&SUART.send_buf[SUART.send_i],MS.MSV,5);
              SUART.send_i += 5;
              break;
            case 'H':
              UartGetBCD(&SUART.send_buf[SUART.send_i],ReadRTCTime() * 2,5);
              SUART.send_i += 5;
              break;
            case 'I':
              UartGetBCD(&SUART.send_buf[SUART.send_i],(MS.Versions << 6) + (MS.Tamper << 5) + (MS.Battery << 4) + (MS.LBattery),2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],((!!(Zenner.ValveStatus)) << 7) + ((Zenner.DLFlag1 | Zenner.DLFlag2) << 6)  + ((Zenner.XLFlag1 | Zenner.XLFlag2) << 5) + ((Zenner.TimeFlag|Zenner.TimeBaoJingFlag) << 4) + (Zenner.QMFlag << 2) + ((Zenner.ValveErrorStatus == 1) << 1) + (Zenner.ValveErrorStatus == 2 || Zenner.ValveCheckErrorStatus == 2),2);
              SUART.send_i += 2;            
              SysdataUpdateTamper();             
              break;
            case 'J':
              UartGetBCD(&SUART.send_buf[SUART.send_i],0,2);
              SUART.send_i += 2;
              break;
            case 'Q':           //读取escape参数
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DLSecSet2,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DLTimesSet2,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DL_Pulse,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DLTimesSet1,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DL_Pulse2,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.XLSecSet1,6);
              SUART.send_i += 6;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.XLTimesSet1,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.XLSecSet2,6);
              SUART.send_i += 6;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.XLTimesSet2,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.XLCloseEnableFlag,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.MeterFlag,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.DLSecSet1,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeFlagSendTimeSet,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeFlagSendTimesSet,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.QMFlagSendTimeSet,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.QMFlagSendTimesSet,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.Reserved1,2);
              SUART.send_i += 2;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.Reserved2,2);
              SUART.send_i += 2;
              break;
            case 'T':           //读取阀门状态

              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeFlag | Zenner.TimeBaoJingFlag,1);
              SUART.send_i += 1;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeShengYuSec,8);
              SUART.send_i += 8;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeBaoJingSec,8);
              SUART.send_i += 8;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.ValveData1,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.ValveData2,4);
              SUART.send_i += 4;
              
              break;
            case 'X':
              
              RecorderPrint();
              break;
            case 'Y':
              
              RecorderClear();
              break;
            case 'Z':
              
              RecorderPrintMore();
              break;
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              break;
          }
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          IWDG_ReloadCounter();               //喂狗
          UartSendStr(SUART.send_buf,SUART.send_i);
        }
      }
      else if(SUART.buf_len == 8 && SUART.buf[6] == 0x2a && SUART.buf[7] == 0x0d)//
      {
        SUART.buf_len = 0;
        SUART.time = 0;
        if(UartCheckOk(SUART.buf,8))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          switch(SUART.buf[2])
          {
            case 'W'://send
              
              MSSendWithSave();
              
              return 0;
              
              break;
              
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              
              break;
          }
        }
      }
      
      else if((SUART.buf_len == 12&& SUART.buf[10] == 0x2a&& SUART.buf[11] == 0x0d)||(SUART.buf_len == 11&& SUART.buf[10] == 0x2a)) //
      {
        SUART.buf_len = 0;
        SUART.time = 0;
        if(UartCheckOk(SUART.buf,12))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          switch(SUART.buf[2])
          {
            case 'A'://msid-6
              UartGetHEX(&(SUART.buf[3]),&(MS.MSID),5);
              if(sysdata.SendMode == 0) WriteRTCAlarm(MS.MSID % 1024 * 84);             
              MSDataSave();
              break;
            case 'E'://msv
              UartGetHEX(&(SUART.buf[3]),&(MS.MSV),5);             
              MSDataSave();
              break;
            case 'G'://mst
              UartGetHEX(&(SUART.buf[3]),&(MS.MST),5);
              WriteRTCTime(MS.MST / 2); 
              MSDataSave();
              break;             
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              break;
          }
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          UartSendStr(SUART.send_buf,SUART.send_i);
        }
      }          
      //else if(SUART.buf[12] == 0x2a )//&& SUART.buf[13] == 0x0d
      else if((SUART.buf_len == 14 && SUART.buf[12] == 0x2a&& SUART.buf[13] == 0x0d)||(SUART.buf_len == 13 && SUART.buf[12] == 0x2a))//
      {
        SUART.buf_len = 0;
        SUART.time = 0;
        if(UartCheckOk(SUART.buf,14))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          switch(SUART.buf[2])
          {
            case 'A'://msid-8
              UartGetHEX(&(SUART.buf[3]),&(MS.MSID),7);
              if(sysdata.SendMode == 0) WriteRTCAlarm(MS.MSID % 1024 * 84);
              MSDataSave();
              break;
             case 'E'://msv
              UartGetHEX(&(SUART.buf[3]),&(MS.MSV),7);             
              MSDataSave();
              break;
            case 'G'://mst
              UartGetHEX(&(SUART.buf[3]),&(MS.MST),7);
              WriteRTCTime(MS.MST / 2); 
              MSDataSave();
              break;             
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              break;
          }
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          UartSendStr(SUART.send_buf,SUART.send_i);
        }
      }

      
      else if(SUART.buf_len == 32 && SUART.buf[30] == 0x2a )//&& SUART.buf[31] == 0x0d
      {
        SUART.buf_len = 0;
        SUART.time = 0;
        if(UartCheckOk(SUART.buf,32))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          
          switch(SUART.buf[2])
          {
            case 'R'://写阀状态
              
              UartGetHEX(&(SUART.buf[4]),&(Uart_lData),8);
              if(Uart_lData != 0xffffffff)
              {
                Uart_VErrorFlag = 3;
                if((Uart_lData >= 60 && Uart_lData <= 311040000) || Uart_lData == 0) Zenner.TimeShengYuSec = Uart_lData;//min:60s  max：120月
                else Zenner.TimeShengYuSec = 194400000;
                Zenner.TimeFlag = 0;
              }
              
              UartGetHEX(&(SUART.buf[12]),&(Uart_lData),8);
              if(Uart_lData != 0xffffffff)
              {
                Uart_VErrorFlag = 3;
                if((Uart_lData >= 60 && Uart_lData <= 103680000) || Uart_lData == 0) Zenner.TimeBaoJingSec = Uart_lData;//min:60s   max：40月
                else Zenner.TimeBaoJingSec = 15552000;
              }
              UartGetHEX(&(SUART.buf[20]),&(Uart_lData),4);
              if(Uart_lData != 0xffff) Zenner.ValveData1 = Uart_lData;
              UartGetHEX(&(SUART.buf[24]),&(Uart_lData),4);
              if(Uart_lData != 0xffff) Zenner.ValveData2 = Uart_lData;
              
              
              UartGetHEX(&(SUART.buf[3]),&(Uart_lData),1);
              if(Uart_lData != 0xf)
              {
                if(Uart_lData == 0)//open
                {
                  Uart_ret = ZennerValveDoWhile(ZENNER_OPEN);
                  Zenner.ValveStatus = 0;
                  Uart_RFlag = 1;
                  
                  if(Uart_VErrorFlag == 3 && Uart_ret != 2) Uart_VErrorFlag = 0;//2-断开
                  else if(Uart_VErrorFlag == 1 && Uart_ret == 1) Uart_VErrorFlag = 0;
                  else Uart_VErrorFlag = Uart_ret;
                  
                  SUART.AIFlag = 0;
                  Zenner.QMFlag = 0;
                }
                else if(Uart_lData == 1)//close
                {
                  Uart_RFlag = 0;
                  Uart_ret = ZennerValveDoWhile(ZENNER_CLOSE);
                  Zenner.ValveStatus = 1;
                  
                  //if(Uart_VErrorFlag == 3 && Uart_ret != 2) Uart_VErrorFlag == 0;//不是断开  就正常
                  if(Uart_VErrorFlag == 1 && Uart_ret == 1) Uart_VErrorFlag = 0;
                  else Uart_VErrorFlag = Uart_ret;
                }
              }
              if(Uart_VErrorFlag == 3) Uart_VErrorFlag = 0;
              /***********阀门状态判断更改begin********          
              UartGetHEX(&(SUART.buf[3]),&(Uart_lData),1);
              if(Uart_lData != 0xf)
              {
                if(Uart_lData == 0)//open
                {
                  Uart_ret = ZennerValveDo(ZENNER_OPEN);
                  Zenner.ValveStatus = 0;
                  Uart_RFlag = 1;
                  
                  if(Uart_ret==1)
                    Uart_ret=0;

                  SUART.AIFlag = 0;
                  Zenner.QMFlag = 0;
                }
                else if(Uart_lData == 1)//close
                {
                  Uart_RFlag = 0;
                  Uart_ret = ZennerValveDo(ZENNER_CLOSE);
                  Zenner.ValveStatus = 1;  
                }
                 switch(Uart_ret) 
                 {
                     case 0:break;
                     case 1:Uart_ret=2;break;
                     case 2:Uart_ret=1;break;
                 }
              }
              if(Uart_VErrorFlag == 3) Uart_VErrorFlag = 0;
              /***********阀门状态判断更改end********/
            

              if(Uart_RFlag == 1)
              {
                ZennerWorkDataInit();                
                if(sysflag.CloseFlag != 2) sysflag.TimeFlag = 0;
                else sysflag.CloseFlag = 0;                
                sysflag.DLFlag = 0;
                sysflag.XLFlag = 0;
                sysflag.MFFlag = 0;
                sysflag.MFFirstFlag = 0;
                sysflag.ZlErrorFlag = 0;                
                sysdata.cDL_Times = 0;
                sysdata.cQuanTime = 0;
                sysdata.cMiFeng = 0;               
                sysdata.TXL_Sec = 0;
                sysdata.cXL_StartFlag = 0;
                sysdata.cXL_Times = 0;
                sysdata.cJiQuan = 0;
                sysdata.TXL_Sec2 = 0;
                sysdata.cXL_StartFlag2 = 0;
                sysdata.cXL_Times2 = 0;
                sysdata.cJiQuan2 = 0;            
                sysdata.DLCloseFlag = 0;
                sysdata.XLCloseFlag = 0;
                for(sysdata.cQuanTimeBufI = 0 ; sysdata.cQuanTimeBufI < 10 ; sysdata.cQuanTimeBufI ++) sysdata.cQuanTimeBuf[sysdata.cQuanTimeBufI] = 255;
              }
              ZennerDataSave();
              
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.ValveStatus,1);
              SUART.send_i += 1;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeShengYuSec,8);
              SUART.send_i += 8;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.TimeBaoJingSec,8);
              SUART.send_i += 8;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.ValveData1,4);
              SUART.send_i += 4;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Zenner.ValveData2,4);
              SUART.send_i += 4;
              
              if(Uart_VErrorFlag == 1) Uart_ret = 2;
              else if(Uart_VErrorFlag == 2) Uart_ret = 1;
              else Uart_ret = 0;
              UartGetBCD(&SUART.send_buf[SUART.send_i],Uart_ret,1);
              SUART.send_i += 1;
              
              
              
              break;
              
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              
              break;
          }
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          IWDG_ReloadCounter();               //喂狗
          UartSendStr("ghghghghghghghghghgh",20);
          UartSendStr(SUART.send_buf,SUART.send_i);
        }
      }
     
      //else if((SUART.buf_len == 60+4) && (SUART.buf[59+4] == 0x2a))
      else if(((SUART.buf_len == 60+4) && (SUART.buf[59+4] == 0x2a)) || ((SUART.buf_len == 60) && (SUART.buf[59] == 0x2a)))
      {
        if(SUART.buf_len == 64) SUART.buf_len = 65;
        else SUART.buf_len = 61;
        
        if(UartCheckOk(SUART.buf,SUART.buf_len))
        {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = SUART.buf[1];
          SUART.send_buf[SUART.send_i ++] = SUART.buf[2];
          SUART.send_buf[SUART.send_i ++] = 0x30;
          switch(SUART.buf[2])
          {
            case 'P'://写escape参数
              
              UartGetHEX(&(SUART.buf[3]),&(Uart_lData),2);
              if(Uart_lData >= 6 && Uart_lData <= 17|| Uart_lData == 0) Zenner.DLSecSet2 = Uart_lData;
              else Zenner.DLSecSet2 = 10;
              
              UartGetHEX(&(SUART.buf[5]),&(Uart_lData),4);
              if(Uart_lData >= 3 && Uart_lData <= 10 || Uart_lData == 0) Zenner.DLTimesSet2 = Uart_lData;
              else Zenner.DLTimesSet2 = 6;
              
              UartGetHEX(&(SUART.buf[9]),&(Uart_lData),4);
              if(Uart_lData==0) Zenner.DL_Pulse = Uart_lData;
              else
                Zenner.DL_Pulse = 1;
              
              UartGetHEX(&(SUART.buf[13]),&(Uart_lData),4);
              if(Uart_lData >= 3 && Uart_lData <= 255 || Uart_lData == 0) Zenner.DLTimesSet1 = Uart_lData;
              else Zenner.DLTimesSet1 = 190;
              
              UartGetHEX(&(SUART.buf[17]),&(Uart_lData),4);
              if(Uart_lData)
                Zenner.DL_Pulse2 = 1;
              else
                Zenner.DL_Pulse2 = 0;
              
              
              UartGetHEX(&(SUART.buf[21]),&(Uart_lData),6);
              if(Uart_lData >= 3600 && Uart_lData <= 360000 || Uart_lData == 0) Zenner.XLSecSet1 = Uart_lData;
              else Zenner.XLSecSet1 = 3600;
              
              UartGetHEX(&(SUART.buf[27]),&(Uart_lData),2);
              if(Uart_lData >= 2 && Uart_lData <= 200 || Uart_lData == 0) Zenner.XLTimesSet1 = Uart_lData;
              else Zenner.XLTimesSet1 = 24;
              
              UartGetHEX(&(SUART.buf[29]),&(Uart_lData),6);
              if(Uart_lData >= 3600 && Uart_lData <= 360000 || Uart_lData == 0) Zenner.XLSecSet2 = Uart_lData;
              else Zenner.XLSecSet2 = 10800;
              
              UartGetHEX(&(SUART.buf[35]),&(Uart_lData),2);
              if(Uart_lData >= 2 && Uart_lData <= 200 || Uart_lData == 0) Zenner.XLTimesSet2 = Uart_lData;
              else Zenner.XLTimesSet2 = 24;
              
              UartGetHEX(&(SUART.buf[37]),&(Uart_lData),2);
              if(Uart_lData <= 1) Zenner.XLCloseEnableFlag = Uart_lData;
              else Zenner.XLCloseEnableFlag = 1;
              
              UartGetHEX(&(SUART.buf[39]),&(Uart_lData),4);
              Zenner.MeterFlag = Uart_lData;
              
              UartGetHEX(&(SUART.buf[43]),&(Uart_lData),2);
              if(Uart_lData >= 6 && Uart_lData <= 55|| Uart_lData == 0) Zenner.DLSecSet1 = Uart_lData;
              else Zenner.DLSecSet1 = 22;
              
              UartGetHEX(&(SUART.buf[45]),&(Uart_lData),4);
              if(Uart_lData >= 60 && Uart_lData <= 90000||Uart_lData==0)
              Zenner.TimeFlagSendTimeSet = Uart_lData;
              else Zenner.TimeFlagSendTimeSet=3600;
              
              UartGetHEX(&(SUART.buf[49]),&(Uart_lData),2);
              Zenner.TimeFlagSendTimesSet = Uart_lData;
              
              UartGetHEX(&(SUART.buf[51]),&(Uart_lData),4);
              if(Uart_lData >= 60 && Uart_lData <= 90000||Uart_lData==0)
              Zenner.QMFlagSendTimeSet = Uart_lData;
              else Zenner.QMFlagSendTimeSet = 3600;
              
              UartGetHEX(&(SUART.buf[55]),&(Uart_lData),2);
              Zenner.QMFlagSendTimesSet = Uart_lData;
              
              if(SUART.buf_len == 65)
              {
                UartGetHEX(&(SUART.buf[57]),&(Uart_lData),2);
                Zenner.Reserved1 = Uart_lData;
                
                UartGetHEX(&(SUART.buf[59]),&(Uart_lData),2);
                Zenner.Reserved2 = Uart_lData;
              }
              
              SUART.AIFlag = 0;
              
              ZennerWorkDataInit();
              
              
              ZennerDataSave();
              
              
              break;
              
            default:
              SUART.send_buf[SUART.send_i - 1] = 0x31;
              
              break;
          }
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          IWDG_ReloadCounter();               //喂狗
          UartSendStr(SUART.send_buf,SUART.send_i);
          
          for(;SUART.send_i > 0 ; SUART.send_i --) SUART.send_buf[SUART.send_i] = 0;
        }
        SUART.buf_len = 0;
        SUART.time = 0;
      }
    }
    else SUART.buf_len = 0;
  }
  

  /*
  if(Zenner.QMFlag && SUART.AIFlag == 0)
  {
          SUART.send_i = 0;
          SUART.send_buf[SUART.send_i ++] = 0x40;
          SUART.send_buf[SUART.send_i ++] = 'A';
          SUART.send_buf[SUART.send_i ++] = 'I';
          SUART.send_buf[SUART.send_i ++] = 0x30;
          UartGetBCD(&SUART.send_buf[SUART.send_i],(MS.Versions << 6) + (MS.Tamper << 5) + (MS.Battery << 4) + (MS.LBattery),2);
          SUART.send_i += 2;
          UartGetBCD(&SUART.send_buf[SUART.send_i],((!!(Zenner.ValveStatus)) << 7) + ((Zenner.DLFlag1 | Zenner.DLFlag2) << 6)  + ((Zenner.XLFlag1 | Zenner.XLFlag2) << 5) + (Zenner.TimeFlag << 4) + (Zenner.QMFlag << 2) + ((Zenner.ValveErrorStatus == 1) << 1) + (Zenner.ValveErrorStatus == 2 || Zenner.ValveCheckErrorStatus == 2),2);
          SUART.send_i += 2;
          UartWriteCheck(SUART.send_buf,SUART.send_i);
          SUART.send_i += 4;
          UartSendStr(SUART.send_buf,SUART.send_i);
          for(;SUART.send_i > 0 ; SUART.send_i --) SUART.send_buf[SUART.send_i] = 0;
          
    SUART.AIFlag = 1;
    Zenner.QMFlag = 0;
  }*/
  
  if(SUART.time == 200)
  {
    SUART.time = 0;
    SUART.start = 0;
    memset(SUART.send_buf,0,70);
    memset(SUART.buf,0,70);
    UartDeInit();   
    return 1;
  }  
  return 0;
}




