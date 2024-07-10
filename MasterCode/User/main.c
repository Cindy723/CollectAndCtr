#include "main.h" 
#include "Application.h"  
#include "TFTCommunic.h"

int main(void)
{
    HSI_SetSysClock(RCC_PLLMul_9);
    RCC_Configuration(); 
    USART_Configuration();
    NVIC_Configuration();
		TIM_3_Configuration(); 
    OLED_Init(); 
		// Draw_BMP(0,0,128,8,BMP_biankuang);   //在（0,0）起始坐标  显示128*(8页*8)的图片(每页8点，也即y的坐标是8个为单位)
		printf("\r\nMaster Rebbot!\r\n\r\n"); 
		// Xintiao();
		// Draw_BMP(0,0,128,8,BMP_biankuang);   
		// OLED_P6x8Str(0,0,(unsigned char*)"     Oled Monitor  -x ",1); 
	
		// 初始化接收 TFT 数据包结构体指针
		uart2TFTPack.cmd0 				 = &uart2TFTPack.dataBuf[1];
		uart2TFTPack.cmd1 				 = &uart2TFTPack.dataBuf[2];  
		uart2TFTPack.Screen_id0 	 = &uart2TFTPack.dataBuf[3];
		uart2TFTPack.Screen_id1 	 = &uart2TFTPack.dataBuf[4];
		uart2TFTPack.Control_id0 	 = &uart2TFTPack.dataBuf[5];
		uart2TFTPack.Control_id1 	 = &uart2TFTPack.dataBuf[6];  
		uart2TFTPack.Control_type  = &uart2TFTPack.dataBuf[7];
		uart2TFTPack.Subtype  		 = &uart2TFTPack.dataBuf[8]; 
		uart2TFTPack.Status   		 = &uart2TFTPack.dataBuf[9];
		
		// 初始化用户自定义 TFT 结构体指针
		pTFTPackUser.cmd 			  	 = &uart2TFTPack.dataBuf[1]; 
		pTFTPackUser.dataType 		 = &uart2TFTPack.dataBuf[2];  
		pTFTPackUser.dataLen 	  	 = &uart2TFTPack.dataBuf[3];  
		pTFTPackUser.data 				 = &uart2TFTPack.dataBuf[4];  
		
		// 初始化节点响应数据包结构体指针 
		uart3_485Pack.addr0 			 = &uart3_485Pack.dataBuf[2];   
		uart3_485Pack.addr1 			 = &uart3_485Pack.dataBuf[3];  
		uart3_485Pack.addr2 			 = &uart3_485Pack.dataBuf[4];  
		uart3_485Pack.cmd 			   = &uart3_485Pack.dataBuf[6]; // 5 保留
		uart3_485Pack.contenType   = &uart3_485Pack.dataBuf[7];
		uart3_485Pack.dataLen		   = &uart3_485Pack.dataLen[8];
		uart3_485Pack.content 	   = &uart3_485Pack.dataBuf[9]; 
		
		// 现在测试阶段 485 使用U3 TFT 使用U2 打印使用U1
		// 项目实际为 485 使用u1, TFT 使用 u2
		while (1)
		{
			TFTanalysis();
			NodeDataAnalysis();
			IntervalProc(); 
		}
}
