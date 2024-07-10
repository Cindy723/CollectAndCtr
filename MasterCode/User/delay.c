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
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 100000))
	{
		/* Capture error */
		while (1)
			;
	}
}

/**
  * @brief   us��ʱ����,10usΪһ����λ
  * @param  
  *		@arg t: Delay_us( 10 ) ��ʵ�ֵ���ʱΪ 10 * 10us = 100us
  * @retval  ��
  */
void delay_10us(unsigned int t)
{
	TimingDelay = t;

	while (TimingDelay != 0)
		;
}

/**
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

/**
  * @brief  us����ռ��CPU����ʱ
  * @param  
  *     @arg t:�ӳٵ�ʱ�䣬��λus
  * @retval ��
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
* ��    �ƣ�delay_ms(u16 ms)
* ��    �ܣ�������ʱ����
* ��ڲ�����u16 nms
* ���ڲ�������
* ˵    ����
* ���÷�������
****************************************************************************/
void delay_ms(u16 ms)
{
	u32 temp;
	//SysTick����CPU��Ƶ��1/8Ƶ������36M/8=4.5M
	//SysTick����CPU��Ƶ��1/8Ƶ������72M/8=9M
	SysTick->LOAD = 9000 * ms;
	SysTick->VAL = 0X00;  //��ռ�����
	SysTick->CTRL = 0X01; //ʹ�ܣ����������޶����������ⲿʱ��Դ
	do
	{
		temp = SysTick->CTRL;						  //��ȡ��ǰ������ֵ
	} while ((temp & 0x01) && (!(temp & (1 << 16)))); //�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00;							  //�رռ�����
	SysTick->VAL = 0X00;							  //��ռ�����
}
/**
 * @brief  us����ռ��CPU����ʱ
 * @param  
 *     @arg t:�ӳٵ�ʱ�䣬��λus
 * @retval ��
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
