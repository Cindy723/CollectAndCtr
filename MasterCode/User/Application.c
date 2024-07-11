#include "Application.h"  
#include "init.h"    
#include "flash.h"   
#include "Uart.h"   
#include "oled.h" 
#include "TFTCommunic.h"
#include "UartSendQueue.h"  

u16 E_old_reg = 0; 
u16 g_kCount = 0;  					  // AC电量脉冲 监测是否有更新 	


/**********************************************************************************************************
 @ 功能: 解析请求的节点数据
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void NodeDataAnalysis()
{
		char tempstr[120];
		static u8 nodeIDSimuOffset = 0; //!!!!!!!!!!!!!!!!!!!!!!!!!! 模拟每次搜索到的不同设备
		//u8 nodeIDSimuOffset = 0; 
	
		if(uart3_485Pack.receiveok)
		{
			uart3_485Pack.receiveok = 0;   
		 	switch(*uart3_485Pack.cmd)
			{
				case POWERCONTRL:
				{
					printf("NodeData ret [%02x] POWERCONTRL \r\n", *uart3_485Pack.content);  
					//buildAndSendStr2TFT(g_retBuf, 8, tempstr);
			 	}
				break;
				case REQUESTADDR: // 搜索到的节点
				{
					printf("NodeData ret ADDR\r\n"); 
					sprintf(tempstr, "%02x", *uart3_485Pack.addr0);
					sprintf(&tempstr[2], "%02x", *uart3_485Pack.addr1);
					sprintf(&tempstr[4], "%02x", *uart3_485Pack.addr2 + nodeIDSimuOffset);
					 
					nodeInfo[g_nodeTotalCount].baddr.addr[0] = *uart3_485Pack.addr0;
					nodeInfo[g_nodeTotalCount].baddr.addr[1] = *uart3_485Pack.addr1;
					nodeInfo[g_nodeTotalCount].baddr.addr[2] = *uart3_485Pack.addr2;
					nodeInfo[g_nodeTotalCount].baddr.type = &nodeInfo[g_nodeTotalCount].baddr.addr[0]; 
					
					buildAndSendStr2TFT(TFT_PAGE_SETNUM, 8, tempstr);
					nodeIDSimuOffset ++; 
				  dispSetTips("搜索结束");
				}
				break;
				case REQUESTELEC:
				{
					printf("NodeData Ret REQUESTELEC ");  printHex(uart3_485Pack.addr0, 3); 
					memcpy(&nodeInfo[g_currentRquesNodeIndex].baddr, uart3_485Pack.addr0, 3);
					if(*uart3_485Pack.addr0 == 0xac)
					{
					  nodeInfo[g_currentRquesNodeIndex].needDispElec = 1;
						nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal = (float)(((uart3_485Pack.content[0] << 16) | (uart3_485Pack.content[1] << 8) | uart3_485Pack.content[2]) * 0.01);
						//printf("vTotal %f\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal); 

						nodeInfo[g_currentRquesNodeIndex].eInfo.i1 =	(float)(((uart3_485Pack.content[3] << 8) | uart3_485Pack.content[4]) * 0.01);
						//printf("i1 %f\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.i1); 
					}
					else if(*uart3_485Pack.addr0 == 0xdc)
					{
						nodeInfo[g_currentRquesNodeIndex].needDispElec = 1;
						nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal = (float)(((uart3_485Pack.content[0] << 8) | uart3_485Pack.content[1]) * 0.01);
						//printf("vTotal %f\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal); 

						nodeInfo[g_currentRquesNodeIndex].eInfo.i1 = (float)(((uart3_485Pack.content[2] << 8) | uart3_485Pack.content[3]) * 0.01);
						//printf("i1 %f\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.i1); 

						nodeInfo[g_currentRquesNodeIndex].eInfo.i2 = (float)(((uart3_485Pack.content[4] << 8) | uart3_485Pack.content[5]) * 0.01);
						//printf("i2 %f\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.i2);
 
						//g_currentRquesNodeIndex ++; 
						if(g_currentRquesNodeIndex >= g_nodeTotalCount)
						{
							g_currentRquesNodeIndex = 0; // 从头请求
						}
						g_RequestNodeElecFlag = 1;
					}
				}
			  break;
				default: printf("uart3_485Pack.cmd unkown! %x\r\n", *uart3_485Pack.cmd); break; 
			}
			memset(uart3_485Pack.dataBuf, 0, sizeof(uart3_485Pack.dataBuf));  
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
		
		// 请求一个节点数据
		if(g_RequestNodeCount >= 500)
		{ 
			//getListPagef1();
			if(g_RequestNodeElecFlag && strcmp((char*)nodeInfo[g_currentRquesNodeIndex].name, "") ) // 允许请求电能并且当前节点已注册
			{  
				buildAndSendDataToNode(&nodeInfo[g_currentRquesNodeIndex].baddr, REQUESTELEC, 0, sendNodeDatabuf); 
			}
			g_RequestNodeCount = 0; 
		}

		// 显示所有节点数据
		if(g_DispElecNodeCount >= 1100 && g_currentPage == TFT_DISPNODE_NUM)
		{ 
			dispElec2TFT(nodeInfo); 
			g_DispElecNodeCount = 0;
		}		
		
		// 发送TFT队列
		if(g_SendTFTQueueCount >= 200)
		{ 
			sendQueueMSG();  
			g_SendTFTQueueCount = 0;
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

