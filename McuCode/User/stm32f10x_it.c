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
#include "Tools.h"   
#include "Uart.h"   
 
unsigned int g_LEDBlinkCount = 0;  
 
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
	
	printf("HardFault_Handler!\r\n");  
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
	printf("MemManage_Handler!\r\n");
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
	printf("BusFault_Handler!\r\n");
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
	printf("UsageFault_Handler!\r\n");
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
  
}


void TIM2_IRQHandler(void)
{	
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	 
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);  		 
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
			++g_LEDBlinkCount;   
	} 
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
   

/* 串口1中断入口 连接上位机*/
// 帧结构 5A A5 0D FF FF 00 0A 02 00 81 33
// 0D FF FF 板类型+地址
// 00 0A  cmd
// 02 00 81 数据长度 数据类型 数据
// 注意： 由于打印是io模拟，所以不要在接收过程打印 否则数据错误！
void USART1_IRQHandler(void)
{
		  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   
     {
			  uart1Pack.busy = 1;
				uart1Pack.dataOrig[uart1Pack.Counter] = USART_ReceiveData(USART1);    
				if(uart1Pack.Counter == 0 && uart1Pack.dataOrig[0] != 0x5A) 
				{
					printf("throw\r\n");
					USART_data_Reset(USART1);
					return;   
				}
				else  uart1Pack.Counter++;
  
				if(uart1Pack.Counter > 1 && uart1Pack.dataOrig[1] != 0xA5)
				{
				  printf("--Uart1_ISR data err \r\n");
					USART_data_Reset(USART1);
					return;
				}
				
			  if(uart1Pack.Counter == 5) // 地址校验
				{ 
					if((uart1Pack.dataOrig[3] == 0xff) && (uart1Pack.dataOrig[4] == 0xff)) // 广播地址不区分板子类型
					{
					 // 收到的是广播数据 这里不要用io打印 将影响后续数据
						//printf("broadcast \r\n"); 
					}
					else if(0 != hexCompaer(&uart1Pack.dataOrig[2], boardAddr.addr , 3))// 不是我的数据
					{
						printf("This data does not belong to me. \r\ntarget: ");
						printHex(&uart1Pack.dataOrig[2], 3); 
						printf("myaddr: "); 
						printHex(&boardAddr.addr[0], 3); 
						USART_data_Reset(USART1);
						return;
					}
				}
				 
				if(uart1Pack.Counter >= 9) //收到数据长度位
				{
					if(uart1Pack.Counter == (uart1Pack.dataOrig[8] + 10))
					{
						str_copy_usart_buf(uart1Pack.dataBuf, uart1Pack.Counter, uart1Pack.dataOrig);	
						printf("mcuRecive: ");
						printHex(uart1Pack.dataBuf, uart1Pack.Counter); 
						uart1Pack.packLen = uart1Pack.Counter;
						USART_data_Reset(USART1);
						uart1Pack.receiveok = 1; 
					}
				}
				
				if(uart1Pack.Counter >= 120) // 没有这么长的数据 强行限制
				{
					printf("Data overLenth\n");
					USART_data_Reset(USART1);
				}
					uart1Pack.busy = 0; 
			}

}

/* 串口2连接电能模块 */
void USART2_IRQHandler(void)
{
#ifdef TESTHMI_DC
		// EE B1 11 00 01 00 6F 10 01 01 FF FC FF FF
		// EE B1 11 00 01 00 6F 10 01 00 FF FC FF FF
	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   
	 {
			uart2HMIPack.dataOrig[uart2HMIPack.Counter] = USART_ReceiveData(USART2);   
			if(uart2HMIPack.Counter == 0 && (uart2HMIPack.dataOrig[0] !=0xEE))
			{
				return;  
			}else uart2HMIPack.Counter++;
 
			if(uart2HMIPack.Counter >= 2)
			{ 
				switch(uart2HMIPack.dataOrig[1])
				{
					case BUTTON_CMD:
					{
						if(uart2HMIPack.Counter == HMI_BUTTON_LEN) 
						{
							if((uart2HMIPack.dataOrig[HMI_BUTTON_LEN - 4] == 0xff) &&
								 (uart2HMIPack.dataOrig[HMI_BUTTON_LEN - 3] == 0xfc) &&
								 (uart2HMIPack.dataOrig[HMI_BUTTON_LEN - 2] == 0xff) &&
								 (uart2HMIPack.dataOrig[HMI_BUTTON_LEN - 1] == 0xff))
								{ 
									printf("ReciveHMI: ");
									printHex(uart2HMIPack.dataOrig, uart2HMIPack.Counter);
									str_copy_usart_buf2(uart2HMIPack.dataBuf,uart2HMIPack.Counter, uart2HMIPack.dataOrig); 
									uart2HMIPack.packLen = uart2HMIPack.Counter; 
									uart2HMIPack.receiveok = 1; 
								}
									
								USART_data_Reset(USART2);
								return;
						}
						break;
					}
					case 0xb2:
						break;
					default: break;
				}
			}
 
		}
	  
#else
	static uint16_t inval = 0;
	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   
	 {
			uart2Pack.dataOrig[uart2Pack.Counter] = USART_ReceiveData(USART2);   

				//printf("c %d  b %x!\n", uart2Pack.Counter, uart2Pack.dataOrig[uart2Pack.Counter]); 
 				if(inval++ >= 2) // 间隔 2*50ms 次处理一次
				{
						if(uart2Pack.Counter == 0 && (uart2Pack.dataOrig[0] !=0x55))
						{  
							return;  
						}else uart2Pack.Counter++;

						
						if(uart2Pack.Counter == 2 && uart2Pack.dataOrig[1] != 0x5A) 
						{
							uart2Pack.Counter = 0;
							return;
						}

						if(uart2Pack.Counter >= 24)
						{
							str_copy_usart_buf2(uart2Pack.dataBuf,uart2Pack.Counter, uart2Pack.dataOrig); 
							//printHex(uart2Pack.dataBuf, uart2Pack.Counter);
							uart2Pack.packLen = uart2Pack.Counter;
							uart2Pack.Counter = 0;    
							uart2Pack.receiveok = 1;
						  inval = 0;
						}
				}
		}
	 
#endif
} 
  
