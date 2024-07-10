#include "Uart.h"  
#include "Tools.h"  

UartPack uart1Pack;  
UartPack uart2Pack; 
UartHMIRecivePack uart2HMIPack;

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
	if(pUSARTx == USART1)
	{
		uart1Pack.busy = 0; 
		uart1Pack.Counter = 0;
		memset(uart1Pack.dataOrig , 0, sizeof(uart1Pack.dataOrig)); 
		printf("reset USART1\r\n");
	}
	else if(pUSARTx == USART2)
	{
		uart2Pack.busy = 0; 
		uart2Pack.Counter = 0;
		memset(uart2Pack.dataOrig , 0, sizeof(uart2Pack.dataOrig)); 
#ifdef TESTHMI_DC
		uart2HMIPack.busy = 0; 
		uart2HMIPack.Counter = 0;
		memset(uart2HMIPack.dataOrig , 0, sizeof(uart2HMIPack.dataOrig));  
#endif		
		printf("reset uart2HMIPack\r\n"); 
	}
} 

/***********************************************************************************************************
 @ ���ܣ� ������Ӧ���� 
 @ ��ڣ�ָ��֡ͷ�� ��Ӧ��Ӧ��ָ���룬���ݳ��ȣ�����
 *********************************************************************************************************/
void buildAndSendDataTo485(u8 *pbuf, u8 cmd, u8 datalen, u8* data)
{
	// ֡ͷ
	pbuf[0] = 0xA5;
	pbuf[1] = 0x5A;
	
	// ������
	pbuf[2] = *boardAddr.type;
	
	// ���ַ
	pbuf[3] = boardAddr.addr[1];
	pbuf[4] = boardAddr.addr[2];
	
	// ����
	pbuf[5] = 0;
	pbuf[6] = cmd;
	
	// ��������
	pbuf[7] = 0;
	
	// ���ݳ���
	pbuf[8] = datalen; // ��������ռ��1
	
	// ���� (��Ч�غ�)
	memcpy(&pbuf[9], data, datalen);
	
	// У���
	pbuf[datalen + 9] = CheckSum(pbuf, datalen + 9);  
	
	// ��������
	printf("mcubuild : ");
	printHex(g_retBuf, datalen + 10);
	Usart_SendByte(USART1, (char*)g_retBuf, datalen + 10); 
}
 

/***********************************************************************************************************
 @ ���ܣ�����HMI�ı����� 
 @ ��ڣ� 
 *********************************************************************************************************/
void buildAndSendDataToHMI(u8 *pbuf, u8 ctrlid, char* str)
{
	int strLen = 0;
	
	// ֡ͷ
	pbuf[0] = 0xee;
	
	// ָ��
	pbuf[1] = 0xB1;
	pbuf[2] = 0x10;
	
	// ҳ��id  
	pbuf[3] = 0;
	pbuf[4] = 1;
	
	// �ؼ�id
	pbuf[5] = 0;
	pbuf[6] = ctrlid;
	
	// string
	strLen = strlen(str)+1;
	memcpy(&pbuf[7], str, strLen);
	pbuf[strLen + 6]  = '\0';
	// ֡β
	pbuf[strLen + 7]  = 0xff;
	pbuf[strLen + 8]  = 0xfc;
	pbuf[strLen + 9]  = 0xff;
	pbuf[strLen + 10] = 0xff;
	
	// �������� 
	printf("uplen %d : %s \r\n", strLen, str);
	printf("build : ");
	printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}
