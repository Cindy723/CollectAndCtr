#include "TFTCommunic.h"
#include "Uart.h"
#include "Tools.h"
#include "UartSendQueue.h"
#include "flash.h"

u8 g_nodeTotalCount; 				  // 当前存储的总节点个数
 			
u8 g_RequestNodeElecFlag = 1; // 是否需要请求节点数据并显示
u8 sendNodeDatabuf[12]; 		  // 发给节点的有效载荷数据

u8 g_currentPage = TFT_DISPNODE_NUM; // 当前页面
u8 g_currentNodeListPage = 1; // 设置界面下的分子
u8 g_currentDispListPage = 1; // 显示界面下的分子

u8 g_currentRquesNodeIndex = 0; // 当前请求节点索引

// 显示页面id索引
u8 dispNodeN[6] = {101, 102, 103, 104, 105, 106};
u8 dispNodeP[6] = {5, 6, 7, 8, 9, 10};
u8 CH1Button[6] = {111, 112, 113, 114, 115, 116};
u8 CH2Button[6] = {121, 122, 123, 124, 125, 126};
u8 StatusText[6] = {15, 16, 17, 19, 24, 26};

int RegisterNode(UartTFTRecivePackUser *pack);
void setListPagef2(u8 page);
void disp_hindTFTDisPage(u8 isdisp); 

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
		buildAndSendStr2TFT(TFT_PAGE_SETNUM, nameCtlAddrId[i + 1], (char*)nodeInfo[offset + i].baddr.addrStr);  
	
	}
	  
	for(i = 0; i < 6; i++){   
		buildAndSendStr2TFT(TFT_PAGE_SETNUM, nameCtlNameId[i + 1], (char*)nodeInfo[offset + i].name);  
	} 
	
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendStr2TFT(TFT_PAGE_SETNUM, 54, temp);  
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
	u8 ButtonCH2Start = 121;
	u8 offset = (g_currentDispListPage - 1) * ONEPAGENODE;
	
	/* 第1页的按钮 */
	if(*uart2TFTPack.Screen_id1 == 1)  
	{
  
			if(*uart2TFTPack.Control_id1 == 1) // 设置按钮按下
			{
					printf("set button unDisplay\r\n"); 
				  dispSetTips("  ");
					TFTDisplayNodeList2SetPag();
					g_RequestNodeElecFlag = 0;
					g_currentPage = TFT_PAGE_SETNUM;
			}
			if(*uart2TFTPack.Control_id1 == 2) // 显示页分子返回
			{
					g_currentDispListPage = *uart2TFTPack.Subtype_or_Conten - '0'; 
					printf("get DisplayPage f1 %d\r\n", g_currentDispListPage);  
			}
			else if((*uart2TFTPack.Control_id1 == 11) || (*uart2TFTPack.Control_id1 == 12))	// 切换了页面 显示对应页面电能参数 还没加开关状态
			{  
					disp_hindTFTDisPage(0);
					dispElec2TFT(nodeInfo);  
			} 
			else if((*uart2TFTPack.Control_id1 > 110) &&  (*uart2TFTPack.Control_id1 < 117)) // 第一列开关按下
			{
				printf("power ch1 change\r\n");  
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
			  buildAndSendDataToNode(&nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH1Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
			}
			else if((*uart2TFTPack.Control_id1 > 120) &&  (*uart2TFTPack.Control_id1 < 127)) // 第二列开关按下
			{ 
				printf("power ch2 change\r\n"); 
			  sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH2;
			  buildAndSendDataToNode(&nodeInfo[offset + *uart2TFTPack.Control_id1 - ButtonCH2Start].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
			}

	}
	
	/* 第2页的按钮 */
	else if(*uart2TFTPack.Screen_id1 == TFT_PAGE_SETNUM) 
	{ 
	 		switch(*uart2TFTPack.Control_id1) 
			{  			 
				case 21: /*  屏幕 -》 返回的分子 */
				{
					g_currentNodeListPage = *uart2TFTPack.Subtype_or_Conten - '0'; 
					printf("get SetPage f1 %d\r\n", g_currentNodeListPage);  
				}
	      break;
				case 16 : /*  屏幕 -》 设置界面节点上翻 */
				{		  
				 dispSetTips("上一页");
				}break; 
				case 17 : /*  屏幕 -》 设置界面节点下翻 */
				{			
				 dispSetTips("下一页");
				}break;
				case 30 : /* 屏幕-》搜索节点 */
				{	
					printf("search node button\r\n");
					buildAndSendStr2TFT(TFT_PAGE_SETNUM, 8, " ");				
				  dispSetTips("搜索..");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(&tempaddr, REQUESTADDR, 0, sendNodeDatabuf); 
				}break; 
				case 32 : /* 屏幕-》注册节点 转CD USER_CMD_REGISTER*/
				{		
					printf("register node \r\n"); 	
				  dispSetTips("注册节点..");
				}break;
				case 33 : /* 屏幕-》 从设置返回主页面 */
				{		
					printf("back button Display\r\n");   
				  dispSetTips("  ");
					
					g_currentPage = TFT_DISPNODE_NUM;
					g_currentRquesNodeIndex = 0;
					g_RequestNodeElecFlag = 1;   
					disp_hindTFTDisPage(0);
				}break;
				default: printf("Notfun page2 button %d\r\n", *uart2TFTPack.Control_id1); break;
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
							TFTDisplayNodeList2SetPag();
					}
					else if(*pTFTPackUser.dataType == USER_CMD_DISPP_AGECHANGE) 
					{ 
							g_currentDispListPage = pTFTPackUser.data[0] - '0';
							printf("USER_CMD -》 dispPageChange %d \r\n", g_currentDispListPage); 
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
 @ 功能: TFT->> 注册节点
 @ 参数: 
 @ 返回: 
 @ 备注: 节点id默认使用芯片id
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
//				dispSetTips("节点已经被注册!");
//				return -1;
//		 }
//	}
	
	if(g_nodeTotalCount > MAX_NODE)
	{
		sprintf(temp, "节点数超过 %d 个!", MAX_NODE);
		dispSetTips(temp);
	  return -1; 
	}
	
	// id 不同即可
		for(; i < g_nodeTotalCount; i++){ 
	   if(0 == strcmp((char*)nodeTemp.baddr.addrStr, (char*)nodeInfo[i].baddr.addrStr))
		 {
				printf("USER_CMD-->> CMD_0xC1 id or name already registered!\r\n");
				dispSetTips("节点已经被注册!");
				return -1;
		 }
	}
	
	strcpy((char*)nodeInfo[g_nodeTotalCount].baddr.addrStr, (char*)(nodeTemp.baddr.addrStr)); 
	strcpy((char*)nodeInfo[g_nodeTotalCount].name, (char*)(nodeTemp.name));
	StrToHexByte(nodeInfo[g_nodeTotalCount].baddr.addrStr, nodeInfo[g_nodeTotalCount].baddr.addr);
	nodeInfo[g_nodeTotalCount].baddr.type = &nodeInfo[g_nodeTotalCount].baddr.addr[0];
 
	g_nodeTotalCount ++;
	printf("Node Count %d List: \r\n", g_nodeTotalCount);
	for(i = 0; i < g_nodeTotalCount; i++){	// 打印注册后的项 
		printf("Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	}
	
  dispSetTips("节点注册成功!");
	if(g_nodeTotalCount > 6)
	{
	 if(g_nodeTotalCount%ONEPAGENODE != 0)
	 setListPagef2((g_nodeTotalCount/6) + 1);
	}else  setListPagef2(1);
	 
	nodeInfo[0].totalNode = g_nodeTotalCount;
	
  flash_erase(PARAM_MAX_SIZE, PARAM_SAVE_ADDR_BASE);
	
	//wFlashData((uint8_t*)nodeInfo, sizeof(nodeInfo), PARAM_SAVE_ADDR_BASE);
	//rFlashData((uint8_t*)nodeInfo, sizeof(nodeInfo), PARAM_SAVE_ADDR_BASE);
	
		for(i = 0; i < g_nodeTotalCount; i++){	// 打印注册后的项 
		printf("--- Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	} 
	
	
	return  g_nodeTotalCount;
}


/***********************************************************************************************************
 @ 功能：更新TFT文本数据 
 @ 入口： 
 *********************************************************************************************************/
void buildAndSendStr2TFT(u8 pagid, u8 ctrlid, char* str)
{
	int strLen = 0;
	
	// 帧头
	g_retBuf[0] = 0xee;
	
	// 指令
	g_retBuf[1] = 0xB1;
	g_retBuf[2] = 0x10;
	
	// 页面id  
	g_retBuf[3] = 0;
	g_retBuf[4] = pagid;
	
	// 控件id
	g_retBuf[5] = 0;
	g_retBuf[6] = ctrlid;
	
	// string
	strLen = strlen(str)+1;
	memcpy(&g_retBuf[7], str, strLen);
	g_retBuf[strLen + 6]  = '\0';
	// 帧尾
	g_retBuf[strLen + 7]  = 0xff;
	g_retBuf[strLen + 8]  = 0xfc;
	g_retBuf[strLen + 9]  = 0xff;
	g_retBuf[strLen + 10] = 0xff;
	
	// 发送数据 
	//printf("uplen %d : %s \r\n", strLen, str);
	//printf("build : ");
	//printHex(g_retBuf, strLen + 11);  
	Usart_SendByte(USART2, (char*)g_retBuf, strLen + 11); 
}


/***********************************************************************************************************
 @ 功能：获取TFT文本数据 ,EE B1 11 00 02 00 09 FF FC FF FF
 @ 备注：返回 EE B1 11 00 02 00 09 (11) [4E 61 6D 65 68 68 68 68] 00 FF FC FF FF
				 已通过队列
 *********************************************************************************************************/
void getTFTText(u8 pagid, u8 ctrlid)
{
	int strLen = 0;
	int totalLen = 0;
	Message msg;
	
	// 帧头
	g_retBuf[0] = 0xee;
	
	// 指令
	g_retBuf[1] = 0xB1;
	g_retBuf[2] = 0x11;
	
	// 页面id  
	g_retBuf[3] = 0;
	g_retBuf[4] = pagid;
	
	// 控件id
	g_retBuf[5] = 0;
	g_retBuf[6] = ctrlid;
	 
	// 帧尾
	g_retBuf[strLen + 7]  = 0xff;
	g_retBuf[strLen + 8]  = 0xfc;
	g_retBuf[strLen + 9]  = 0xff;
	g_retBuf[strLen + 10] = 0xff;
	totalLen = strLen + 11;
	
	// 发送数据  
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
 @ 功能：显示电能数据到TFT
 @ 备注： 
 *********************************************************************************************************/
void dispElec2TFT(NodeInfo *info)
{
	char str[240], i; 
	u8 offset = (g_currentDispListPage - 1) * ONEPAGENODE;
	u8 toDisplay = ONEPAGENODE;

	// 计算剩余条目数
	if(g_nodeTotalCount == 0){
		toDisplay = 0;
	}
	else
	{
		u8 remaining = g_nodeTotalCount - offset;
		if (remaining < ONEPAGENODE){
			toDisplay = remaining;
		}
	} 

	// printf("dispElec2TFT offset %d toDisplay %d \r\n", offset, toDisplay);  
	// 显示当前页的数据
	for(i = 0; i < toDisplay; i++)
	{   
		
		if(nodeInfo[offset + i].needDispElec == 1)
		{
			disp_hindTFTContrl(TFT_DISPNODE_NUM, CH1Button[i], 1);
			disp_hindTFTContrl(TFT_DISPNODE_NUM, CH2Button[i], 1);
			buildAndSendStr2TFT(TFT_DISPNODE_NUM, StatusText[i], "正常");
			
			if(nodeInfo[offset + i].baddr.addr[0] == 0xdc)
			{ 
				buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				sprintf(str, "DC: %.2f V,  CH1: %.2f A,  CH2: %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1, nodeInfo[offset + i].eInfo.i2);
				buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeN[i], str);  
			}
			else if(nodeInfo[offset + i].baddr.addr[0] == 0xac)
			{
				buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeP[i], (char*)nodeInfo[offset + i].name);
				sprintf(str, "AC: %.2f V, %.2f A", nodeInfo[offset + i].eInfo.vTotal,  nodeInfo[offset + i].eInfo.i1);
				buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeN[i], str);  
			}
			
			nodeInfo[offset + i].needDispElec = 0;
		}
	}
}



/***********************************************************************************************************
 @ 功能：设置界面显示提示信息
 @ 备注： 
 *********************************************************************************************************/
void dispSetTips(char* tip)
{
  buildAndSendStr2TFT(2, 47, tip);
}


/***********************************************************************************************************
 @ 功能：设置分母
 @ 备注： 
 *********************************************************************************************************/
void setListPagef2(u8 page)
{
	char temp[6];
	sprintf(temp, "%d", page); 
	printf("updateListPage f2 %s\r\n", temp);
  buildAndSendStr2TFT(1, 22, temp);
  buildAndSendStr2TFT(2, 22, temp);
}

/***********************************************************************************************************
 @ 功能：获取分子
 @ 备注：切换显示列表时候会自动更新 但在显示界面时 由于需要处理节点 
					导致有时候数据被丢掉 所以定期主动请求显示界面的分子
 *********************************************************************************************************/
void getListPagef1()
{
	getTFTText(1, 2);
	// getTFTText(2, 2);
}
 
/***********************************************************************************************************
 @ 功能：复位TFT
 @ 备注： 
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
 @ 功能：显示 隐藏控件
 @ 备注： 
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
 @ 功能：显示 隐藏 显示页面的控件
 @ 备注： 
 *********************************************************************************************************/
void disp_hindTFTDisPage(u8 isdisp)
{
	u8 i;  
	for(i = 0; i < 6; i++)
	{ 
		buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeN[i], " "); 
		buildAndSendStr2TFT(TFT_DISPNODE_NUM, dispNodeP[i], " ");
		buildAndSendStr2TFT(TFT_DISPNODE_NUM, StatusText[i], " ");
		disp_hindTFTContrl(TFT_DISPNODE_NUM, CH1Button[i], isdisp);
		disp_hindTFTContrl(TFT_DISPNODE_NUM, CH2Button[i], isdisp);
	} 
}

