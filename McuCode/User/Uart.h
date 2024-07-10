#ifndef _USERUART_H
#define _USERUART_H

#include "init.h"
  
#define HMI_BUTTON_LEN 14
 
/* 定义串口数据包 */
typedef struct 
{
	u8 dataOrig[255];		 		// 完整原始数据 仅接收处使用
	u8 dataBuf[255];		 		// 完整数据 接收完成后拷贝到这里
	const u8 *dataLen;	 		// 数据长度
	const u8 *contenType;		// 数据类型
	const u8 *content;   		// 数据内容	
	const u8 *cmd;			 		// 命令
	const u8 *sum;			 		// 校验和
	u8 receiveok;				 		// 接收完成标志
	u8 busy;						 		// 数据接收中
	u8 Counter;					 		// 总接收计数 
	u8 packLen;					 		// 数据包长度 
}UartPack;
 

/* 定义HMI接收数据包 */
typedef struct 
{
	u8 dataOrig[120];		  	// 完整原始数据 仅接收处使用
	u8 dataBuf[120];		  	// 完整数据 接收完成后拷贝到这里
	const u8 *cmd0;			  	// 指令
	const u8 *cmd1;			  	// 指令
	const u8 *Screen_id0; 	// 页面id	
	const u8 *Screen_id1; 	// 页面id	 
	const u8 *Control_id0;	// 控件id
	const u8 *Control_id1;	// 控件id
	const u8 *Control_type;	 
	const u8 *Subtype;	  
	const u8 *Status ;	 
	u8 receiveok;				 		// 接收完成标志
	u8 busy;						 		// 数据接收中
	u8 Counter;					 		// 总接收计数 
	u8 packLen;					 		// 数据包长度 
}UartHMIRecivePack;
  

extern u8 g_retBuf[120];
extern UartPack uart1Pack;
extern UartPack uart2Pack; 
extern UartHMIRecivePack uart2HMIPack; 

void USART_data_Reset(USART_TypeDef * pUSARTx);
void Usart_SendByte(USART_TypeDef * pUSARTx, char* ch, u8 len);
void Usart_SendString(USART_TypeDef * pUSARTx, char *str);

void str_copy_usart_buf(unsigned  char *des, unsigned char len ,unsigned  char *sourse);
void str_copy_usart_buf2(unsigned  char *des, unsigned char len ,unsigned  char *sourse);
 
void buildAndSendDataTo485(u8 *pbuf, u8 cmd, u8 datalen, u8* data);
void buildAndSendDataToHMI(u8 *pbuf, u8 ctrlid, char* str);
#endif
