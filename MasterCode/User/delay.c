/**
 * @file delay.c
 * @brief delay configuration function  
 * @author T
 * @version 1.0
 * @date 2018-11-11
 */
#include "delay.h"

static volatile u32 TimingDelay;

void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemCoreClock / 100000))
	{
		/* Capture error */
		while (1)
			;
	}
}

/**
  * @brief   us延时程序,10us为一个单位
  * @param  
  *		@arg t: Delay_us( 10 ) 则实现的延时为 10 * 10us = 100us
  * @retval  无
  */
void delay_10us(unsigned int t)
{
	TimingDelay = t;

	while (TimingDelay != 0)
		;
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

/**
  * @brief  us级，占用CPU的延时
  * @param  
  *     @arg t:延迟的时间，单位us
  * @retval 无
  */
void delay_us(unsigned int t)
{
	int i;
	for (i = 0; i < t; i++)
	{
		int a = 9;
		while (a--)
			;
	}
}

void get_ms(unsigned long *time)
{
}

/****************************************************************************
* 名    称：delay_ms(u16 ms)
* 功    能：毫秒延时函数
* 入口参数：u16 nms
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void delay_ms(u16 ms)
{
	u32 temp;
	//SysTick会以CPU主频的1/8频率运行36M/8=4.5M
	//SysTick会以CPU主频的1/8频率运行72M/8=9M
	SysTick->LOAD = 9000 * ms;
	SysTick->VAL = 0X00;  //清空计数器
	SysTick->CTRL = 0X01; //使能，减到零是无动作，采用外部时钟源
	do
	{
		temp = SysTick->CTRL;						  //读取当前倒计数值
	} while ((temp & 0x01) && (!(temp & (1 << 16)))); //等待时间到达
	SysTick->CTRL = 0x00;							  //关闭计数器
	SysTick->VAL = 0X00;							  //清空计数器
}
/**
 * @brief  us级，占用CPU的延时
 * @param  
 *     @arg t:延迟的时间，单位us
 * @retval 无
 */
void delay(unsigned int t)
{
	int i;
	for (i = 0; i < t; i++)
    {
		int a = 9;
		while (a--)
		;
	}
}
