#include "Uart.h"  
#include "Tools.h"  
  
UartTFTRecivePack uart2TFTPack;
UartPack uart3_485Pack; 
UartTFTRecivePackUser pTFTPackUser;

u8 g_retBuf[120]; 


/* ���鿽������1 */
void str_copy_usart_buf(unsigned  char *des, unsigned char len ,unsigned  char *sourse)
{
   unsigned char cont;
	for(cont=0;cont<len;cont++)  
	{
		des[cont] = sourse[cont];
	}

		des[len] = '\0';  
}

/* ���鿽������2 */
void str_copy_usart_buf2(unsigned  char *des, unsigned char len ,unsigned  char *sourse)
{
   unsigned char cont;
	for(cont=0;cont<len;cont++)  
	{
    des[cont] = sourse[cont];
	} 
    des[len] = '\0'; 
} 

 
/*����X���� ����len�ֽڽ�*/
void Usart_SendByte(USART_TypeDef * pUSARTx, char* ch, u8 len)
{ 
	unsigned char cont;  
	CTRL485SEND;
	for(cont=0;cont<len;cont++)  
	{
		USART_SendData(pUSARTx,*(ch++));
		while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	} 
	CTRL485RECE;
}


/*����X�����ַ���*/
void Usart_SendString(USART_TypeDef * pUSARTx, char *str)
{ 
//	while(*str != '\0')   
//	{
//		USART_SendData(pUSARTx, *(str ++));
//	}  
	int len = strlen(str);
	int j = 0;
	for(; j < len; j++)
	{
		printf("%c", str[j]); 
		USART_SendData(pUSARTx, str[j]);
    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET) ;
	}  
	
} 
 
/* ����1������λ ׼�����½��� */
void USART_data_Reset(USART_TypeDef * pUSARTx)
{
//	if(pUSARTx == USART1)
//	{
//		uart1Pack.busy = 0; 
//		uart1Pack.Counter = 0;
//		memset(uart1Pack.dataOrig , 0, sizeof(uart1Pack.dataOrig)); 
//		printf("reset USART1\r\n");
//	}
	if(pUSARTx == USART2)
	{ 
		uart2TFTPack.busy = 0; 
		uart2TFTPack.Counter = 0;
		memset(uart2TFTPack.dataOrig , 0, sizeof(uart2TFTPack.dataOrig));  
		uart2TFTPack.endflag = 0;
		printf("reset uart2TFTPack\r\n"); 
	}
	if(pUSARTx == USART3)
	{ 
		uart3_485Pack.busy = 0; 
		uart3_485Pack.Counter = 0;
		memset(uart3_485Pack.dataOrig , 0, sizeof(uart3_485Pack.dataOrig));  
	
		printf("reset uart3_485Pack\r\n"); 
	}
} 
  

/***********************************************************************************************************
 @ ���ܣ����ɽڵ�֡���ݲ�����
 @ ��ڣ� 
 *********************************************************************************************************/
void buildAndSendDataToNode(BoardAddr *addr, u8 cmd, u8 datalen, u8 *data)
{ 
	//0x5A, 0xA5, 0x0D, 0xFF, 0xFF, 0x00, 0x0A, 0x02, 0x00, 0x81, 97
	
	// ֡ͷ
	g_retBuf[0] = 0x5A; 
	g_retBuf[1] = 0xA5;
	
	// �ڵ�����
	g_retBuf[2] = addr->addr[0];
	
	// ����ָ���ַ
	g_retBuf[3] = addr->addr[1];
	g_retBuf[4] = addr->addr[2];
	
	// cmd 
	g_retBuf[5] = 0;  // cmd Ԥ��λ
	g_retBuf[6] = cmd;
	
	// ��������
	g_retBuf[7] = 0;
	
	// ���ݳ���
	g_retBuf[8] = datalen;  
	
	// ���ݿ�ʼ  
	memcpy(&g_retBuf[9], data, datalen); 
	
	// У���
	g_retBuf[datalen+9] = CheckSum(g_retBuf, datalen+9);
	
	// ��������  
	printf("build u3: ");
	printHex(g_retBuf, datalen + 10);   
	delay_ms(100);
	Usart_SendByte(USART3, (char*)g_retBuf, datalen + 10); 
}
