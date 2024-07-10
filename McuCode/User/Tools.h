#ifndef _TOOLS_H
#define _TOOLS_H
 
#include "main.h"   
#include "init.h"   
   
  
double EconAnalysis(u8 *pbuf); 
void ctrl485Dir(u8 dir);

void uint32_Str(uint32_t *id, char* desbuf, uchar uint32len);
void getChipIdStr(char *pbuf); 
void IO_TXD(u8 Data); 
void IO_USART_Send(u8 *buf, u8 len);
uchar CheckSum(uchar* buf, uchar len);
void printHex(const unsigned char* data, size_t length);
u8 hexCompaer(u8* desp, u8* srcp, u8 len);
 
extern u16 kCount;   
 
   
#endif
