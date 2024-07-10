#ifndef  _APPLICATION_H_
#define  _APPLICATION_H_


#include "main.h"    
#include "Tools.h"     
#include "stm32f10x_it.h"
#include "UART.h"
#include "OLED.h"

 
extern u16 g_kCount; 

void NodeDataAnalysis(void);
void updateBaseInfo(void);
u8 setBoardBaseInfo(const u8 *pconten);
void IntervalProc(void);  
double EconAnalysis(u8 *pbuf);
 
#endif
