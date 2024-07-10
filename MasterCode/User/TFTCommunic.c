#include "TFTCommunic.h"
#include "Uart.h"
#include "Tools.h"

u8 g_nodeTotalCount; 				  // ��ǰ�洢���ܽڵ����
 			
u8 g_RequestNodeElecFlag = 1; // �Ƿ���Ҫ����ڵ����ݲ���ʾ
u8 sendNodeDatabuf[12]; 		  // �����ڵ����Ч�غ�����

u8 g_currentPage;   			// ��ǰҳ��
u8 g_currentNodeListPage; // ���ý����µĵ�ǰ�ڵ��б�

u8 g_currentRquesNodeIndex = 0; // ��ǰ����ڵ�����

int RegisterNode(UartTFTRecivePackUser *pack);

/**********************************************************************************************************
@ ����: ������ҳ�������ʾ��ע��Ľڵ�
@ ����: 
@ ����: 
@ ��ע:  
*********************************************************************************************************/
void TFTDisplayNodeList2SetPag()
{
	u8 nameCtlAddrId[7] = {0, 1, 2, 3, 4, 5, 6};			// ����ڵ��ַID����
	u8 nameCtlNameId[7] = {0, 34, 35, 36, 37, 38, 39};// ����ڵ�����ID����
	u8 i;
	char temp[4];
	
	for(i = 0; i < g_nodeTotalCount; i++){ 
		// ����ÿ��ͣ���ڵ�һҳ
		if(i < 6) // С��һҳʱ�Ÿ���ע���б�
		{
			buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlAddrId[i+1], (char*)nodeInfo[i].baddr.addrStr);   
		}
	}
	 
	for(i = 0; i < g_nodeTotalCount; i++){  
		if(i < 6) 
		{
			buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlNameId[i+1], (char*)nodeInfo[i].name);  
		}
	}
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, 54, temp);  
}

/**********************************************************************************************************
 @ ����: ����ť�¼�
 @ ����: 
 @ ����: 
 @ ��ע: ���ص��ı�����Ҳ������
 *********************************************************************************************************/
void TFTButtonEvent()
{
	BoardAddr tempaddr;
	
	/* ��1ҳ�İ�ť */
	if(*uart2TFTPack.Screen_id1 == 1)  
	{
			switch(*uart2TFTPack.Control_id1) // ��һ��CH1����
			{			
				case 1 : /*  ��Ļ-�����ð�ť*/  
				{		
					printf("set button unDisplay\r\n"); 
					g_RequestNodeElecFlag = 0;
					g_currentPage = TFT_PAGE_SETNUM;
				}break;
				case 111:
				{
					printf("contrl node ch1\r\n"); 
					// ���������ҳ���Ӧ����id��Ӧ�İ��� ���´��Ӧ��ַ���ӿ���ָ��
					sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
					buildAndSendDataToNode(g_retBuf, &nodeInfo[0].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
				}break;  
			 case 121: // ��һ��CH2���� 
			 { 
				  printf("contrl node ch2\r\n"); 
				  //relayOpreat(uart2TFTPack.Status[0], POWERCH2);
			 }  break; 
			
			 default: printf("Notfun page1 button %x\r\n", *uart2TFTPack.Control_id1);  break;
		 }
	}
	
	/* ��2ҳ�İ�ť */
	else if(*uart2TFTPack.Screen_id1 == TFT_PAGE_SETNUM) 
	{ 
	 		switch(*uart2TFTPack.Control_id1) 
			{ 
				case 9 : /*  ��Ļ -�� ������ı� */
				{		 
				}break; 
				case 16 : /*  ��Ļ -�� �ڵ��Ϸ� */
				{		 

				}break; 
				case 17 : /*  ��Ļ -�� �ڵ��·� */
				{			
				}break;
				case 30 : /* ��Ļ-�������ڵ� */
				{	
					printf("search node button\r\n");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(g_retBuf, &tempaddr, REQUESTADDR, 0, sendNodeDatabuf); 
				}break; 
				case 32 : /* ��Ļ-��ע��ڵ� תCD USER_CMD_REGISTER*/
				{		
					printf("register node \r\n");   
				}break;
				case 33 : /* ��Ļ-�� �����÷�����ҳ�� */
				{		
					printf("back button Display\r\n"); 
					
					g_currentPage = TFT_DISP_SETNUM;
					g_RequestNodeElecFlag = 1;   
					g_currentRquesNodeIndex = 0;
				}break;
				default: printf("Notfun page2 button %x\r\n", *uart2TFTPack.Control_id1); break;
			}
	}
	else printf("Notfun Screen %d\r\n", *uart2TFTPack.Screen_id1); 

}

/**********************************************************************************************************
 @ ����: �������������� ����Ҫת����Ҫ�����ݸ��ڵ�
 @ ����: 
 @ ����: 
 @ ��ע: ת��������3 PB11xx PB10tx
 *********************************************************************************************************/
void TFTanalysis()
{  
		if(uart2TFTPack.receiveok)
		{
			uart2TFTPack.receiveok = 0;    
			switch(*uart2TFTPack.cmd0)
			{
				case BUTTON_CMD: 
				{  
					if(*uart2TFTPack.cmd1 == 0x11)
					{
						TFTButtonEvent(); 
					}
				}
				break;
				case USER_CMD: // BUTTON_CMD Ҳ�ᴥ�� ��Ҫʹ��������û�����һ���Դ洢
				{
					//TFTPackUser.
					if(*pTFTPackUser.dataType == USER_CMD_REGISTER) // ע��ڵ�
					{ 
						RegisterNode(&pTFTPackUser);
						TFTDisplayNodeList2SetPag();
					}
					else if(*pTFTPackUser.dataType == USER_CMD_NODE_PAGECHANGE) // ���ý���ڵ��б��л�
					{ 
							g_currentNodeListPage = pTFTPackUser.data[0] - '0';
							printf("USER_CMD -�� nodePageChange %d \r\n", g_currentNodeListPage); 
					}
					
				}
				break;
				default: printf("uart2TFTPack.cmd unkown! %x\r\n", *uart2TFTPack.cmd0); break; 
			}
			memset(uart2TFTPack.dataBuf, 0, sizeof(uart2TFTPack.dataBuf));  
		}
 
}
 
/**********************************************************************************************************
 @ ����: TFT->> ע��ڵ�
 @ ����: 
 @ ����: 
 @ ��ע: �ڵ�idĬ��ʹ��оƬid
 *********************************************************************************************************/
int RegisterNode(UartTFTRecivePackUser *pack)
{
	u8 i = 0;
	NodeInfo nodeTemp;
	strcpy((char*)nodeTemp.baddr.addrStr, (char*)(pack->data)); 
	strcpy((char*)nodeTemp.name, (char*)(pack->data + 7)); // '\0'
	 
	for(; i < g_nodeTotalCount; i++){
		 if(0 == strcmp((char*)nodeTemp.name, (char*)nodeInfo[i].name)||
			  0 == strcmp((char*)nodeTemp.baddr.addrStr, (char*)nodeInfo[i].baddr.addrStr))
		 {
				printf("USER_CMD-->> CMD_0xC1 id or name already registered!\r\n");
				return -1;
		 }
	}
	
	strcpy((char*)nodeInfo[g_nodeTotalCount].baddr.addrStr, (char*)(nodeTemp.baddr.addrStr)); 
	strcpy((char*)nodeInfo[g_nodeTotalCount].name, (char*)(nodeTemp.name));
	StrToHexByte(nodeInfo[g_nodeTotalCount].baddr.addrStr, nodeInfo[g_nodeTotalCount].baddr.addr);
	nodeInfo[g_nodeTotalCount].baddr.type = &nodeInfo[g_nodeTotalCount].baddr.addr[0];
 
	g_nodeTotalCount ++;
	printf("Node List: \r\n");
	for(i = 0; i < g_nodeTotalCount; i++){	// ��ӡע������ 
		printf("Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	}
	return  g_nodeTotalCount;
}


/***********************************************************************************************************
 @ ���ܣ�����TFT�ı����� 
 @ ��ڣ� 
 *********************************************************************************************************/
void buildAndSendStr2TFT(u8 *pbuf, u8 pagid, u8 ctrlid, char* str)
{
	int strLen = 0;
	
	// ֡ͷ
	pbuf[0] = 0xee;
	
	// ָ��
	pbuf[1] = 0xB1;
	pbuf[2] = 0x10;
	
	// ҳ��id  
	pbuf[3] = 0;
	pbuf[4] = pagid;
	
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
	//printf("uplen %d : %s \r\n", strLen, str);
	//printf("build : ");
	//printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}


/***********************************************************************************************************
 @ ���ܣ���ȡTFT�ı����� ,EE B1 11 00 02 00 09 FF FC FF FF
 @ ��ע������ EE B1 11 00 02 00 09 (11) [4E 61 6D 65 68 68 68 68] 00 FF FC FF FF
 *********************************************************************************************************/
void getTFTText(u8 *pbuf, u8 pagid, u8 ctrlid)
{
	int strLen = 0;
	
	// ֡ͷ
	pbuf[0] = 0xee;
	
	// ָ��
	pbuf[1] = 0xB1;
	pbuf[2] = 0x11;
	
	// ҳ��id  
	pbuf[3] = 0;
	pbuf[4] = pagid;
	
	// �ؼ�id
	pbuf[5] = 0;
	pbuf[6] = ctrlid;
	 
	// ֡β
	pbuf[strLen + 7]  = 0xff;
	pbuf[strLen + 8]  = 0xfc;
	pbuf[strLen + 9]  = 0xff;
	pbuf[strLen + 10] = 0xff;
	
	// ��������  
	printf("build : ");
	printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}
 

/***********************************************************************************************************
 @ ���ܣ���ʾ�������ݵ�TFT
 @ ��ע�� 
 *********************************************************************************************************/
void dispElec2TFT(u8 *pbuf, NodeInfo *info)
{
	char str[240], i;
  
	for(i = 0; i < g_nodeTotalCount; i++)
	{ 
		buildAndSendStr2TFT(g_retBuf, TFT_DISP_SETNUM, 5+i, (char*)nodeInfo[i].name);
		if(nodeInfo[i].baddr.addr[0] == 0xdc)
		{ 
			sprintf(str, "DC: %.2f V,  CH1 %.2f A,  CH2 %.2f A", nodeInfo[i].eInfo.vTotal,  nodeInfo[i].eInfo.i1, nodeInfo[i].eInfo.i2);
			buildAndSendStr2TFT(g_retBuf, TFT_DISP_SETNUM, 101+i, str);  
		}
		else if(nodeInfo[i].baddr.addr[0] == 0xac)
		{ 
			
		}
		 
	}
	  
}
