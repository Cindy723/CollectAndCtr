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

// ʹ�ô���2�������ڵ����TFT
//#define TESTTFT_DC 

// LOG���� 
#define DEBUGUSART USART1

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
 
// �̵�������		 
#define RELAY1  PBout(RELAY1PIN)
#define RELAY2  PBout(RELAY2PIN)

// 485�������
#define CTRL485SEND PAout(RS485DIRPIN) = RS485SEND
#define CTRL485RECE PAout(RS485DIRPIN) = RS485RECE

// LED ����
#define LEDContrl(n, sw) (PBout(n) = sw) 


 
// �����ⲿ����ܶ���
#define POWERCONTRL  0X0A  // ������Դͨ��
#define REQUESTELEC  0X0B  // ��������������� 
#define REQUESTADDR  0X0C  // ��������ڵ��ַ
#define SETBOARDADDR 0X0D  // �����޸Ľڵ��ַ,���ڵ�û�д����ַʱʹ��0��ַ�·������Ѿ��е�ַʱ��Ҫʹ��ԭ�ȵ�ַ����. ����ʹ�ù㲥��ַ
 
 
// AC��ѹУ��ֵ
#define ACADJVP 0.989803567
// AC����У��ֵ
#define ACADJAP 1.149905189
 
// ���ڵ�����
#define MAX_NODE 255

// ���ýڵ��ɾ���ڵ����õ�����
#define SET_PSW "123456"
  
// ��ע��Ľڵ������ж�ʱ�� *10ms = s
#define NODE_TIMEOUT10ms 500
	
// AC �쳣��ֵ
#define AC_MAX_V 260
#define AC_MIN_V 180
#define AC_MAX_A 12
	
// �ڵ������Ϣ  
typedef struct
{
	u8 addr[3];
	u8 addrStr[6]; // dc 11 33 -->> "dc1133"
	uint32_t addrInt;
	u8 *type;
}BoardAddr; 
 

// ��Ӧid�ĵ�����Ϣ
typedef struct 
{ 
	float vTotal;
	float i1;		// AC ʱi1��Ч
	float i2;		
	u8 erro[2]; // index0��ʾCH1����λ index1��ʾCH2����λ AC ʱindex0��Ч
}NodeElecInfo;


// �ڵ���Ϣ������ҳ��
typedef struct 
{
	BoardAddr baddr;
	u8 name[32]; 				// һ������ռ�������ֽ�  ��GB2312��GBK 
	u8 totalNode;				// �ܽڵ��� nodeInfo[0].totalNode����Ч ����flash�洢
	NodeElecInfo eInfo; // ��ǰ�ڵ��������
	u8 needDispElec; 		// �յ��ڵ�����ʱ��λ ��ʾ��λ ��������������ʾ
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
