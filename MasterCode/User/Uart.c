#include "Uart.h"  
#include "Tools.h"  
  
UartTFTRecivePack uart2TFTPack;
UartPack uart3_485Pack; 
UartTFTRecivePackUser pTFTPackUser;

u8 g_retBuf[120]; 


/* 数组拷贝函数1 */
void str_copy_usart_buf(unsigned  char *des, unsigned char len ,unsigned  char *sourse)
{
   unsigned char cont;
	for(cont=0;cont<len;cont++)  
	{
		des[cont] = sourse[cont];
	}

		des[len] = '\0';  
}

/* 数组拷贝函数2 */
void str_copy_usart_buf2(unsigned  char *des, unsigned char len ,unsigned  char *sourse)
{
   unsigned char cont;
	for(cont=0;cont<len;cont++)  
	{
    des[cont] = sourse[cont];
	} 
    des[len] = '\0'; 
} 

 
/*串口X发送 发送len字节节*/
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


/*串口X发送字符串*/
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
 
/* 串口1数据置位 准备重新接收 */
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
 @ 功能：更新TFT文本数据 
 @ 入口： 
 *********************************************************************************************************/
void buildAndSendDataToTFT(u8 *pbuf, u8 pagid, u8 ctrlid, char* str)
{
	int strLen = 0;
	
	// 帧头
	pbuf[0] = 0xee;
	
	// 指令
	pbuf[1] = 0xB1;
	pbuf[2] = 0x10;
	
	// 页面id  
	pbuf[3] = 0;
	pbuf[4] = pagid;
	
	// 控件id
	pbuf[5] = 0;
	pbuf[6] = ctrlid;
	
	// string
	strLen = strlen(str)+1;
	memcpy(&pbuf[7], str, strLen);
	pbuf[strLen + 6]  = '\0';
	// 帧尾
	pbuf[strLen + 7]  = 0xff;
	pbuf[strLen + 8]  = 0xfc;
	pbuf[strLen + 9]  = 0xff;
	pbuf[strLen + 10] = 0xff;
	
	// 发送数据 
	//printf("uplen %d : %s \r\n", strLen, str);
	//printf("build : ");
	//printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}


/***********************************************************************************************************
 @ 功能：获取TFT文本数据 ,EE B1 11 00 02 00 09 FF FC FF FF
 @ 备注：返回 EE B1 11 00 02 00 09 (11) [4E 61 6D 65 68 68 68 68] 00 FF FC FF FF
 *********************************************************************************************************/
void getTFTText(u8 *pbuf, u8 pagid, u8 ctrlid)
{
	int strLen = 0;
	
	// 帧头
	pbuf[0] = 0xee;
	
	// 指令
	pbuf[1] = 0xB1;
	pbuf[2] = 0x11;
	
	// 页面id  
	pbuf[3] = 0;
	pbuf[4] = pagid;
	
	// 控件id
	pbuf[5] = 0;
	pbuf[6] = ctrlid;
	 
	// 帧尾
	pbuf[strLen + 7]  = 0xff;
	pbuf[strLen + 8]  = 0xfc;
	pbuf[strLen + 9]  = 0xff;
	pbuf[strLen + 10] = 0xff;
	
	// 发送数据  
	printf("build : ");
	printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}


/***********************************************************************************************************
 @ 功能：生成节点帧数据并发送
 @ 入口： 
 *********************************************************************************************************/
void buildAndSendDataToNode(u8 *pbuf, BoardAddr *addr, u8 cmd, u8 datalen, u8 *data)
{ 
	//0x5A, 0xA5, 0x0D, 0xFF, 0xFF, 0x00, 0x0A, 0x02, 0x00, 0x81, 97
	
	// 帧头
	pbuf[0] = 0x5A; 
	pbuf[1] = 0xA5;
	
	// 节点类型
	pbuf[2] = addr->addr[0];
	
	// 数据指向地址
	pbuf[3] = addr->addr[1];
	pbuf[4] = addr->addr[2];
	
	// cmd 
	pbuf[5] = 0;  // cmd 预留位
	pbuf[6] = cmd;
	
	// 数据类型
	pbuf[7] = 0;
	
	// 数据长度
	pbuf[8] = datalen;  
	
	// 数据开始  
	memcpy(&pbuf[9], data, datalen); 
	
	// 校验和
	pbuf[datalen+9] = CheckSum(pbuf, datalen+9);
	
	// 发送数据  
	printf("build u3: ");
	printHex(g_retBuf, datalen + 10);   
	delay_ms(100);
	Usart_SendByte(USART3, (char*)g_retBuf, datalen + 10); 
}
