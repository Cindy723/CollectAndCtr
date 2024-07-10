#include "TFTCommunic.h"
#include "Uart.h"
#include "Tools.h"

u8 g_nodeTotalCount; 				  // 当前存储的总节点个数
 			
u8 g_RequestNodeElecFlag = 1; // 是否需要请求节点数据并显示
u8 sendNodeDatabuf[12]; 		  // 发给节点的有效载荷数据

int RegisterNode(UartTFTRecivePackUser *pack);

/**********************************************************************************************************
@ 功能: 在设置页面更新显示已注册的节点
@ 参数: 
@ 返回: 
@ 备注:  
*********************************************************************************************************/
void TFTDisplayNodeList2SetPag()
{
	u8 nameCtlAddrId[7] = {0, 1, 2, 3, 4, 5, 6};			// 界面节点地址ID索引
	u8 nameCtlNameId[7] = {0, 34, 35, 36, 37, 38, 39};// 界面节点名字ID索引
	u8 i;
	char temp[4];
	
	for(i = 0; i < g_nodeTotalCount; i++){ 
		// 假设每次停留在第一页
		if(i < 6) // 小于一页时用户查看才更新
		{
			buildAndSendDataToTFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlAddrId[i+1], (char*)nodeInfo[i].baddr.addrStr);   
		}
	}
	 
	for(i = 0; i < g_nodeTotalCount; i++){  
		if(i < 6) 
		{
			buildAndSendDataToTFT(g_retBuf, TFT_PAGE_SETNUM, nameCtlNameId[i+1], (char*)nodeInfo[i].name);  
		}
	}
	sprintf(temp, "%d", g_nodeTotalCount);
	buildAndSendDataToTFT(g_retBuf, TFT_PAGE_SETNUM, 54, temp);  
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
	
	/* 第1页的按钮 */
	if(*uart2TFTPack.Screen_id1 == 1)  
	{
			switch(*uart2TFTPack.Control_id1) // 第一行CH1开关
			{			
				case 1 : /*  屏幕-》设置按钮*/  
				{		
					printf("set button unDisplay\r\n"); 
					g_RequestNodeElecFlag = 0;
				}break;
				case 111:
				{
					printf("contrl node ch1\r\n"); 
					// 在这这里找出对应开关id对应的板子 再下达对应地址板子控制指令
					sendNodeDatabuf[0] = (uart2TFTPack.Status[0] << 7) | POWERCH1;
					buildAndSendDataToNode(g_retBuf, &nodeInfo[0].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
				}break;  
			 case 121: // 第一行CH2开关 
			 { 
				  printf("contrl node ch2\r\n"); 
				  //relayOpreat(uart2TFTPack.Status[0], POWERCH2);
			 }  break; 
			
			 default: printf("Notfun page1 button %x\r\n", *uart2TFTPack.Control_id1);  break;
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
				case 16 : /*  屏幕 -》 节点上翻 */
				{		
					printf("node last page button\r\n");
				}break; 
				case 17 : /*  屏幕 -》 节点下翻 */
				{		
					printf("node nextpage button\r\n");
				}break; 
				case 30 : /* 屏幕-》搜索节点 */
				{	
					printf("search node button\r\n");
					tempaddr.addr[0] = 0xdc; tempaddr.addr[1] = 0xff; tempaddr.addr[2] = 0xff;
					buildAndSendDataToNode(g_retBuf, &tempaddr, REQUESTADDR, 0, sendNodeDatabuf); 
				}break; 
				case 32 : /* 屏幕-》注册节点 转CD REGISTERBUTTON*/
				{		
					printf("register node \r\n");   
				}break;
				case 33 : /* 屏幕-》 从设置返回主页面 */
				{		
					printf("back button Display\r\n"); 
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
					if(*pTFTPackUser.dataType == REGISTERBUTTON) // 注册节点
					{ 
						RegisterNode(&pTFTPackUser);
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
 @ 功能: 注册节点
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


