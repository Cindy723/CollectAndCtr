#ifndef _USERUART_H
#define _USERUART_H

#include "init.h" 
 
// TFT ҳ�涨��
#define TFT_PAGE_SETNUM 2

/* ���崮�����ݰ� */
typedef struct 
{
	u8 dataOrig[255];		 				// ����ԭʼ���� �����մ�ʹ��
	u8 dataBuf[255];		 				// �������� ������ɺ󿽱�������
	const u8 *addr0;   					// �ڵ����ݰ�ʱ �ڵ��ַ0 ��������
	const u8 *addr1;	 					// �ڵ����ݰ�ʱ �ڵ��ַ1
	const u8 *addr2;	 					// �ڵ����ݰ�ʱ �ڵ��ַ2
	const u8 *dataLen;	 				// ���ݳ���
	const u8 *contenType;				// ��������
	const u8 *content;   				// ��������	
	const u8 *cmd;			 				// ����
	const u8 *sum;			 				// У���
	u8 receiveok;				 				// ������ɱ�־
	u8 busy;						 				// ���ݽ�����
	u8 Counter;					 				// �ܽ��ռ��� 
	u8 packLen;					 				// ���ݰ����� 
}UartPack;
 

/* ����TFT�������ݰ� */
typedef struct 
{
	u8 dataOrig[120];		  			// ����ԭʼ���� �����մ�ʹ��
	u8 dataBuf[120];		  			// �������� ������ɺ󿽱�������
	const u8 *cmd0;			  			// ָ��
	const u8 *cmd1;			  			// ָ��
	const u8 *Screen_id0; 			// ҳ��id	
	const u8 *Screen_id1; 			// ҳ��id	 
	const u8 *Control_id0;			// �ؼ�id
	const u8 *Control_id1;			// �ؼ�id
	const u8 *Control_type;	 
	const u8 *Subtype;	  
	const u8 *Status ;	 
	u8 receiveok;				 				// ������ɱ�־ 
	u8 busy;						 				// ���ݽ����� 
	u8 Counter;					 				// �ܽ��ռ��� 
	u8 packLen;					 				// ���ݰ����� 
	u8 endflag;
}UartTFTRecivePack;
  
/* ����TFT�û��Զ���������ݰ�ָ�� */
typedef struct 
{ 
	const u8 *cmd; 	  					// cmd Ϊ USER_CMD
	const u8 *dataType; 	  		// ��������	
	const u8 *dataLen; 					// ���ݳ��� 
	const u8 *data;							// ���� 
}UartTFTRecivePackUser;

extern u8 g_retBuf[120];
extern UartPack uart3_485Pack; ;
extern UartTFTRecivePack uart2TFTPack; 
extern UartTFTRecivePackUser pTFTPackUser; 

void USART_data_Reset(USART_TypeDef * pUSARTx);
void Usart_SendByte(USART_TypeDef * pUSARTx, char* ch, u8 len);
void Usart_SendString(USART_TypeDef * pUSARTx, char *str);

void str_copy_usart_buf(unsigned  char *des, unsigned char len ,unsigned  char *sourse);
void str_copy_usart_buf2(unsigned  char *des, unsigned char len ,unsigned  char *sourse);
  
void buildAndSendDataToTFT(u8 *pbuf, u8 pagid, u8 ctrlid, char* str);
void getTFTText(u8 *pbuf, u8 pagid, u8 ctrlid);
void buildAndSendDataToNode(u8 *pbuf, BoardAddr *addr, u8 cmd, u8 datalen, u8 *data);

#endif
