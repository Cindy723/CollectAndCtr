#ifndef _USERUART_H
#define _USERUART_H

#include "init.h"
  
#define HMI_BUTTON_LEN 14
 
/* ���崮�����ݰ� */
typedef struct 
{
	u8 dataOrig[255];		 		// ����ԭʼ���� �����մ�ʹ��
	u8 dataBuf[255];		 		// �������� ������ɺ󿽱�������
	const u8 *dataLen;	 		// ���ݳ���
	const u8 *contenType;		// ��������
	const u8 *content;   		// ��������	
	const u8 *cmd;			 		// ����
	const u8 *sum;			 		// У���
	u8 receiveok;				 		// ������ɱ�־
	u8 busy;						 		// ���ݽ�����
	u8 Counter;					 		// �ܽ��ռ��� 
	u8 packLen;					 		// ���ݰ����� 
}UartPack;
 

/* ����HMI�������ݰ� */
typedef struct 
{
	u8 dataOrig[120];		  	// ����ԭʼ���� �����մ�ʹ��
	u8 dataBuf[120];		  	// �������� ������ɺ󿽱�������
	const u8 *cmd0;			  	// ָ��
	const u8 *cmd1;			  	// ָ��
	const u8 *Screen_id0; 	// ҳ��id	
	const u8 *Screen_id1; 	// ҳ��id	 
	const u8 *Control_id0;	// �ؼ�id
	const u8 *Control_id1;	// �ؼ�id
	const u8 *Control_type;	 
	const u8 *Subtype;	  
	const u8 *Status ;	 
	u8 receiveok;				 		// ������ɱ�־
	u8 busy;						 		// ���ݽ�����
	u8 Counter;					 		// �ܽ��ռ��� 
	u8 packLen;					 		// ���ݰ����� 
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
