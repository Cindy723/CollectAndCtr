#ifndef _Init_H
#define _Init_H

#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_gpio.h"
#include "main.h"  

// ���ӳ���汾
#define VERSIONFRIST 0
#define VERSIONLAST 11

//#define MINIBOARD 

// ʹ�ô���2�������ڵ����HMI
//#define TESTHMI_DC 

// LOG���� 
#define DEBUGUSART USART3

// �źŶ���
#define LEDON  1
#define LEDOFF 0

#define POWER_ON  1
#define POWER_OFF 0

#define POWERCH1 1
#define POWERCH2 2

#define RS485SEND 1
#define RS485RECE 0

#define OI_TXD	PBout(3)

// �ӿڶ��� 
#define LEDRUNPINCFG   		GPIO_Pin_12
#define LED2PINCFG 				GPIO_Pin_13
#define LED3PINCFG 				GPIO_Pin_14
#define TYPEPINCFG 				GPIO_Pin_15  // �ߵ�ƽ��ʾAC���� �͵�ƽ��ʾDC��

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
 
// �̵�������		 
#define RELAY1  PBout(RELAY1PIN)
#define RELAY2  PBout(RELAY2PIN)

// 485�������
#define CTRL485SEND PAout(RS485DIRPIN) = RS485SEND
#define CTRL485RECE PAout(RS485DIRPIN) = RS485RECE

// LED ����
#define LEDContrl(n, sw) (PBout(n) = sw) 


 
// ��������ܶ���
#define POWERCONTRL  0X0A  // ������Դͨ��
#define REQUESTELEC  0X0B  // ���������������
#define REQUESTADDR  0X0C  // ��������ڵ��ַ
#define SETBOARDADDR 0X0D  // �����޸Ľڵ��ַ,���ڵ�û�д����ַʱʹ��0��ַ�·������Ѿ��е�ַʱ��Ҫʹ��ԭ�ȵ�ַ����. ����ʹ�ù㲥��ַ
 
// HMI��ָ��
#define BUTTON_CMD 0XB1
 
// AC��ѹУ��ֵ
#define ACADJVP 0.989803567
// AC����У��ֵ
#define ACADJAP 1.149905189

 
// �ڵ������Ϣ  
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
void USART_Configuration(void);  
void TIM_3_Configuration(void);
void WDG_Init(uint32_t timeout_ms);
#endif
