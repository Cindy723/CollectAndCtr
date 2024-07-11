#include "UartSendQueue.h"   
#include "UART.h"     

Message message[MAX_MESSAGES]= { 0 }; 
uint16_t MsgCount = 0; 	
u8 msgid = 0;

/**********************************************************************************************************
 @ 功能: 左移出队
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void QueueShiftLeft()
{
	uint8_t i = 0;
	for (i = 0; i < MsgCount - 1; i++) { 
		message[i] = message[i + 1];
	}
	MsgCount--;  
}


/***********************************************************************************************************
 @ 功能： 打印当前队列id
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void printQuenId()
{ 
	uint8_t i = 0;
	if(MsgCount > 1)
	{
		printf("Queun id ");
		for(i = 0; i < MsgCount - 1; i++) { 
			printf("%d ", message[i].id);  
		} 
		printf("\r\n");
	}
}

 
/***********************************************************************************************************
 @ 功能：添加队列消息 
 @ 入口： 
 @ 出口： 
 @ 备注： 
 *********************************************************************************************************/
void addTFTSendQueue(Message *qdata)
{  	
    if (MsgCount < MAX_MESSAGES) { 		 
				memcpy(&message[MsgCount], qdata, sizeof(message));  
				printQuenId();  
				MsgCount++; 
        if (MsgCount == 1) { // 第一条消息应该马上发 
        }
    } 
		else { 
				printf("发送队列已满 %d 条丢弃最早的消息 id %d\n", MAX_MESSAGES, message[0].id);
				QueueShiftLeft(); 
    } 
}
 
/***********************************************************************************************************
 @ 功能： 发送队列消息
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void sendQueueMSG()
{
   //QuemMutex = TRUE; 
  if (MsgCount > 0)  
	{
		Usart_SendByte(message[0].pUartx, (char*)message[0].buf, message[0].len); 
		QueueShiftLeft(); 
	}
		//QuemMutex = FALSE;
} 
