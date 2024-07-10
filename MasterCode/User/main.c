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
		Draw_BMP(0,0,128,8,BMP_biankuang);   //�ڣ�0,0����ʼ����  ��ʾ128*(8ҳ*8)��ͼƬ(ÿҳ8�㣬Ҳ��y��������8��Ϊ��λ)
		printf("\r\nMaster Rebbot!\r\n\r\n"); 
		Xintiao(); delay_ms(100); Xintiao(); delay_ms(100);	Xintiao();
		Draw_BMP(0,0,128,8,BMP_biankuang);   
		// OLED_P6x8Str(0,0,(unsigned char*)"     Oled Monitor  -x ",1); 
	
		// ��ʼ������ HMI ���ݰ��ṹ��ָ��
		uart2HMIPack.cmd0 				 = &uart2HMIPack.dataBuf[1];
		uart2HMIPack.cmd1 				 = &uart2HMIPack.dataBuf[2];  
		uart2HMIPack.Screen_id0 	 = &uart2HMIPack.dataBuf[3];
		uart2HMIPack.Screen_id1 	 = &uart2HMIPack.dataBuf[4];
		uart2HMIPack.Control_id0 	 = &uart2HMIPack.dataBuf[5];
		uart2HMIPack.Control_id1 	 = &uart2HMIPack.dataBuf[6];  
		uart2HMIPack.Control_type  = &uart2HMIPack.dataBuf[7];
		uart2HMIPack.Subtype  		 = &uart2HMIPack.dataBuf[8]; 
		uart2HMIPack.Status   		 = &uart2HMIPack.dataBuf[9];
		
		// ��ʼ���û��Զ��� HMI �ṹ��ָ��
		pHMIPackUser.cmd 			  	 = &uart2HMIPack.dataBuf[1]; 
		pHMIPackUser.dataType 		 = &uart2HMIPack.dataBuf[2];  
		pHMIPackUser.dataLen 	  	 = &uart2HMIPack.dataBuf[3];  
		pHMIPackUser.data 				 = &uart2HMIPack.dataBuf[4];  
		
		// ��ʼ���ڵ���Ӧ���ݰ��ṹ��ָ�� 
		uart3_485Pack.addr0 			 = &uart3_485Pack.dataBuf[2];   
		uart3_485Pack.addr1 			 = &uart3_485Pack.dataBuf[3];  
		uart3_485Pack.addr2 			 = &uart3_485Pack.dataBuf[4];  
		uart3_485Pack.cmd 			   = &uart3_485Pack.dataBuf[6]; // 5 ����
		uart3_485Pack.contenType   = &uart3_485Pack.dataBuf[7];
		uart3_485Pack.dataLen		   = &uart3_485Pack.dataLen[8];
		uart3_485Pack.content 	   = &uart3_485Pack.dataBuf[9]; 
		
		// ���ڲ��Խ׶� 485 ʹ��U3 HMI ʹ��U2 ��ӡʹ��U1
		// ��Ŀʵ��Ϊ 485 ʹ��u1, HMI ʹ�� u2
		while (1)
		{
			HMIanalysis();
			NodeDataAnalysis();
			IntervalProc(); 
		}
}
