#ifndef  _APPLICATION_H_
#define  _APPLICATION_H_


#include "main.h"    
#include "Tools.h"     
#include "stm32f10x_it.h"


void needOpreat(u8 sw, u8 ch);
void updateElectInfoAC(u8 needSend);
void updateElectInfoDC(u8 needSend);
void updateBaseInfo(void);
u8 setBoardBaseInfo(const u8 *pconten);
void IntervalProc(void);
void MasterDataAnalysis(void);
void rwTypeAndAddr(u8 rw, BroadAddr *baddr);
 
#endif
