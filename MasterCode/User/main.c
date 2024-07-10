#include "main.h" 
#include "Application.h"   

int main(void)
{
    HSI_SetSysClock(RCC_PLLMul_9);
    RCC_Configuration(); 
    USART_Configuration();
    NVIC_Configuration();
		TIM_3_Configuration(); 
    OLED_Init(); 
		Draw_BMP(0,0,128,8,BMP_biankuang);   //在（0,0）起始坐标  显示128*(8页*8)的图片(每页8点，也即y的坐标是8个为单位)
		printf("\r\nMaster Rebbot!\r\n\r\n"); 
		Xintiao(); delay_ms(100); Xintiao(); delay_ms(100);	Xintiao();
		Draw_BMP(0,0,128,8,BMP_biankuang);   
		// OLED_P6x8Str(0,0,(unsigned char*)"     Oled Monitor  -x ",1); 
	
		// 初始化接收 HMI 数据包结构体指针
		uart2HMIPack.cmd0 				 = &uart2HMIPack.dataBuf[1];
		uart2HMIPack.cmd1 				 = &uart2HMIPack.dataBuf[2];  
		uart2HMIPack.Screen_id0 	 = &uart2HMIPack.dataBuf[3];
		uart2HMIPack.Screen_id1 	 = &uart2HMIPack.dataBuf[4];
		uart2HMIPack.Control_id0 	 = &uart2HMIPack.dataBuf[5];
		uart2HMIPack.Control_id1 	 = &uart2HMIPack.dataBuf[6];  
		uart2HMIPack.Control_type  = &uart2HMIPack.dataBuf[7];
		uart2HMIPack.Subtype  		 = &uart2HMIPack.dataBuf[8]; 
		uart2HMIPack.Status   		 = &uart2HMIPack.dataBuf[9];
		
		// 初始化用户自定义 HMI 结构体指针
		pHMIPackUser.cmd 			  	 = &uart2HMIPack.dataBuf[1]; 
		pHMIPackUser.dataType 		 = &uart2HMIPack.dataBuf[2];  
		pHMIPackUser.dataLen 	  	 = &uart2HMIPack.dataBuf[3];  
		pHMIPackUser.data 				 = &uart2HMIPack.dataBuf[4];  
		
		// 初始化节点响应数据包结构体指针 
		uart3_485Pack.addr0 			 = &uart3_485Pack.dataBuf[2];   
		uart3_485Pack.addr1 			 = &uart3_485Pack.dataBuf[3];  
		uart3_485Pack.addr2 			 = &uart3_485Pack.dataBuf[4];  
		uart3_485Pack.cmd 			   = &uart3_485Pack.dataBuf[6]; // 5 保留
		uart3_485Pack.contenType   = &uart3_485Pack.dataBuf[7];
		uart3_485Pack.dataLen		   = &uart3_485Pack.dataLen[8];
		uart3_485Pack.content 	   = &uart3_485Pack.dataBuf[9]; 
		
		// 现在测试阶段 485 使用U3 HMI 使用U2 打印使用U1
		// 项目实际为 485 使用u1, HMI 使用 u2
		while (1)
		{
			HMIanalysis();
			NodeDataAnalysis();
			IntervalProc(); 
		}
}
