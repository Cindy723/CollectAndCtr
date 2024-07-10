#include "Application.h"  
#include "init.h"    
#include "flash.h"   
#include "Uart.h"   
#include "oled.h" 

u8 sendNodeDatabuf[12]; 		 // �����ڵ����Ч�غ����� 
u8 g_nodeTotalCount; 				 // ��ǰ�洢���ܽڵ����
 
u16 E_old_reg = 0; 
u16 g_kCount = 0;  						// �������� ����Ƿ��и��� 
u8 g_RequestNodeElecFlag = 1; // �Ƿ���Ҫ����ڵ����ݲ���ʾ
u8 currentRquesNodeIndex = 0; // ��ǰ����ڵ�����
  
/**********************************************************************************************************
 @ ����: ע��ڵ�
 @ ����: 
 @ ����: 
 @ ��ע: �ڵ�idĬ��ʹ��оƬid
 *********************************************************************************************************/
int RegisterNode(UartHMIRecivePackUser *pack)
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


/**********************************************************************************************************
@ ����: ������ҳ�������ʾ��ע��Ľڵ�
@ ����: 
@ ����: 
@ ��ע:  
*********************************************************************************************************/
void HMIDisplayNodeList2SetPag()
{
	u8 nameCtlAddrId[7] = {0, 1, 2, 3, 4, 5, 6};			// ����ڵ��ַID����
	u8 nameCtlNameId[7] = {0, 34, 35, 36, 37, 38, 39};// ����ڵ�����ID����
	u8 i;
	char temp[4];
	
	for(i = 0; i < g_nodeTotalCount; i++){ 
		// ����ÿ��ͣ���ڵ�һҳ
		if(i < 6) // С��һҳʱ�û��鿴�Ÿ���
		{
			buildAndSendDataToHMI(g_retBuf, HMI_PAGE_SETNUM, nameCtlAddrId[i+1], (char*)nodeInfo[i].baddr.addrStr);   
		}
	}
	 
	for(i = 0; i < g_nodeTotalCount; i++){  
		if(i < 6) 
		{
			buildAndSendDataToHMI(g_retBuf, HMI_PAGE_SETNUM, nameCtlNameId[i+1], (char*)nodeInfo[i].name);  
		}
	}
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendDataToHMI(g_retBuf, HMI_PAGE_SETNUM, 54, temp);  
}

/**********************************************************************************************************
 @ ����: ����ť�¼�
 @ ����: 
 @ ����: 
 @ ��ע: ���ص��ı�����Ҳ������
 *********************************************************************************************************/
void HMIButtonEvent()
{
	BoardAddr tempaddr;
	
	/* ��1ҳ�İ�ť */
	if(*uart2HMIPack.Screen_id1 == 1)  
	{
			switch(*uart2HMIPack.Control_id1) // ��һ��CH1����
			{			
				case 1 : /*  ��Ļ-�����ð�ť*/  
				{		
					printf("set button unDisplay\r\n"); 
					g_RequestNodeElecFlag = 0;
				}break;
				case 111:
				{
					printf("contrl node ch1\r\n"); 
					// ���������ҳ���Ӧ����id��Ӧ�İ��� ���´��Ӧ��ַ���ӿ���ָ��
					sendNodeDatabuf[0] = (uart2HMIPack.Status[0] << 7) | POWERCH1;
					buildAndSendDataToNode(g_retBuf, &nodeInfo[0].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
				}break;  
			 case 121: // ��һ��CH2���� 
			 { 
				  printf("contrl node ch2\r\n"); 
				  //relayOpreat(uart2HMIPack.Status[0], POWERCH2);
			 }  break; 
			
			 default: printf("Notfun page1 button %x\r\n", *uart2HMIPack.Control_id1);  break;
		 }
	}
	
	/* ��2ҳ�İ�ť */
	else if(*uart2HMIPack.Screen_id1 == HMI_PAGE_SETNUM) 
	{ 
	 		switch(*uart2HMIPack.Control_id1) 
			{ 
				case 9 : /*  ��Ļ -�� ������ı� */
				{		
			 
				}break; 
				case 16 : /*  ��Ļ -�� �ڵ��Ϸ� */
				{		
					printf("node last page button\r\n");
				}break; 
				case 17 : /*  ��Ļ -�� �ڵ��·� */
				{		
					printf("node nextpage button\r\n");
				}break; 
				case 30 : /* ��Ļ-�������ڵ� */
				{	
					printf("search node button\r\n");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(g_retBuf, &tempaddr, REQUESTADDR, 0, sendNodeDatabuf); 
				}break; 
				case 32 : /* ��Ļ-��ע��ڵ� תCD REGISTERBUTTON*/
				{		
					printf("register node \r\n");   
				}break;
				case 33 : /* ��Ļ-�� �����÷�����ҳ�� */
				{		
					printf("back button Display\r\n"); 
					g_RequestNodeElecFlag = 1;   
				}break;
				default: printf("Notfun page2 button %x\r\n", *uart2HMIPack.Control_id1); break;
			}
	}
	else printf("Notfun Screen %d\r\n", *uart2HMIPack.Screen_id1); 

}

/**********************************************************************************************************
 @ ����: �������������� ����Ҫת����Ҫ�����ݸ��ڵ�
 @ ����: 
 @ ����: 
 @ ��ע: ת��������3 PB11xx PB10tx
 *********************************************************************************************************/
void HMIanalysis()
{  
		if(uart2HMIPack.receiveok)
		{
			uart2HMIPack.receiveok = 0;    
			switch(*uart2HMIPack.cmd0)
			{
				case BUTTON_CMD: 
				{  
					if(*uart2HMIPack.cmd1 == 0x11)
					{
						HMIButtonEvent(); 
					}
				}
				break;
				case USER_CMD: // BUTTON_CMD Ҳ�ᴥ�� ��Ҫʹ��������û�����һ���Դ洢
				{
					//HMIPackUser.
					if(*pHMIPackUser.dataType == REGISTERBUTTON) // ע��ڵ�
					{ 
						RegisterNode(&pHMIPackUser);
						HMIDisplayNodeList2SetPag();
					}
				}
				break;
				default: printf("uart2HMIPack.cmd unkown! %x\r\n", *uart2HMIPack.cmd0); break; 
			}
			memset(uart2HMIPack.dataBuf, 0, sizeof(uart2HMIPack.dataBuf));  
		} 
 
}


/**********************************************************************************************************
 @ ����: ��������Ľڵ�����
 @ ����: 
 @ ����: 
 @ ��ע: 
 *********************************************************************************************************/
void NodeDataAnalysis()
{
		char tempstr[120];
		//static u8 // ģ��ÿ���������Ĳ�ͬ�豸
		u8 nodeIDSimuOffset = 0; 
	
		if(uart3_485Pack.receiveok)
		{
			uart3_485Pack.receiveok = 0;   
		 	switch(*uart3_485Pack.cmd)
			{
				case POWERCONTRL:
				{
					printf("NodeData ret [%02x] POWERCONTRL \r\n", *uart3_485Pack.content);  
					//buildAndSendDataToHMI(g_retBuf, 8, tempstr);
			 	}
				break;
				case REQUESTADDR:
				{
					printf("NodeData ret ADDR\r\n"); 
					sprintf(tempstr, "%02x", *uart3_485Pack.addr0);
					sprintf(&tempstr[2], "%02x", *uart3_485Pack.addr1);
					sprintf(&tempstr[4], "%02x", *uart3_485Pack.addr2 + nodeIDSimuOffset);
					
					// ��ȥ��ʾ����str �洢��hex
					nodeInfo[g_nodeTotalCount].baddr.addr[0] = *uart3_485Pack.addr0;
					nodeInfo[g_nodeTotalCount].baddr.addr[1] = *uart3_485Pack.addr1;
					nodeInfo[g_nodeTotalCount].baddr.addr[2] = *uart3_485Pack.addr2;
					nodeInfo[g_nodeTotalCount].baddr.type = &nodeInfo[g_nodeTotalCount].baddr.addr[0];
					 
					buildAndSendDataToHMI(g_retBuf, HMI_PAGE_SETNUM, 8, tempstr);
					nodeIDSimuOffset ++;
				}
				break;
				case REQUESTELEC:
				{
					printf("NodeData Ret REQUESTELEC ");  printHex(uart3_485Pack.addr0, 3);
					// ���ﷵ�ظ�����ע��Ľڵ���������
					memcpy(&nodeElecInfo[currentRquesNodeIndex].baddr, uart3_485Pack.addr0, 3);
					if(*uart3_485Pack.addr0 == 0xac)
					{
						nodeElecInfo[currentRquesNodeIndex].vTotal = 
								(float)(((uart3_485Pack.content[0] << 16) | (uart3_485Pack.content[1] << 8) | uart3_485Pack.content[2]) * 0.01);
						printf("vTotal %f\r\n", nodeElecInfo[currentRquesNodeIndex].vTotal); 

						nodeElecInfo[currentRquesNodeIndex].i1 = 
								(float)(((uart3_485Pack.content[3] << 8) | uart3_485Pack.content[4]) * 0.01);
						printf("i1 %f\r\n", nodeElecInfo[currentRquesNodeIndex].i1);

					}
					else if(*uart3_485Pack.addr0 == 0xdc)
					{
						nodeElecInfo[currentRquesNodeIndex].vTotal = 
								(float)(((uart3_485Pack.content[0] << 8) | uart3_485Pack.content[1]) * 0.01);
						printf("vTotal %f\r\n", nodeElecInfo[currentRquesNodeIndex].vTotal); 

						nodeElecInfo[currentRquesNodeIndex].i1 = 
								(float)(((uart3_485Pack.content[2] << 8) | uart3_485Pack.content[3]) * 0.01);
						printf("i1 %f\r\n", nodeElecInfo[currentRquesNodeIndex].i1); 

						nodeElecInfo[currentRquesNodeIndex].i2 = 
								(float)(((uart3_485Pack.content[4] << 8) | uart3_485Pack.content[5]) * 0.01);
						printf("i2 %f\r\n", nodeElecInfo[currentRquesNodeIndex].i2);
 
						currentRquesNodeIndex ++; 
						g_RequestNodeElecFlag = 1; // ����������һ������
					}
				}
			  break;
				default: printf("uart3_485Pack.cmd unkown! %x\r\n", *uart3_485Pack.cmd); break; 
			}
			memset(uart3_485Pack.dataBuf, 0, sizeof(uart3_485Pack.dataBuf));  
		}
}

/**********************************************************************************************************
 @ ���ܣ� �����̵���
 @ ��ڣ� ���� ͨ����1 2
 *********************************************************************************************************/
void relayOpreat(u8 sw, u8 ch)
{
	printf("relayOpreat sw %d ch %d \r\n", sw, ch);
	if(ch == POWERCH1)
	{
		if(sw == POWER_ON)
		{
			printf("HC1 ON \r\n");
			OLED_P6x8Str(0,1,(u8*)"CH1:ON ",0); 
			RELAY1 = POWER_ON;  
			LEDContrl(LED2PIN, LEDON);
		}
		else
		{
			printf("CH1 OFF\r\n");
			OLED_P6x8Str(0,1,(u8*)"CH1:OFF",0); 
			RELAY1 = POWER_OFF; 
			LEDContrl(LED2PIN, LEDOFF);
		}
	}
	else if(ch == POWERCH2)
	{
		if(sw == POWER_ON)
		{
			printf("CH2 ON \r\n");
			OLED_P6x8Str(52,1,(u8*)"CH2:ON ",0); 
			RELAY2 = POWER_ON; 
			LEDContrl(LED3PIN, LEDON);
		}
		else 
		{
			printf("CH2 OFF\r\n");
			OLED_P6x8Str(52,1,(u8*)"CH2:OFF",0); 
			RELAY2 = POWER_OFF; 
			LEDContrl(LED3PIN, LEDOFF);
		}
	}

}


/**********************************************************************************************************
 @ ���ܣ� ��Ҫ������еĳ���
 @ ��ڣ�  
 *********************************************************************************************************/
void IntervalProc()
{ 
		if(g_LEDBling_kCount > 1 && g_LEDBling_kCount < 800)
		{
			LEDContrl(LEDRUNPIN, LEDOFF);  
			OLED_P6x8Str(120,6,(u8*)" ",0);
		}
		else if(g_LEDBling_kCount > 830)
		{
			g_LEDBling_kCount = 0;
			LEDContrl(LEDRUNPIN, LEDON);  
			OLED_P6x8Str(120,6,(u8*)" ",1);  
		}
		
		// 1000ms ����һ���ڵ�����
		if(g_RequestNodeCount >= 1000)
		{ 
			if(g_RequestNodeElecFlag && strcmp((char*)nodeInfo[currentRquesNodeIndex].name, "") )
			{  
				buildAndSendDataToNode(g_retBuf, &nodeInfo[currentRquesNodeIndex].baddr, REQUESTELEC, 0, sendNodeDatabuf); 
				//g_RequestNodeElecFlag = 0;	 // �յ��ڵ����ݺ�������´�����
			}
			g_RequestNodeCount = 0; 
		}

}


/**********************************************************************************************************
 @ ���ܣ� ��д�ڵ����ͺ͵�ַ��flash
 @ ��ڣ� 0�� 1д
 *********************************************************************************************************/
void rwTypeAndAddr(u8 rw, BoardAddr *baddr)
{
	if(rw == 0) // ��ȡ
	{
		rFlashData(baddr->addr, 3, PARAM_SAVE_ADDR_BASE);
		printf("read board addr: \r\n");
		printHex(baddr->addr, 3);
	}
	else if(rw == 1)
	{
		flash_erase(1024 , PARAM_SAVE_ADDR_BASE); 
		printf("write board addr: \r\n");
		printHex(baddr->addr, 3);
		wFlashData(baddr->addr, 3, PARAM_SAVE_ADDR_BASE); 
	}
}

  
/**********************************************************************************************************
 @ ���ܣ�HLW8023�������� 
 @ ���أ�
 @ ��ע��
 *********************************************************************************************************/
double EconAnalysis(u8 *pbuf)
{  
	unsigned int PF_COUNT = 0, PF = 0, PP_REG = 0;
	double E_con = 0;
	
	if((pbuf[20]&0x80)!=E_old_reg)//�ж����ݸ��¼Ĵ������λ��û�з�ת
	{
		g_kCount++;
		E_old_reg=pbuf[20]&0x80;
	}
	
	PP_REG=pbuf[14]*65536+pbuf[15]*256+pbuf[16];	//���㹦�ʲ����Ĵ�  
	PF=(g_kCount*65536)+(pbuf[21]*256)+pbuf[22];		//�������õ���������
	PF_COUNT=((100000*3600)/(PP_REG*1.88))*10000;	//����1�ȵ��Ӧ����������
	E_con=((PF*10000)/PF_COUNT)/10000.0;//�������õ��� 
	//printf("PF %d  PF_COUNT %d  ���õ�����%0.4f��\r\n",PF,PF_COUNT,E_con); 
	 
 
	return E_con;
}
