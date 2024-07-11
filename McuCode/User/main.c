#include "main.h" 
#include "stm32f10x_it.h"    
#include "Tools.h"      
#include "Application.h"  
#include "ADC.h"   
#include "Uart.h"  
 

/**********************************************************************************************************
 @ 功能：主程序 
*********************************************************************************************************/
int main(void)
{  
	HSE_SetSysClock(RCC_PLLMul_9);
	SysTick_Init();  
	delay_ms(1000);  
	RCC_Configuration();
	LED_Configuration(); 
	USART_Configuration();
	RE485Pin_Configuration();
	// RTC_Configuration();
	Relay_Configuration();
#ifdef MINIBOARD
	LogPin_Configuration();
#endif
	printf("   \n\n\n\n\nRestart Flag!! ver %d.%d\n\n\n", VERSIONFRIST, VERSIONLAST);   
	TIM_3_Configuration(); 
	// WDG_Init(3000); 
	
	NVIC_Configuration();
	LEDContrl(LEDRUNPIN, LEDON);
	delay_ms(500);
	LEDContrl(LEDRUNPIN, LEDOFF);
	
	// 地址设置
	if(PBin(LEDRUNPIN) == 1){
	boardAddr.addr[0] = ACTYPE;
	}
	else{
	boardAddr.addr[0] = DCTYPE;
	}
	getChipIdStr(g_ChipIDS);  
	boardAddr.addr[1] = g_ChipIDS[23];
	boardAddr.addr[2] = g_ChipIDS[24];
	boardAddr.type = &boardAddr.addr[0];
	StrToHexByte(boardAddr.addrStr, boardAddr.addr); 
	rwTypeAndAddr(1, &boardAddr); // 默认写ID后两位
	rwTypeAndAddr(0, &boardAddr); 
	  
	
	// 初始化主机数据包结构体指针
	boardAddr.type 			 = &boardAddr.addr[0];
	uart1Pack.cmd 			 = &uart1Pack.dataBuf[6]; // 5 保留
	uart1Pack.contenType = &uart1Pack.dataBuf[7];
	uart1Pack.dataLen		 = &uart1Pack.dataLen[8];
	uart1Pack.content 	 = &uart1Pack.dataBuf[9]; 
	uart1Pack.Counter		 = 0;
	 
	// 初始化 HMI 数据包结构体指针 
	uart2HMIPack.cmd0 				= &uart2HMIPack.dataBuf[1];  
	uart2HMIPack.cmd1 				= &uart2HMIPack.dataBuf[2];  
	uart2HMIPack.Screen_id0 	= &uart2HMIPack.dataBuf[3];
	uart2HMIPack.Screen_id1 	= &uart2HMIPack.dataBuf[4];
	uart2HMIPack.Control_id0 	= &uart2HMIPack.dataBuf[5];
	uart2HMIPack.Control_id1 	= &uart2HMIPack.dataBuf[6];  
	uart2HMIPack.Control_type = &uart2HMIPack.dataBuf[7];
	uart2HMIPack.Subtype  		= &uart2HMIPack.dataBuf[8]; 
	uart2HMIPack.Status   		= &uart2HMIPack.dataBuf[9];

	// 如果是 DC 板：
	if(boardAddr.type){ADCx_Init();}
	CTRL485RECE;
	while (1)
	{
		// IWDG_ReloadCounter();
		MasterDataAnalysis();
		IntervalProc(); 
		
		// TEST
		// delay_ms(100);
		// updateElectInfoDC(0);
    // updateElectInfoAC(1); 
	  // sprintf(temp, "AC: %.2fV / %.2fA", 220.2, 1.43); 
	  // buildAndSendDataToHMI(g_retBuf, 101, temp);  
		
		// test
		//delay_ms(100);
		//buildAndSendDataTo485(g_retBuf, *uart1Pack.cmd, 4, temp);
	}
}
