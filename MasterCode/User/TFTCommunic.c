#include "TFTCommunic.h"
#include "Uart.h"
#include "Tools.h"

u8 g_nodeTotalCount; 				  // 当前存储的总节点个数
 			
u8 g_RequestNodeElecFlag = 1; // 是否需要请求节点数据并显示
u8 sendNodeDatabuf[12]; 		  // 发给节点的有效载荷数据

u8 g_currentPage;   			// 当前页面
u8 g_currentNodeListPage; // 设置界面下的分子
u8 g_currentDispListPage; // 显示界面下的分子

u8 g_currentRquesNodeIndex = 0; // 当前请求节点索引

int RegisterNode(UartTFTRecivePackUser *pack);

/**********************************************************************************************************
@ 功能: 在设置页面更新显示已注册的节点
@ 参数: 
@ 返回: 
@ 备注: 需要在此之前拿到页面分子
*********************************************************************************************************/
void TFTDisplayNodeList2SetPag()
{
	u8 nameCtlAddrId[7] = {0, 1, 2, 3, 4, 5, 6};			// 界面节点地址ID索引
	u8 nameCtlNameId[7] = {0, 34, 35, 36, 37, 38, 39};// 界面节点名字ID索引
	u8 i;
	char temp[4]; 
	u8 offset = (g_currentNodeListPage - 1) * ONEPAGENODE;
	printf("TFTDisplayNodeList2SetPag offset %d", offset);
	 
	// 根据当前分子显示当前页面 ONEPAGENODE 条
	for(i = 0; i < 6; i++){   
		buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlAddrId[i + 1], (char*)nodeInfo[offset + i].baddr.addrStr);  
	
	}
	  
	for(i = 0; i < 6; i++){   
		buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlNameId[i + 1], (char*)nodeInfo[offset + i].name);  
	} 
	
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendStr2TFT(g_retBuf, TFT_PAGE_SETNUM, 54, temp);  
}

/**********************************************************************************************************
 @ 功能: 处理按钮事件
 @ 参数: 
 @ 返回: 
 @ 备注: 返回的文本数据也在这里
 *********************************************************************************************************/
void TFTButtonEvent()
{
	BoardAddr tempaddr;
	u8 ButtonCH1Start = 111;
	u8 offset = (g_currentDispListPage - 1) * ONEPAGENODE;
	
	/* 第1页的按钮 */
	if(*uart2TFTPack.Screen_id1 == 1)  
	{
//			switch(*uart2TFTPack.Control_id1) // 第一行CH1开关
//			{			
//				case 1 : /*  屏幕-》设置按钮*/  
//				{		
//					printf("set button unDisplay\r\n"); 
//					g_RequestNodeElecFlag = 0;
//					g_currentPage = TFT_PAGE_SETNUM;
//				}break;
//				case 111:
//				{
//					printf("contrl node ch1\r\n"); 
//					sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
//					buildAndSendDataToNode(g_retBuf, &nodeInfo[0].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
//				}break;  
//			 case 121: // 第一行CH2开关 
//			 { 
//				  printf("contrl node ch2\r\n"); 
//				  //relayOpreat(uart2TFTPack.Status[0], POWERCH2);
//			 }  break; 
//			
//			 default: printf("Notfun page1 button %x\r\n", *uart2TFTPack.Control_id1);  break;			
//		 }

			if(*uart2TFTPack.Control_id1 == 1) // 设置按钮按下
			{
					printf("set button unDisplay\r\n"); 
					g_RequestNodeElecFlag = 0;
					g_currentPage = TFT_PAGE_SETNUM;
			}
			else if((*uart2TFTPack.Control_id1 == 11) || (*uart2TFTPack.Control_id1 == 12))	// 切换了页面 显示对应页面电能参数 还没加开关状态
			{  
					dispElec2TFT(g_retBuf, nodeInfo);  
			} 
			else if((*uart2TFTPack.Control_id1 > 110) &&  (*uart2TFTPack.Control_id1 < 117)) // 第一列开关按下
			{
				printf("power ch1 change\r\n");  
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
			  buildAndSendDataToNode(g_retBuf, &nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH1Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
			}
			else if((*uart2TFTPack.Control_id1 > 120) &&  (*uart2TFTPack.Control_id1 < 127)) // 第二列开关按下
			{ 
				printf("power ch2 change\r\n"); 
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH2;
			  buildAndSendDataToNode(g_retBuf, &nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH1Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
			}

	}
	
	/* 第2页的按钮 */
	else if(*uart2TFTPack.Screen_id1 == TFT_PAGE_SETNUM) 
	{ 
	 		switch(*uart2TFTPack.Control_id1) 
			{ 
				case 9 : /*  屏幕 -》 请求的文本 */
				{		 
				}break; 
				case 16 : /*  屏幕 -》 设置界面节点上翻 */
				{		 
				 TFTDisplayNodeList2SetPag(); 
				}break; 
				case 17 : /*  屏幕 -》 设置界面节点下翻 */
				{			
				 TFTDisplayNodeList2SetPag();
				}break;
				case 30 : /* 屏幕-》搜索节点 */
				{	
					printf("search node button\r\n");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(g_retBuf, &tempaddr, REQUESTADDR, 0, sendNodeDatabuf); 
				}break; 
				case 32 : /* 屏幕-》注册节点 转CD USER_CMD_REGISTER*/
				{		
					printf("register node \r\n");   
				}break;
				case 33 : /* 屏幕-》 从设置返回主页面 */
				{		
					printf("back button Display\r\n"); 
					
					g_currentPage = TFT_DISP_SETNUM;
					g_currentRquesNodeIndex = 0;
					g_RequestNodeElecFlag = 1;   
				}break;
				default: printf("Notfun page2 button %x\r\n", *uart2TFTPack.Control_id1); break;
			}
	}
	else printf("Notfun Screen %d\r\n", *uart2TFTPack.Screen_id1); 

}

/**********************************************************************************************************
 @ 功能: 解析串口屏数据 按需要转发需要的数据给节点
 @ 参数: 
 @ 返回: 
 @ 备注: 转发给串口3 PB11xx PB10tx
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
				case USER_CMD: // BUTTON_CMD 也会触发 主要使用这里的用户数据一次性存储
				{
					//TFTPackUser.
					if(*pTFTPackUser.dataType == USER_CMD_REGISTER) // 注册节点
					{ 
						RegisterNode(&pTFTPackUser);
						TFTDisplayNodeList2SetPag();
					}
					else if(*pTFTPackUser.dataType == USER_CMD_NODE_PAGECHANGE) 
					{ 
							g_currentNodeListPage = pTFTPackUser.data[0] - '0';
							printf("USER_CMD -》 nodePageChange %d \r\n", g_currentNodeListPage); 
					}
					else if(*pTFTPackUser.dataType == USER_CMD_DISPP_AGECHANGE) 
					{ 
							g_currentDispListPage = pTFTPackUser.data[0] - '0';
							printf("USER_CMD -》 dispPageChange %d \r\n", g_currentDispListPage); 
					}
					
				}
				break;
				default: printf("uart2TFTPack.cmd unkown! %x\r\n", *uart2TFTPack.cmd0); break; 
			}
			memset(uart2TFTPack.dataBuf, 0, sizeof(uart2TFTPack.dataBuf));  
		}
 
}
 
/**********************************************************************************************************
 @ 功能: TFT->> 注册节点
 @ 参数: 
 @ 返回: 
 @ 备注: 节点id默认使用芯片id
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
	for(i = 0; i < g_nodeTotalCount; i++){	// 打印注册后的项 
		printf("Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	}
	return  g_nodeTotalCount;
}


/***********************************************************************************************************
 @ 功能：更新TFT文本数据 
 @ 入口： 
 *********************************************************************************************************/
void buildAndSendStr2TFT(u8 *pbuf, u8 pagid, u8 ctrlid, char* str)
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
 @ 功能：显示电能数据到TFT
 @ 备注： 
 *********************************************************************************************************/
void dispElec2TFT(u8 *pbuf, NodeInfo *info)
{
	char str[240], i;
	u8 offset = (g_currentDispListPage - 1) * ONEPAGENODE;
	printf("dispElec2TFT offset %d", offset);
  
	for(i = 0; i < 6; i++)
	{ 
		buildAndSendStr2TFT(g_retBuf, TFT_DISP_SETNUM, i, (char*)nodeInfo[offset + i].name);
		if(nodeInfo[offset + i].baddr.addr[0] == 0xdc)
		{ 
			sprintf(str, "DC: %.2f V,  CH1 %.2f A,  CH2 %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1, nodeInfo[offset + i].eInfo.i2);
			buildAndSendStr2TFT(g_retBuf, TFT_DISP_SETNUM, 101+i, str);  
		}
		else if(nodeInfo[offset + i].baddr.addr[0] == 0xac)
		{ 
	    sprintf(str, "AC: %.2f V, %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1);
			buildAndSendStr2TFT(g_retBuf, TFT_DISP_SETNUM, 101+i, str);  
		}
	}
	  
}
