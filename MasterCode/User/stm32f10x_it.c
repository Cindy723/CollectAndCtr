/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h" 
#include "Init.h" 
#include "Uart.h"   
#include "Tools.h"   
#include "oled.h"   

unsigned int g_LEDBling_kCount = 0;  
unsigned int g_RequestNodeCount = 0;
extern void TimingDelay_Decrement(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

void RTCAlarm_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Set the RTC alarm flag */ 

        /* Clear EXTI line17 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line17);

        /* Check if the Wake-Up flag is set */
        if (PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
        {
            /* Clear Wake Up flag */
            PWR_ClearFlag(PWR_FLAG_WU);
        }
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Clear RTC Alarm interrupt pending bit */
        RTC_ClearITPendingBit(RTC_IT_ALR);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}


/***********************************************************************************************************
 @ 功能： 10ms 一次定时器中断
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void TIM3_IRQHandler(void)
{    
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
	{
    	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		  g_LEDBling_kCount += 10;
		  g_RequestNodeCount += 10;
	} 
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

/*======================================================
    系统运行
======================================================*/
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) // 触发了中断
    { 
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}

/**********************************************************************************************************
 @ 功能: 连接log打印
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   
	 {
	 } 
}


/**********************************************************************************************************
 @ 功能: 连接串口屏
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void USART2_IRQHandler(void)
{
		// EE B1 11 00 01 00 6F 10 01 01 FF FC FF FF
		// EE B1 11 00 01 00 6F 10 01 00 FF FC FF FF
		static int endmark = 0;
	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   
	 {
			uart2TFTPack.dataOrig[uart2TFTPack.Counter] = USART_ReceiveData(USART2);   
			if(uart2TFTPack.Counter == 0 && (uart2TFTPack.dataOrig[0] !=0xEE))
			{ 
				return;  
			}else uart2TFTPack.Counter++;
 
			// 捕获帧尾第一个字节
			if((uart2TFTPack.dataOrig[uart2TFTPack.Counter-1] == 0xff) && uart2TFTPack.endflag == 0)
			{
			  endmark = uart2TFTPack.Counter;
				uart2TFTPack.endflag = 1;
			}
			
			// 到达帧尾长度
			if((uart2TFTPack.endflag == 1) && (uart2TFTPack.Counter - endmark == 3)) 
			{ 
				if((uart2TFTPack.dataOrig[endmark] == 0xfc) && 
					 (uart2TFTPack.dataOrig[endmark + 1] == 0xff)&&
					 (uart2TFTPack.dataOrig[endmark + 2] == 0xff))
				{ // 帧尾完全匹配 
					printf("ReciveTFT: \r\n");
					printHex(uart2TFTPack.dataOrig, uart2TFTPack.Counter);	
					str_copy_usart_buf2(uart2TFTPack.dataBuf,uart2TFTPack.Counter, uart2TFTPack.dataOrig); 					
					uart2TFTPack.packLen = uart2TFTPack.Counter; 
					uart2TFTPack.receiveok = 1;  
					USART_data_Reset(USART2);
				}
				else 
				{
					printf("ReciveTFT erro \r\n");
					uart2TFTPack.receiveok = 0;  
					USART_data_Reset(USART2); 
					endmark = 0;
				}
			}
			
//			if(uart2TFTPack.Counter >= 2)
//			{ 
//				switch(uart2TFTPack.dataOrig[1])
//				{
//					case BUTTON_CMD: // 按钮数据
//					{ 
//						if(uart2TFTPack.Counter == TFT_BUTTON_LEN) 
//						{
//							if((uart2TFTPack.dataOrig[TFT_BUTTON_LEN - 4] == 0xff) &&
//								 (uart2TFTPack.dataOrig[TFT_BUTTON_LEN - 3] == 0xfc) &&
//								 (uart2TFTPack.dataOrig[TFT_BUTTON_LEN - 2] == 0xff) &&
//								 (uart2TFTPack.dataOrig[TFT_BUTTON_LEN - 1] == 0xff))
//								{ 
//									printf("ReciveTFT: ");
//									printHex(uart2TFTPack.dataOrig, uart2TFTPack.Counter);
//									str_copy_usart_buf2(uart2TFTPack.dataBuf,uart2TFTPack.Counter, uart2TFTPack.dataOrig); 
//									uart2TFTPack.packLen = uart2TFTPack.Counter; 
//									uart2TFTPack.receiveok = 1; 
//								}
//									
//								USART_data_Reset(USART2);
//								return;
//						}
//						
//					}break;
//					case 0xcd: // 收到用户自定义的数据
//					{
//						
//					}
//					break;
//					case 0xb2:
//						break;
//					default: break;
//				} 
//			}
			
			if(uart2TFTPack.Counter >= 120)
			{ 
					printf("data overleng\r\n");
					USART_data_Reset(USART2);
			}
			
		}
}

/**********************************************************************************************************
 @ 功能: 连接485节点
 @ 参数: 
 @ 返回: 
 @ 备注: 接收节点回复的数据
 *********************************************************************************************************/
void USART3_IRQHandler(void)
{
	 static u8 dispFlag = 0;
	 if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   
	 { 
			uart3_485Pack.busy = 1;
			uart3_485Pack.dataOrig[uart3_485Pack.Counter] = USART_ReceiveData(USART3);     
			if(uart3_485Pack.Counter == 0 && uart3_485Pack.dataOrig[0] != 0xA5) 
			{
				USART_data_Reset(USART3); 
				return;   
			}
			else  uart3_485Pack.Counter++;

			if(uart3_485Pack.Counter > 1 && uart3_485Pack.dataOrig[1] != 0x5A)
			{
				printf("--Uart3_ISR data err \r\n");
		    OLED_P6x8Str(0,1,(unsigned char*)"data err   ",0); 
				displayHex2oled(uart3_485Pack.dataOrig, 2, 0, 2);
				USART_data_Reset(USART3);
				return;
			}
			
			// 主节点每次只会向一个节点请求数据，收到的数据将不区分地址
			if(uart3_485Pack.Counter >= 9) //收到数据长度位
			{
				if(uart3_485Pack.Counter == (uart3_485Pack.dataOrig[8] + 9))
				{
					str_copy_usart_buf(uart3_485Pack.dataBuf, uart3_485Pack.Counter, uart3_485Pack.dataOrig);	
					printf("Master Recive: ");
					switch(dispFlag++ %3)
					{
						case 0: 
							OLED_CLS();
							OLED_P6x8Str(1,0,(unsigned char*)"Recive:   ",0); 
						  displayHex2oled(uart3_485Pack.dataOrig, uart3_485Pack.Counter, 1, 1);
						 break; 
						case 1: displayHex2oled(uart3_485Pack.dataOrig, uart3_485Pack.Counter, 1, 3);
						 break; 
						case 2: displayHex2oled(uart3_485Pack.dataOrig, uart3_485Pack.Counter, 1, 5);
						 break;
					} 
					printHex(uart3_485Pack.dataBuf, uart3_485Pack.Counter); 
					uart3_485Pack.packLen = uart3_485Pack.Counter;
					USART_data_Reset(USART3);
					uart3_485Pack.receiveok = 1; 
				}
			}
			
			if(uart3_485Pack.Counter >= 120) // 没有这么长的数据 强行限制
			{
				printf("Data overLenth\n");
			  OLED_P6x8Str(0,1,(unsigned char*)"overLenth   ",0); 
				USART_data_Reset(USART3);
			}
			uart3_485Pack.busy = 0;  
	 }
}

/****************************************************************************
* 名    称：void EXTI0_IRQHandler(void)
* 功    能：EXTI0中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：蓝牙连接转态引脚，中断唤醒
* 调用方法：无 
****************************************************************************/
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
