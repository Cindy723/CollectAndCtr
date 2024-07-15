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

// 使用串口2代替主节点测试HMI
//#define TESTHMI_DC 

// LOG串口 
#define DEBUGUSART USART3

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
#define TYPEPINCFG 				GPIO_Pin_15  // 高电平表示AC板子 低电平表示DC板

#define LEDRUNPIN     12
#define LED2PIN 			13
#define LED3PIN 		  14
#define TYPEPIN 			15
 
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

// TPS2492 FLT 出现异常 过流过压 短路 PG为触发芯片阈值 异常后拉低
#define FLT1PINCFG GPIO_Pin_4
#define PG1PINCFG  GPIO_Pin_3

#define FLT2PINCFG GPIO_Pin_6
#define PG2PINCFG  GPIO_Pin_5

#define FLT1_STATUS  PBin(4)
#define PG1_STATUS   PBin(3)
  
#define FLT2_STATUS  PBin(6)
#define PG2_STATUS   PBin(5)
 
// 串口命令功能定义
#define POWERCONTRL  0X0A  // 操作电源通断
#define REQUESTELEC  0X0B  // 主机请求电能数据
#define REQUESTADDR  0X0C  // 主机请求节点地址
#define SETBOARDADDR 0X0D  // 主机修改节点地址,当节点没有存入地址时使用0地址下发，当已经有地址时需要使用原先地址发送. 或者使用广播地址
 
// HMI的指令
#define BUTTON_CMD 0XB1
 
// AC电压校正值
#define ACADJVP 0.989803567
// AC电流校正值
#define ACADJAP 1.149905189

 
// 节点基本信息  
typedef struct
{
	u8 addr[3];	
	u8 addrStr[6]; // dc 11 33 -->> "dc1133"
	u8 *type;
}BroadAddr;

enum type
{
  ACTYPE = 0xac,
  DCTYPE = 0xdc
}; 
extern BroadAddr boardAddr;
  
// DC节点异常标志 最高位表示FLT 最低位表示PG
typedef struct
{
	u8 CH1erro;
	u8 CH2erro;	 
}DCPowerSta; 
extern DCPowerSta dcPowerSta;

void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);
void RCC_Configuration(void);
void RTC_Configuration(void);
void GPIO_Configuration(void);  
void NVIC_Configuration(void);
void LED_Configuration(void);
void Relay_Configuration(void); 
void LogPin_Configuration(void);
void RE485Pin_Configuration(void);
void TPS2492Pin_Configuration(void);
void USART_Configuration(void);  
void TIM_3_Configuration(void);
void WDG_Init(uint32_t timeout_ms);
#endif
