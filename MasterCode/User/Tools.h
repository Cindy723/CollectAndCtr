#ifndef _TOOLS_H
#define _TOOLS_H
 
#include "main.h"   
#include "init.h"   
   
   
void ctrl485Dir(u8 dir);

void uint32_Str(uint32_t *id, char* desbuf, uchar uint32len);
void getChipIdStr(char *pbuf); 
void IO_TXD(u8 Data); 
void IO_USART_Send(u8 *buf, u8 len);
uchar CheckSum(uchar* buf, uchar len);
void printHex(const unsigned char* data, size_t length);
void displayHex2oled(const unsigned char* data, u8 length, u8 x, u8 y);
u8 hexCompaer(u8* desp, u8* srcp, u8 len);
int StrToHexByte(unsigned char *str, unsigned char *hex);
   
#endif
