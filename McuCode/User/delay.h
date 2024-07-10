/**
 * @file delay.h
 * @brief  delay head file     
 * @author T
 * @version 1.0
 * @date 2018-11-11
 */

#ifndef __T_DELAY_H__
#define __T_DELAY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

	void SysTick_Init(void);
	void TimingDelay_Decrement(void);
	void delay_10us(unsigned int t); 

	//void delay_us(u32 n);
	//void delay_ms(u32 n);
	void get_ms(unsigned long *time);
	void delay_ms(u16 ms);
	void delay_us(u16 us);
  void delay(unsigned int t);

#endif
