#ifndef __RECORDER_H_
#define __RECORDER_H_


extern unsigned char RecorderDays;

extern volatile unsigned char data_pllrange;
extern volatile unsigned char data_pllrange1;

void RecorderInit(void);
void RecorderClear(void);
void RecorderSaveOne(void);
void RecorderPrint(void);
void RecorderPrintMore(void);


#endif


