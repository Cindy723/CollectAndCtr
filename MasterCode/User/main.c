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
		// Draw_BMP(0,0,128,8,BMP_biankuang);   //�ڣ�0,0����ʼ����  ��ʾ128*(8ҳ*8)��ͼƬ(ÿҳ8�㣬Ҳ��y��������8��Ϊ��λ)
		printf("\r\nMaster Rebbot!\r\n\r\n"); 
		// Xintiao();
		// Draw_BMP(0,0,128,8,BMP_biankuang);   
		// OLED_P6x8Str(0,0,(unsigned char*)"     Oled Monitor  -x ",1); 
	
		// ��ʼ������ TFT ���ݰ��ṹ��ָ��
		uart2TFTPack.cmd0 				 = &uart2TFTPack.dataBuf[1];
		uart2TFTPack.cmd1 				 = &uart2TFTPack.dataBuf[2];  
		uart2TFTPack.Screen_id0 	 = &uart2TFTPack.dataBuf[3];
		uart2TFTPack.Screen_id1 	 = &uart2TFTPack.dataBuf[4];
		uart2TFTPack.Control_id0 	 = &uart2TFTPack.dataBuf[5];
		uart2TFTPack.Control_id1 	 = &uart2TFTPack.dataBuf[6];  
		uart2TFTPack.Control_type  = &uart2TFTPack.dataBuf[7];
		uart2TFTPack.Subtype  		 = &uart2TFTPack.dataBuf[8]; 
		uart2TFTPack.Status   		 = &uart2TFTPack.dataBuf[9];
		
		// ��ʼ���û��Զ��� TFT �ṹ��ָ��
		pTFTPackUser.cmd 			  	 = &uart2TFTPack.dataBuf[1]; 
		pTFTPackUser.dataType 		 = &uart2TFTPack.dataBuf[2];  
		pTFTPackUser.dataLen 	  	 = &uart2TFTPack.dataBuf[3];  
		pTFTPackUser.data 				 = &uart2TFTPack.dataBuf[4];  
		
		// ��ʼ���ڵ���Ӧ���ݰ��ṹ��ָ�� 
		uart3_485Pack.addr0 			 = &uart3_485Pack.dataBuf[2];   
		uart3_485Pack.addr1 			 = &uart3_485Pack.dataBuf[3];  
		uart3_485Pack.addr2 			 = &uart3_485Pack.dataBuf[4];  
		uart3_485Pack.cmd 			   = &uart3_485Pack.dataBuf[6]; // 5 ����
		uart3_485Pack.contenType   = &uart3_485Pack.dataBuf[7];
		uart3_485Pack.dataLen		   = &uart3_485Pack.dataLen[8];
		uart3_485Pack.content 	   = &uart3_485Pack.dataBuf[9]; 
		
		// ���ڲ��Խ׶� 485 ʹ��U3 TFT ʹ��U2 ��ӡʹ��U1
		// ��Ŀʵ��Ϊ 485 ʹ��u1, TFT ʹ�� u2
		while (1)
		{
			TFTanalysis();
			NodeDataAnalysis();
			IntervalProc(); 
		}
}
