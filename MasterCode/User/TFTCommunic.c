#include "TFTCommunic.h"
#include "Uart.h"
#include "Tools.h"
#include "UartSendQueue.h"
#include "flash.h"
#include "stm32f10x_it.h"

CurrentTFTVariate currentTFTV;
 			
u8 g_RequestNodeElecFlag = 1; // �Ƿ���Ҫ����ڵ����ݲ���ʾ
u8 sendNodeDatabuf[12]; 		  // �����ڵ����Ч�غ�����
u8 g_nodeTotalCount; 				  // ��ǰ�洢���ܽڵ����
u8 g_currentRquesNodeIndex = 0; // ��ǰ����ڵ�����

// ��ʾҳ��id����
u8 dispNodeN[6] = {101, 102, 103, 104, 105, 106};
u8 dispNodeP[6] = {5, 6, 7, 8, 9, 10};
u8 CH1Button[6] = {111, 112, 113, 114, 115, 116};
u8 CH2Button[6] = {121, 122, 123, 124, 125, 126};
u8 StatusText[6] = {15, 16, 17, 19, 24, 26};

int RegisterNode(UartTFTRecivePackUser *pack);
void setListPagef2(u8 page);
void disp_hindTFTDisPage(u8 isdisp); 
void deleteNodeInfo(int index);
void updateListPagef2(void);
void updateRegisterCount(void);

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
	u8 offset = (currentTFTV.setPageNodeListF1 - 1) * ONEPAGENODE;
	printf("TFTDisplayNodeList2SetPag offset %d\r\n", offset);
	 
	// ���ݵ�ǰ������ʾ��ǰҳ�� ONEPAGENODE ��
	for(i = 0; i < 6; i++){   
		buildAndSendStr2TFT(TFT_SET_PAGE, nameCtlAddrId[i + 1], (char*)nodeInfo[offset + i].baddr.addrStr);  
	
	}
	  
	for(i = 0; i < 6; i++){   
		buildAndSendStr2TFT(TFT_SET_PAGE, nameCtlNameId[i + 1], (char*)nodeInfo[offset + i].name);  
	} 
	
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
	u8 ButtonCH1Start = 111;
	u8 ButtonCH2Start = 121;
	u8 offset = (currentTFTV.DispListPage - 1) * ONEPAGENODE;
	
	/* ��1ҳ�İ�ť */
	if(*uart2TFTPack.Screen_id1 == 1)  
	{
  
			if(*uart2TFTPack.Control_id1 == 1) // ���ð�ť����
			{
					printf("set button unDisplay\r\n"); 
				  dispSetTips("  ");
					TFTDisplayNodeList2SetPag();
					g_RequestNodeElecFlag = 0;
					currentTFTV.Page = TFT_SET_PAGE;
			}
			if(*uart2TFTPack.Control_id1 == 2) // ��ʾҳ���ӷ���
			{
					currentTFTV.DispListPage = *uart2TFTPack.Subtype_or_Conten - '0'; 
					printf("get DisplayPage f1 %d\r\n", currentTFTV.DispListPage);  
			}
			else if((*uart2TFTPack.Control_id1 == 11) || (*uart2TFTPack.Control_id1 == 12))	// �л���ҳ�� ��ʾ��Ӧҳ����ܲ��� ��û�ӿ���״̬
			{  
					disp_hindTFTDisPage(0);
					dispElec2TFT(nodeInfo);  
					currentTFTV.Page = TFT_DISP_PAGE;
			} 
			else if((*uart2TFTPack.Control_id1 > 110) &&  (*uart2TFTPack.Control_id1 < 117)) // ��һ�п��ذ���
			{
				printf("power ch1 change\r\n");  
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
			  buildAndSendDataToNode(&nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH1Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
				currentTFTV.Page = TFT_DISP_PAGE;
			}
			else if((*uart2TFTPack.Control_id1 > 120) &&  (*uart2TFTPack.Control_id1 < 127)) // �ڶ��п��ذ���
			{ 
				printf("power ch2 change\r\n"); 
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH2;
			  buildAndSendDataToNode(&nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH2Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
        currentTFTV.Page = TFT_DISP_PAGE;
			}

	}
	
	/* ��2ҳ�İ�ť */
	else if(*uart2TFTPack.Screen_id1 == TFT_SET_PAGE) 
	{ 
		  currentTFTV.Page = TFT_SET_PAGE;  
	 		switch(*uart2TFTPack.Control_id1) 
			{  			 
				case 21: /*  ��Ļ -�� ���صķ��� */
				{
					currentTFTV.setPageNodeListF1 = *uart2TFTPack.Subtype_or_Conten - '0'; 
					printf("get SetPage f1 %d\r\n", currentTFTV.setPageNodeListF1);  
				}
	      break;
				case 16 : /*  ��Ļ -�� ���ý���ڵ��Ϸ� */
				{		  
				 dispSetTips("��һҳ"); 
				}break; 
				case 17 : /*  ��Ļ -�� ���ý���ڵ��·� */
				{			
				 dispSetTips("��һҳ"); 
				}break;
				case 30 : /* ��Ļ-�������ڵ� */
				{	
					printf("search node button\r\n");
					buildAndSendStr2TFT(TFT_SET_PAGE, 8, " ");				
				  dispSetTips("����..");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(&tempaddr, REQUESTADDR, 0, sendNodeDatabuf);  
				}break; 
				case 32 : /* ��Ļ-��ע��ڵ� תCD USER_CMD_REGISTER*/
				{		
					printf("register node \r\n"); 	
				  dispSetTips("ע��ڵ�.."); 
				}break;
				case 33 : /* ��Ļ-�� �����÷�����ҳ�� */
				{		
					printf("back button Display\r\n");   
				  dispSetTips("  "); 
					currentTFTV.Page = TFT_DISP_PAGE;
					g_currentRquesNodeIndex = 0;
					g_RequestNodeElecFlag = 1;   
					disp_hindTFTDisPage(0);
				}break;
				case 53 : /* ��Ļ-�� ɾ���ڵ� */
				{		
					printf("delete node button\r\n");   
				}break;
				default: printf("Notfun page2 button %d\r\n", *uart2TFTPack.Control_id1); break;
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
				case USER_CMD: // BUTTON_CMD Ҳ�ᴥ�� 
				{ 
					 currentTFTV.Page = TFT_SET_PAGE; 
					if(*pTFTPackUser.dataType == USER_CMD_REGISTER) // ���� ע��ڵ�
					{ 
						RegisterNode(&pTFTPackUser);
						TFTDisplayNodeList2SetPag();  
					}
					else if(*pTFTPackUser.dataType == USER_CMD_PSWENTER) // ���� �����������
					{ 

					} 
					else if(*pTFTPackUser.dataType == USER_CMD_NODE_PAGECHANGE) // ���� �����б���ӱ仯
					{ 
						currentTFTV.setPageNodeListF1 = pTFTPackUser.data[0] - '0';
						printf("USER_CMD -�� nodePageChange %d \r\n", currentTFTV.setPageNodeListF1); 
						TFTDisplayNodeList2SetPag(); 
					}
					else if(*pTFTPackUser.dataType == USER_CMD_DISP_PAGECHANGE) // ��ʾ �����б���ӱ仯
					{ 
						currentTFTV.DispListPage = pTFTPackUser.data[0] - '0';
						printf("USER_CMD -�� dispPageChange %d \r\n", currentTFTV.DispListPage); 
						disp_hindTFTDisPage(0); 
						currentTFTV.Page = TFT_DISP_PAGE; 
					}
					else if(*pTFTPackUser.dataType == USER_CMD_DELETE_NODE) // ���� ����ɾ��ָ� ��ѡ��Ŀؼ�id ��ǰ���� ���������
					{ 
						u8 choose = 0,chooseOffset = 41, f1 = 0, str[24];
						choose = pTFTPackUser.data[0];
						f1 = pTFTPackUser.data[1];
						strcpy((char*)str, (char*)&pTFTPackUser.data[2]);
						printf("USER_CMD -�� dispPageDelete choose %d, f1 %d, psw %s\r\n", choose, f1, str); 
						deleteNodeInfo(choose - chooseOffset);
						TFTDisplayNodeList2SetPag(); 
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
	char temp[12];
	
	strcpy((char*)nodeTemp.baddr.addrStr, (char*)(pack->data)); 
	strcpy((char*)nodeTemp.name, (char*)(pack->data + 7)); // '\0'
	  
//	for(; i < g_nodeTotalCount; i++){
//		 if(0 == strcmp((char*)nodeTemp.name, (char*)nodeInfo[i].name)||
//			  0 == strcmp((char*)nodeTemp.baddr.addrStr, (char*)nodeInfo[i].baddr.addrStr))
//		 {
//				printf("USER_CMD-->> CMD_0xC1 id or name already registered!\r\n");
//				dispSetTips("�ڵ��Ѿ���ע��!");
//				return -1;
//		 }
//	}
	
	
	if((0 == strcmp((char*)nodeTemp.baddr.addrStr, "")) || (0 == strcmp((char*)nodeTemp.name, "")))
	{
		dispSetTips("�ڵ��ַ������Ϊ��!");
	  return -1; 
	}
	
	if(g_nodeTotalCount > MAX_NODE)
	{
		sprintf(temp, "�ڵ������� %d ��!", MAX_NODE);
		dispSetTips(temp);
	  return -1; 
	}
	
	
	// id ��ͬ����
		for(; i < g_nodeTotalCount; i++){ 
	   if(0 == strcmp((char*)nodeTemp.baddr.addrStr, (char*)nodeInfo[i].baddr.addrStr))
		 {
				printf("USER_CMD-->> CMD_0xC1 id or name already registered!\r\n");
				dispSetTips("�ڵ��Ѿ���ע��!");
				return -1;
		 }
	}
	
	strcpy((char*)nodeInfo[g_nodeTotalCount].baddr.addrStr, (char*)(nodeTemp.baddr.addrStr)); 
	strcpy((char*)nodeInfo[g_nodeTotalCount].name, (char*)(nodeTemp.name));
	StrToHexByte(nodeInfo[g_nodeTotalCount].baddr.addrStr, nodeInfo[g_nodeTotalCount].baddr.addr);
	nodeInfo[g_nodeTotalCount].baddr.type = &nodeInfo[g_nodeTotalCount].baddr.addr[0];
 
	g_nodeTotalCount ++;
	printf("Node Count %d List: \r\n", g_nodeTotalCount);
	for(i = 0; i < g_nodeTotalCount; i++){	// ��ӡע������ 
		printf("Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	}
	
  dispSetTips("�ڵ�ע��ɹ�!");
	updateListPagef2();
	updateRegisterCount();
	 
	nodeInfo[0].totalNode = g_nodeTotalCount;
	
  flash_erase(PARAM_MAX_SIZE, PARAM_SAVE_ADDR_BASE);
	
	//wFlashData((uint8_t*)nodeInfo, sizeof(nodeInfo), PARAM_SAVE_ADDR_BASE);
	//rFlashData((uint8_t*)nodeInfo, sizeof(nodeInfo), PARAM_SAVE_ADDR_BASE);
	
		for(i = 0; i < g_nodeTotalCount; i++){	// ��ӡע������ 
		printf("--- Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	} 
	
	
	return  g_nodeTotalCount;
}


/***********************************************************************************************************
 @ ���ܣ�����TFT�ı����� 
 @ ��ڣ� 
 *********************************************************************************************************/
void buildAndSendStr2TFT(u8 pagid, u8 ctrlid, char* str)
{
	int strLen = 0;
	
	// ֡ͷ
	g_retBuf[0] = 0xee;
	
	// ָ��
	g_retBuf[1] = 0xB1;
	g_retBuf[2] = 0x10;
	
	// ҳ��id  
	g_retBuf[3] = 0;
	g_retBuf[4] = pagid;
	
	// �ؼ�id
	g_retBuf[5] = 0;
	g_retBuf[6] = ctrlid;
	
	// string
	strLen = strlen(str)+1;
	memcpy(&g_retBuf[7], str, strLen);
	g_retBuf[strLen + 6]  = '\0';
	// ֡β
	g_retBuf[strLen + 7]  = 0xff;
	g_retBuf[strLen + 8]  = 0xfc;
	g_retBuf[strLen + 9]  = 0xff;
	g_retBuf[strLen + 10] = 0xff;
	
	// �������� 
	//printf("uplen %d : %s \r\n", strLen, str);
	//printf("build : ");
	//printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}


/***********************************************************************************************************
 @ ���ܣ���ȡTFT�ı����� ,EE B1 11 00 02 00 09 FF FC FF FF
 @ ��ע������ EE B1 11 00 02 00 09 (11) [4E 61 6D 65 68 68 68 68] 00 FF FC FF FF
				 ��ͨ������
 *********************************************************************************************************/
void getTFTText(u8 pagid, u8 ctrlid)
{
	int strLen = 0;
	int totalLen = 0;
	Message msg;
	
	// ֡ͷ
	g_retBuf[0] = 0xee;
	
	// ָ��
	g_retBuf[1] = 0xB1;
	g_retBuf[2] = 0x11;
	
	// ҳ��id  
	g_retBuf[3] = 0;
	g_retBuf[4] = pagid;
	
	// �ؼ�id
	g_retBuf[5] = 0;
	g_retBuf[6] = ctrlid;
	 
	// ֡β
	g_retBuf[strLen + 7]  = 0xff;
	g_retBuf[strLen + 8]  = 0xfc;
	g_retBuf[strLen + 9]  = 0xff;
	g_retBuf[strLen + 10] = 0xff;
	totalLen = strLen + 11;
	
	// ��������  
	printf("build : ");
	printHex(g_retBuf, totalLen);  
	//Usart_SendByte(USART2, (char*)g_retBuf, totalLen); 
	
	memcpy(msg.buf, g_retBuf, totalLen);
	msg.len = totalLen;
	msg.id = ++msgid;
	msg.pUartx = USART2;
	addTFTSendQueue(&msg); 
}
 

/***********************************************************************************************************
 @ ���ܣ���ʾ�������ݵ�TFT
 @ ��ע�� 
 *********************************************************************************************************/
void dispElec2TFT(NodeInfo *info)
{
	char str[240], i; 
	u8 offset = (currentTFTV.DispListPage - 1) * ONEPAGENODE;
	u8 toDisplay = ONEPAGENODE;
	u8 remaining = 0;

	// ����ʣ����Ŀ��
	if(g_nodeTotalCount == 0){
		toDisplay = 0;
		disp_hindTFTDisPage(0);
		return;
	}

  remaining = g_nodeTotalCount - offset;
	if (remaining < ONEPAGENODE){
		toDisplay = remaining;
	}
 
	// printf("dispElec2TFT offset %d toDisplay %d \r\n", offset, toDisplay);  
	// ��ʾ��ǰҳ������
	for(i = 0; i < toDisplay; i++)
	{   
		
		//if(nodeInfo[offset + i].needDispElec == 1)
		if(timerVariate.NodeTimeCount10ms[offset + i] < NODE_TIMEOUT10ms)
		{
			disp_hindTFTContrl(TFT_DISP_PAGE, CH1Button[i], 1);
			disp_hindTFTContrl(TFT_DISP_PAGE, CH2Button[i], 1);
			buildAndSendStr2TFT(TFT_DISP_PAGE, StatusText[i], "����");
			
			if(nodeInfo[offset + i].baddr.addr[0] == 0xdc)
			{ 
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				sprintf(str, "DC: %.2f V,  CH1: %.2f A,  CH2: %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1, nodeInfo[offset + i].eInfo.i2);
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeN[i], str);  
			}
			else if(nodeInfo[offset + i].baddr.addr[0] == 0xac)
			{
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				sprintf(str, "AC: %.2f V, %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1);
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeN[i], str);  
			}  
		}
		else // �ýڵ㳬�� NODE_TIMEOUT10ms δ����
		{
			//disp_hindTFTContrl(TFT_DISP_PAGE, CH1Button[i], 1);
			//disp_hindTFTContrl(TFT_DISP_PAGE, CH2Button[i], 1);
			buildAndSendStr2TFT(TFT_DISP_PAGE, StatusText[i], "������!");

			if(nodeInfo[offset + i].baddr.addr[0] == 0xdc)
			{ 
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				//sprintf(str, "DC: %.2f V,  CH1: %.2f A,  CH2: %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1, nodeInfo[offset + i].eInfo.i2);
				//buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeN[i], str);  
			}
			else if(nodeInfo[offset + i].baddr.addr[0] == 0xac)
			{
				buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				//sprintf(str, "AC: %.2f V, %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1);
				//buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeN[i], str);  
			}
		}
	} 
		
}



/***********************************************************************************************************
 @ ���ܣ����ý�����ʾ��ʾ��Ϣ
 @ ��ע�� 
 *********************************************************************************************************/
void dispSetTips(char* tip)
{
  buildAndSendStr2TFT(2, 47, tip);
}


/***********************************************************************************************************
 @ ���ܣ����÷�ĸ
 @ ��ע�� 
 *********************************************************************************************************/
void setListPagef2(u8 page)
{
	char temp[6];
	currentTFTV.setPageNodeListF2 = page;
	sprintf(temp, "%d", page); 
	printf("updateListPage f2 %s\r\n", temp);
  buildAndSendStr2TFT(1, 22, temp);
  buildAndSendStr2TFT(2, 22, temp);
}

/***********************************************************************************************************
 @ ���ܣ���ȡ����
 @ ��ע���л���ʾ�б�ʱ����Զ����� ������ʾ����ʱ ������Ҫ����ڵ� 
					������ʱ�����ݱ����� ���Զ�������������ʾ����ķ���
 *********************************************************************************************************/
void getListPagef1()
{
	getTFTText(1, 2);
	// getTFTText(2, 2);
}

/***********************************************************************************************************
 @ ���ܣ��������ý����ĸ 
 *********************************************************************************************************/
void updateListPagef2()
{
	printf("updateListPagef2 \r\n");
	if(g_nodeTotalCount > ONEPAGENODE)
	{
	 if(g_nodeTotalCount%ONEPAGENODE != 0)
	 setListPagef2((g_nodeTotalCount/ONEPAGENODE) + 1);
	}else  setListPagef2(1);
}
 
/***********************************************************************************************************
 @ ���ܣ�����ע������
 *********************************************************************************************************/
void updateRegisterCount()
{
	char temp[12];
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendStr2TFT(TFT_SET_PAGE, 54, temp);  
}

/***********************************************************************************************************
 @ ���ܣ���λTFT
 @ ��ע�� 
 *********************************************************************************************************/
void rebootTFT()
{
	g_retBuf[0] = 0xEE;
	g_retBuf[1] = 0x07;
	g_retBuf[2] = 0x35;
	g_retBuf[3] = 0x5a;
	g_retBuf[4] = 0x53;
	g_retBuf[5] = 0xa5;
		 
	g_retBuf[6]  = 0xff;
	g_retBuf[7]  = 0xfc;
	g_retBuf[8]  = 0xff;
	g_retBuf[9]  = 0xff; 
	Usart_SendByte(USART2, (char*)g_retBuf, 10); 
}

/***********************************************************************************************************
 @ ���ܣ���ʾ ���ؿؼ�
 @ ��ע�� 
 *********************************************************************************************************/
void disp_hindTFTContrl(u8 pageid, u8 contrlid, u8 isdisplay)
{
	g_retBuf[0] = 0xEE;
	g_retBuf[1] = 0xB1;
	g_retBuf[2] = 0x03;
	g_retBuf[3] = 0x00;
	g_retBuf[4] = pageid;
	g_retBuf[5] = 0;
	g_retBuf[6] = contrlid;
	g_retBuf[7] = isdisplay;
		 
	g_retBuf[8]  = 0xff;
	g_retBuf[9]  = 0xfc;
	g_retBuf[10]  = 0xff;
	g_retBuf[11]  = 0xff; 
	Usart_SendByte(USART2, (char*)g_retBuf, 12); 
}

/***********************************************************************************************************
 @ ���ܣ�����ı��� ��ʾ ���� ��ʾҳ��Ŀؼ�
 @ ��ע�� 
 *********************************************************************************************************/
void disp_hindTFTDisPage(u8 isdisp)
{
	u8 i;  
	for(i = 0; i < 6; i++)
	{ 
		buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeN[i], " "); 
		buildAndSendStr2TFT(TFT_DISP_PAGE, dispNodeP[i], " ");
		buildAndSendStr2TFT(TFT_DISP_PAGE, StatusText[i], " ");
		disp_hindTFTContrl(TFT_DISP_PAGE, CH1Button[i], isdisp);
		disp_hindTFTContrl(TFT_DISP_PAGE, CH2Button[i], isdisp);
	} 
}
 
/**********************************************************************************************************
 @ ����: ɾ��ָ���±��nodeInfo����������Ԫ��ǰ�� 
 @ ����: 0 -5��  = ONEPAGENODE -1 
 @ ����: 
 @ ��ע: �����һҳ��ʾ����һҳʱ ѡ���˿յĲ��ֽ��б���
 *********************************************************************************************************/
void deleteNodeInfo(int index) 
{
	u8 i = 0, offset = 0;  
	char temp[32];
	
	if (index < 0 || index > ONEPAGENODE) {
			printf("deleteNodeInfo faild 1! \r\n");
			dispSetTips("ɾ����������");
			return;
	}
	 
  // ��һҳƫ��0
	offset = (currentTFTV.setPageNodeListF1 -1) * ONEPAGENODE;  // offset + index ��Ϊ��ǰѡ��� 

	// �����һҳ�����
	printf("deleteNodeInfo last page node offset %d g_nodeTotalCount %d index %d mod %d\r\n",offset, g_nodeTotalCount, index, (g_nodeTotalCount % ONEPAGENODE));
	if(currentTFTV.setPageNodeListF1 == currentTFTV.setPageNodeListF2)
	{ 
			printf("frist page \r\n"); 
			if(offset + index >= g_nodeTotalCount){
				dispSetTips("����ȷѡ����Ŀ"); 
				return;
			}
			
			sprintf(temp, "��ɾ���ڵ� %s", nodeInfo[offset + index].baddr.addrStr);
		  dispSetTips(temp); 
			for (i = offset; i < g_nodeTotalCount; ++i) {
					nodeInfo[i + index] = nodeInfo[i + index + 1];
			}
			 
	}
	else// �������һҳ ��������������
	{
		sprintf(temp, "��ɾ���ڵ� %s", nodeInfo[offset + index].baddr.addrStr);
		dispSetTips(temp); 
		for (i = offset; i < g_nodeTotalCount; ++i) {
				nodeInfo[i + index] = nodeInfo[i + index + 1];
		}
	}
		
		g_nodeTotalCount --; 
		TFTDisplayNodeList2SetPag();
		updateRegisterCount();
		updateListPagef2(); 
}
