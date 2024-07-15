#include "Application.h"  
#include "init.h"    
#include "flash.h"   
#include "Uart.h"   
#include "oled.h" 
#include "TFTCommunic.h"
#include "UartSendQueue.h"  

u16 E_old_reg = 0; 
u16 g_kCount = 0;  					  // AC�������� ����Ƿ��и��� 	

void sortNodes(void);

/**********************************************************************************************************
 @ ����: ��������Ľڵ�����
 @ ����: 
 @ ����: 
 @ ��ע: 
 *********************************************************************************************************/
void NodeDataAnalysis()
{
		char tempstr[120];
		static u8 nodeIDSimuOffset = 0; //!!!!!!!!!!!!!!!!!!!!!!!!!! ģ��ÿ���������Ĳ�ͬ�豸
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
				case REQUESTADDR: // �������Ľڵ�
				{ 
					 
					nodeInfo[g_nodeTotalCount].baddr.addr[0] = *uart3_485Pack.addr0;
					nodeInfo[g_nodeTotalCount].baddr.addr[1] = *uart3_485Pack.addr1;
					nodeInfo[g_nodeTotalCount].baddr.addr[2] = *uart3_485Pack.addr2 + nodeIDSimuOffset;
					nodeInfo[g_nodeTotalCount].baddr.type 	 = &nodeInfo[g_nodeTotalCount].baddr.addr[0]; 
					
					nodeInfo[g_nodeTotalCount].baddr.addrInt = (nodeInfo[g_nodeTotalCount].baddr.addr[0]  << 16) | 
																									 	  (nodeInfo[g_nodeTotalCount].baddr.addr[1] << 8) | 
																										   nodeInfo[g_nodeTotalCount].baddr.addr[2]; 
					
					// to tft
					sprintf(tempstr, "%02x", *uart3_485Pack.addr0);
					sprintf(&tempstr[2], "%02x", *uart3_485Pack.addr1);
					sprintf(&tempstr[4], "%02x", *uart3_485Pack.addr2 + nodeIDSimuOffset);
					buildAndSendStr2TFT(TFT_SET_PAGE, 8, tempstr);
					printf("NodeData ret Addr %s %d \r\n", tempstr, nodeInfo[g_nodeTotalCount].baddr.addrInt); 
					
					nodeIDSimuOffset ++; 
				  dispSetTips("�������");
				}
				break;
				case REQUESTELEC:
				{
					u8 index = 0;
					printf("NodeData Ret REQUESTELEC ");  printHex(uart3_485Pack.addr0, 3);  
					// match data addr
					for(index = 0; index < g_nodeTotalCount+1; index ++){
						  if(0 == hexCompaer(nodeInfo[index].baddr.addr, (u8*)uart3_485Pack.addr0, 3)){ 
								 break;
							}
					}  
					if(index >= g_nodeTotalCount){
							printf("unmatch addr!\r\n");
							memset(uart3_485Pack.dataBuf, 0, sizeof(uart3_485Pack.dataBuf));  
							return;
					}
					
					if(*uart3_485Pack.addr0 == 0xac)
					{ 
						nodeInfo[index].eInfo.vTotal = (float)(((uart3_485Pack.content[0] << 16) | (uart3_485Pack.content[1] << 8) | uart3_485Pack.content[2]) * 0.01);
						nodeInfo[index].eInfo.i1 		 = (float)(((uart3_485Pack.content[3] << 8)  | uart3_485Pack.content[4]) * 0.01);
						nodeInfo[index].eInfo.i2 		 = nodeInfo[index].eInfo.i1;
						
						if((nodeInfo[index].eInfo.vTotal > AC_MAX_V) || (nodeInfo[index].eInfo.vTotal < AC_MIN_V) ||  
							 (nodeInfo[index].eInfo.i1 > AC_MAX_A)){
							nodeInfo[index].eInfo.erro[0] = 1;
							printf("AC power Erro !! \r\n");	 
								 
							// �ر���·��Դ
							sendNodeDatabuf[0] = POWERCH1;
							buildAndSendDataToNode(&nodeInfo[index].baddr, POWERCONTRL, 1, sendNodeDatabuf); 
							delay_ms(100);	// ctodo !���� δ�Ӷ���
							sendNodeDatabuf[0] = POWERCH2;
							buildAndSendDataToNode(&nodeInfo[index].baddr, POWERCONTRL, 1, sendNodeDatabuf);
						}
						 
						printf("vTotal %f ", nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal); 
						printf("i1 %f ",  nodeInfo[g_currentRquesNodeIndex].eInfo.i1); 
						printf("erro: %d \r\n",  nodeInfo[g_currentRquesNodeIndex].eInfo.erro[0]);
					}
					else if(*uart3_485Pack.addr0 == 0xdc)
					{ 
						nodeInfo[index].eInfo.vTotal	= (float)(((uart3_485Pack.content[0] << 8) | uart3_485Pack.content[1]) * 0.01); 
						nodeInfo[index].eInfo.i1 			= (float)(((uart3_485Pack.content[2] << 8) | uart3_485Pack.content[3]) * 0.01); 
						nodeInfo[index].eInfo.i2 			= (float)(((uart3_485Pack.content[4] << 8) | uart3_485Pack.content[5]) * 0.01); 
						nodeInfo[index].eInfo.erro[0] = uart3_485Pack.content[6];
						nodeInfo[index].eInfo.erro[1] = uart3_485Pack.content[7];
						
						// print
						printf("vTotal %f ",	 		nodeInfo[g_currentRquesNodeIndex].eInfo.vTotal); 
						printf("i1 %f ", 			 		nodeInfo[g_currentRquesNodeIndex].eInfo.i1); 
						printf("i2 %f ", 			 		nodeInfo[g_currentRquesNodeIndex].eInfo.i2);
						printf("Ch1erro: %d ", 		nodeInfo[g_currentRquesNodeIndex].eInfo.erro[0]);
						printf("Ch2erro: %d\r\n", nodeInfo[g_currentRquesNodeIndex].eInfo.erro[1]);
					
						sortNodes(); // �쳣��ǰ��ʾ
					}

					g_RequestNodeElecFlag = 1;
					timerVariate.NodeTimeCount10ms[index] = 0; 
				}
			  break;
				default: printf("uart3_485Pack.cmd unkown! %x\r\n", *uart3_485Pack.cmd); break; 
			}
			memset(uart3_485Pack.dataBuf, 0, sizeof(uart3_485Pack.dataBuf));  
		}
}


/**********************************************************************************************************
 @ ���ܣ� ��Ҫ������еĳ���
 @ ��ڣ�  
 *********************************************************************************************************/
void IntervalProc()
{ 
		if(timerVariate.LEDBling_kCount > 1 && timerVariate.LEDBling_kCount < 800)
		{
			LEDContrl(LEDRUNPIN, LEDOFF);  
			OLED_P6x8Str(120,6,(u8*)" ",0);
		}
		else if(timerVariate.LEDBling_kCount > 830)
		{
			timerVariate.LEDBling_kCount = 0;
			LEDContrl(LEDRUNPIN, LEDON);  
			OLED_P6x8Str(120,6,(u8*)" ",1);  
		}
		
		// ����һ���ڵ�����
		if(timerVariate.RequestNodeCount >= 500)
		{ 
			//getListPagef1();
			printf("RequestNode %d  %s \r\n",g_currentRquesNodeIndex, nodeInfo[g_currentRquesNodeIndex].baddr.addrStr);
			if(g_RequestNodeElecFlag && strcmp((char*)nodeInfo[g_currentRquesNodeIndex].name, "")){ // ����������ܲ��ҵ�ǰ�ڵ���ע��
				buildAndSendDataToNode(&nodeInfo[g_currentRquesNodeIndex].baddr, REQUESTELEC, 0, sendNodeDatabuf); 
			}
			
			g_currentRquesNodeIndex ++; // ������һ�� 
			if(g_currentRquesNodeIndex > g_nodeTotalCount-1){
				g_currentRquesNodeIndex = 0; // ��ͷ����
			}  
			timerVariate.RequestNodeCount = 0;
		}

		// ��ʾ���нڵ�����
		if(timerVariate.DispElecNodeCount >= 1100 && currentTFTV.Page == TFT_DISP_PAGE){ 
			dispElec2TFT(nodeInfo);  
			timerVariate.DispElecNodeCount = 0;
		}		
		
		// ����TFT����
		if(timerVariate.SendTFTQueueCount >= 200){ 
			sendQueueMSG();  
			timerVariate.SendTFTQueueCount = 0;
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
 
 
// �쳣�ڵ㿿ǰ�洢  �������쳣 �ڵ�idС����ǰ
// ���߲���ͨ�� ʵ��δ��
int hasError(NodeInfo *node) {
	return node->eInfo.erro[0] != 0 || node->eInfo.erro[1] != 0;
}
void sortNodes() {
	int i = 1, j;
	for (; i < MAX_NODE; i++) {
		NodeInfo key = nodeInfo[i];
		j = i - 1;

		while (j >= 0 && (hasError(&nodeInfo[j]) < hasError(&key) ||
		                  (hasError(&nodeInfo[j]) == hasError(&key) && 
											nodeInfo[j].baddr.addrInt > key.baddr.addrInt))) 
		{
			nodeInfo[j + 1] = nodeInfo[j];
			j--;
		}
		nodeInfo[j + 1] = key;
	}
}

