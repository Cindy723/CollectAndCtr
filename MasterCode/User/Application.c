#include "Application.h"  
#include "init.h"    
#include "flash.h"   
#include "Uart.h"   
#include "oled.h" 

u8 sendNodeDatabuf[12]; 		 // 发给节点的有效载荷数据 
u8 g_nodeTotalCount; 				 // 当前存储的总节点个数
 
u16 E_old_reg = 0; 
u16 g_kCount = 0;  						// 电量脉冲 监测是否有更新 
u8 g_RequestNodeElecFlag = 1; // 是否需要请求节点数据并显示
u8 currentRquesNodeIndex = 0; // 当前请求节点索引
  
/**********************************************************************************************************
 @ 功能: 注册节点
 @ 参数: 
 @ 返回: 
 @ 备注: 节点id默认使用芯片id
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
	for(i = 0; i < g_nodeTotalCount; i++){	// 打印注册后的项 
		printf("Id index %d, ID [%s], Name [%s]\r\nhexID: ", i, nodeInfo[i].baddr.addrStr, nodeInfo[i].name);
		printHex(nodeInfo[i].baddr.addr, 3);
	}
	return  g_nodeTotalCount;
}


/**********************************************************************************************************
@ 功能: 在设置页面更新显示已注册的节点
@ 参数: 
@ 返回: 
@ 备注:  
*********************************************************************************************************/
void HMIDisplayNodeList2SetPag()
{
	u8 nameCtlAddrId[7] = {0, 1, 2, 3, 4, 5, 6};			// 界面节点地址ID索引
	u8 nameCtlNameId[7] = {0, 34, 35, 36, 37, 38, 39};// 界面节点名字ID索引
	u8 i;
	char temp[4];
	
	for(i = 0; i < g_nodeTotalCount; i++){ 
		// 假设每次停留在第一页
		if(i < 6) // 小于一页时用户查看才更新
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
 @ 功能: 处理按钮事件
 @ 参数: 
 @ 返回: 
 @ 备注: 返回的文本数据也在这里
 *********************************************************************************************************/
void HMIButtonEvent()
{
	BoardAddr tempaddr;
	
	/* 第1页的按钮 */
	if(*uart2HMIPack.Screen_id1 == 1)  
	{
			switch(*uart2HMIPack.Control_id1) // 第一行CH1开关
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
					sendNodeDatabuf[0] = (uart2HMIPack.Status[0] << 7) | POWERCH1;
					buildAndSendDataToNode(g_retBuf, &nodeInfo[0].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
				}break;  
			 case 121: // 第一行CH2开关 
			 { 
				  printf("contrl node ch2\r\n"); 
				  //relayOpreat(uart2HMIPack.Status[0], POWERCH2);
			 }  break; 
			
			 default: printf("Notfun page1 button %x\r\n", *uart2HMIPack.Control_id1);  break;
		 }
	}
	
	/* 第2页的按钮 */
	else if(*uart2HMIPack.Screen_id1 == HMI_PAGE_SETNUM) 
	{ 
	 		switch(*uart2HMIPack.Control_id1) 
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
				default: printf("Notfun page2 button %x\r\n", *uart2HMIPack.Control_id1); break;
			}
	}
	else printf("Notfun Screen %d\r\n", *uart2HMIPack.Screen_id1); 

}

/**********************************************************************************************************
 @ 功能: 解析串口屏数据 按需要转发需要的数据给节点
 @ 参数: 
 @ 返回: 
 @ 备注: 转发给串口3 PB11xx PB10tx
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
				case USER_CMD: // BUTTON_CMD 也会触发 主要使用这里的用户数据一次性存储
				{
					//HMIPackUser.
					if(*pHMIPackUser.dataType == REGISTERBUTTON) // 注册节点
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
 @ 功能: 解析请求的节点数据
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void NodeDataAnalysis()
{
		char tempstr[120];
		//static u8 // 模拟每次搜索到的不同设备
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
					
					// 送去显示的是str 存储是hex
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
					// 这里返回根据已注册的节点请求数据
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
						g_RequestNodeElecFlag = 1; // 允许请求下一个数据
					}
				}
			  break;
				default: printf("uart3_485Pack.cmd unkown! %x\r\n", *uart3_485Pack.cmd); break; 
			}
			memset(uart3_485Pack.dataBuf, 0, sizeof(uart3_485Pack.dataBuf));  
		}
}

/**********************************************************************************************************
 @ 功能： 操作继电器
 @ 入口： 开关 通道，1 2
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
 @ 功能： 需要间隔运行的程序
 @ 入口：  
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
		
		// 1000ms 请求一个节点数据
		if(g_RequestNodeCount >= 1000)
		{ 
			if(g_RequestNodeElecFlag && strcmp((char*)nodeInfo[currentRquesNodeIndex].name, "") )
			{  
				buildAndSendDataToNode(g_retBuf, &nodeInfo[currentRquesNodeIndex].baddr, REQUESTELEC, 0, sendNodeDatabuf); 
				//g_RequestNodeElecFlag = 0;	 // 收到节点数据后才允许下次请求
			}
			g_RequestNodeCount = 0; 
		}

}


/**********************************************************************************************************
 @ 功能： 读写节点类型和地址到flash
 @ 入口： 0读 1写
 *********************************************************************************************************/
void rwTypeAndAddr(u8 rw, BoardAddr *baddr)
{
	if(rw == 0) // 读取
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
 @ 功能：HLW8023电量计算 
 @ 返回：
 @ 备注：
 *********************************************************************************************************/
double EconAnalysis(u8 *pbuf)
{  
	unsigned int PF_COUNT = 0, PF = 0, PP_REG = 0;
	double E_con = 0;
	
	if((pbuf[20]&0x80)!=E_old_reg)//判断数据更新寄存器最高位有没有翻转
	{
		g_kCount++;
		E_old_reg=pbuf[20]&0x80;
	}
	
	PP_REG=pbuf[14]*65536+pbuf[15]*256+pbuf[16];	//计算功率参数寄存  
	PF=(g_kCount*65536)+(pbuf[21]*256)+pbuf[22];		//计算已用电量脉冲数
	PF_COUNT=((100000*3600)/(PP_REG*1.88))*10000;	//计算1度电对应的脉冲数量
	E_con=((PF*10000)/PF_COUNT)/10000.0;//计算已用电量 
	//printf("PF %d  PF_COUNT %d  已用电量：%0.4f°\r\n",PF,PF_COUNT,E_con); 
	 
 
	return E_con;
}
