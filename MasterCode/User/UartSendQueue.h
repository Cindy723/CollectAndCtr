#ifndef  _QUEEN_SENDTFT_H
#define  _QUEEN_SENDTFT_H

#include "main.h"  

#define MAX_MESSAGES 20

typedef struct {
    u8 buf[120]; 
    u8 id;
		u8 len;
		USART_TypeDef * pUartx;
} Message;

extern u8 msgid;
void addTFTSendQueue(Message *qdata);
void sendQueueMSG(void);
 
#endif
