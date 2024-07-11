#ifndef _Init_H
#define _Init_H

#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_gpio.h"
#include "main.h"  

// 板子程序版本
#define VERSIONFRIST 0
#define VERSIONLAST 11

//#define MINIBOARD 

// 使用串口2代替主节点测试TFT
//#define TESTTFT_DC 

// LOG串口 
#define DEBUGUSART USART1

// 信号定义
#define LEDON  1
#define LEDOFF 0

#define POWER_ON  1
#define POWER_OFF 0

#define POWERCH1 1
#define POWERCH2 2

#define RS485SEND 1
#define RS485RECE 0

#define OI_TXD	PBout(3)

// 接口定义 
#define LEDRUNPINCFG   		GPIO_Pin_12
#define LED2PINCFG 				GPIO_Pin_13
#define LED3PINCFG 				GPIO_Pin_14
#define LED4PINCFG 				GPIO_Pin_15

#define LEDRUNPIN     12
#define LED2PIN 			13
#define LED3PIN 		  14
#define LED4PIN 			15
 
// PA
#define RS485DIRPIN   8
#define RS485DIRPINCFG  GPIO_Pin_8  

// PB
#define RELAY1PIN 	  8
#define RELAY2PIN 	  9
 
#define RELAY1PINCFG 	  GPIO_Pin_8
#define RELAY2PINCFG 	  GPIO_Pin_9
 
// 继电器操作		 
#define RELAY1  PBout(RELAY1PIN)
#define RELAY2  PBout(RELAY2PIN)

// 485方向控制
#define CTRL485SEND PAout(RS485DIRPIN) = RS485SEND
#define CTRL485RECE PAout(RS485DIRPIN) = RS485RECE

// LED 操作
#define LEDContrl(n, sw) (PBout(n) = sw) 


 
// 串口外部命令功能定义
#define POWERCONTRL  0X0A  // 操作电源通断
#define REQUESTELEC  0X0B  // 主机请求电能数据 
#define REQUESTADDR  0X0C  // 主机请求节点地址
#define SETBOARDADDR 0X0D  // 主机修改节点地址,当节点没有存入地址时使用0地址下发，当已经有地址时需要使用原先地址发送. 或者使用广播地址
 
 
// AC电压校正值
#define ACADJVP 0.989803567
// AC电流校正值
#define ACADJAP 1.149905189
 
// 最大节点数量
#define MAX_NODE 255
  
// 节点基本信息  
typedef struct
{
	u8 addr[3];
	u8 addrStr[6]; // dc 11 33 -->> "dc1133"
	u8 *type;
}BoardAddr; 
 
// 对应id的电力信息
typedef struct 
{ 
	float vTotal;
	float i1;		
	float i2;		
}NodeElecInfo;

// 节点信息和所在页面
typedef struct 
{
	BoardAddr baddr;
	u8 name[64]; // 一个汉字占用两个字节  是GB2312或GBK
	u8 page;
	u8 index;		 // 页面第几个
	u8 totalPage;
	u8 totalNode;// 总节点数 nodeInfo[0].totalNode才有效 用于flash存储
	NodeElecInfo eInfo;
}NodeInfo;
 
 
extern NodeInfo nodeInfo[MAX_NODE];
 
 
void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);
void RCC_Configuration(void);
void RTC_Configuration(void);
void NVIC_Configuration(void);
void LED_Configuration(void);
void Relay_Configuration(void); 
void LogPin_Configuration(void);
void RE485Pin_Configuration(void);
void USART_Configuration(void);  
void TIM_3_Configuration(void);
void WDG_Init(uint32_t timeout_ms);
#endif
