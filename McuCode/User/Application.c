#include "Application.h"  
#include "init.h"    
#include "flash.h"  
#include "ADC.h"  
#include "Uart.h"   
 
//typedef struct 
//{
//	float	Reality;
//	float Meas;
//	float P;
//}RealityMeasP;
//	 
//#define ARRYPCOUNT 10
//RealityMeasP arryPV[ARRYPCOUNT];	// 电压实际值和测量值以及系数
//RealityMeasP arryPA[ARRYPCOUNT];  // 电流实际值和测量值以及系数 

/**********************************************************************************************************
 @ 功能：初始化电压电流校准数据
	 备注：根据记录值进行填写实际电流电压 测出值后 临近取出对应的系数 
				 目前使用平均系数已经可以达到要求
 *********************************************************************************************************/
//void setRealityMeas()
//{ 
//}
 
/**********************************************************************************************************
 @ 功能：串口数据解析
	 备注： 在拷贝前 就存入一个结构体中 表示一包数据 里面再嵌套一个板子地址信息
 *********************************************************************************************************/
void MasterDataAnalysis()
{ 
	u8 data[10];
	if(1 == uart1Pack.receiveok)
	{
		uart1Pack.receiveok = 0;   
//		if(CheckSum(uart1Pack.dataBuf,uart1Pack.packLen-1) != uart1Pack.dataBuf[uart1Pack.packLen - 1])
//		{
//		  printf("Rec CheckSum faild!\r\n"); 
//			memset(uart1Pack.dataBuf, 0, sizeof(uart1Pack.dataBuf));
//			data[0] = 0xE1; // 校验错误
//			buildAndSendDataTo485(g_retBuf, *uart1Pack.cmd, 1, data); 
//			return;			
//		}
		
		data[0] = 0xdd;
		switch(*uart1Pack.cmd)
		{
			case POWERCONTRL:
			{
				printf("CASE POWERCONTRL \r\n");
				relayOpreat((uart1Pack.content[0] & 0x80) >> 7 , (uart1Pack.content[0] & 0x7f));
				buildAndSendDataTo485(g_retBuf, *uart1Pack.cmd, 1, data); 
			}
			break;
			case REQUESTELEC:  
			{
				 printf("CASE REQUESTELEC \r\n");
				if(*boardAddr.type == ACTYPE) 
				{
					updateElectInfoAC(1); 
				}
				else if(*boardAddr.type == DCTYPE) 
				{ 
					updateElectInfoDC(1); 
				}
			}
			break;
			case REQUESTADDR:  
			{
				 printf("CASE REQUESTADDR \r\n");
				 updateBaseInfo(); 
			}
			break;
			case SETBOARDADDR:
			{
				 printf("CASE SETBOARDADDR \r\n");
				 if(0 != setBoardBaseInfo(uart1Pack.content))
				 {
						data[0] = 0xee;
				 }
				 
				 buildAndSendDataTo485(g_retBuf, *uart1Pack.cmd, 1, data);
			}
			break;
			default: printf("uart1Pack.cmd unkown! \r\n"); break;
		}  
		memset(uart1Pack.dataBuf, 0, sizeof(uart1Pack.dataBuf));  
	}
	 
//#ifdef TESTHMI_DC
//		if(uart2HMIPack.receiveok)
//		{
//			uart2HMIPack.receiveok = 0;   
//  
//			switch(*uart2HMIPack.cmd0)
//			{
//				case BUTTON_CMD: // cmd =0x11
//				{
//					printf("CASE BUTTON_CMD \r\n");
//				  if(*uart2HMIPack.Control_id1 == 0x6F) // 第一行CH1开关
//					{
//						//在这这里找出对应开关id对应的板子 再下达对应地址板子控制指令
//						relayOpreat(uart2HMIPack.Status[0], POWERCH1);
//					}
//				}
//				break;
//				default: printf("uart2HMIPack.cmd unkown! \r\n"); break; 
//			}
//			memset(uart2HMIPack.dataBuf, 0, sizeof(uart2HMIPack.dataBuf));  
//		}
//#endif
 
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
			printf("HC1 ON\r\n");
			RELAY1 = POWER_ON;  
			LEDContrl(LED2PIN, LEDON);
		}
		else 
		{
			printf("CH1 OFF\r\n");
			RELAY1 = POWER_OFF; 
			LEDContrl(LED2PIN, LEDOFF);
		}
	}
	else if(ch == POWERCH2)
	{
		if(sw == POWER_ON)
		{
			printf("CH2 ON\r\n");
			RELAY2 = POWER_ON; 
			LEDContrl(LED3PIN, LEDON);
		}
		else 
		{
			printf("CH2 OFF\r\n");
			RELAY2 = POWER_OFF; 
			LEDContrl(LED3PIN, LEDOFF);
		}
	}

}


/**********************************************************************************************************
 @ 功能：计算并上报AC电能信息 
 @ 备注：配合电能芯片
 *********************************************************************************************************/
void updateElectInfoAC(u8 needSend)
{
	//u8 data[10] = { 0 };
	char temp[120] = { 0 };  // 显示给HMI的数据
	uint32_t VP_REG=0,V_REG=0,CP_REG=0,C_REG=0;
	//uint32_t PP_REG=0,P_REG=0;
	float m_Voltage = 0.0,m_Cureent = 0.0;
	//float m_Power = 0.0,m_Gy = 0.0;
	float m_Voltage_adj = 0.0,m_Cureent_adj = 0.0; // 校正值 没管电阻参数 假设完全线性
	//uint32_t tV,tA,tW,tT;
	//uint32_t tV,tA;
	
//	while(uart2Pack.receiveok == 0)
//	{
//		printf("updateElectInfo waitting... \r\n"); 
//	}
	
 	uart2Pack.receiveok = 0;
	
 	if((uart2Pack.dataBuf[20] & 0x40) == 0x40)
	{
		VP_REG=(uart2Pack.dataBuf[2]<<16)|(uart2Pack.dataBuf[3]<<8)|(uart2Pack.dataBuf[4]);
		V_REG=(uart2Pack.dataBuf[5]<<16)|(uart2Pack.dataBuf[6]<<8)|(uart2Pack.dataBuf[7]);
		m_Voltage=(VP_REG/V_REG)*1.88; 
		m_Voltage_adj = (m_Voltage / 1.1465) * ACADJVP;
	  printf("\nm_Voltage %f  %f V\n", m_Voltage, m_Voltage_adj);
	}
  
	if((uart2Pack.dataBuf[20] & 0x20) == 0x20)
	{
		CP_REG=(uart2Pack.dataBuf[8]<<16)|(uart2Pack.dataBuf[9]<<8)|(uart2Pack.dataBuf[10]);
		C_REG=(uart2Pack.dataBuf[11]<<16)|(uart2Pack.dataBuf[12]<<8)|(uart2Pack.dataBuf[13]);
		//m_Cureent=((CP_REG*100)/C_REG)/100.0;
		m_Cureent=((CP_REG*10)/C_REG) * 2 / 100.0;
		m_Cureent_adj = (m_Cureent * 1.45) * ACADJAP;
		printf("m_Cureent %f %f A\n", m_Cureent, m_Cureent_adj);
	}

//	if((uart2Pack.dataBuf[20] & 0x10) == 0x10)
//	{
//		PP_REG=(uart2Pack.dataBuf[14]<<16)|(uart2Pack.dataBuf[15]<<8)|(uart2Pack.dataBuf[16]);
//		P_REG=(uart2Pack.dataBuf[17]<<16)|(uart2Pack.dataBuf[18]<<8)|(uart2Pack.dataBuf[19]);
//		m_Power=(PP_REG/P_REG)*1.88*1;
//		m_Gy = m_Power/(m_Cureent*m_Voltage); 
//		if(m_Gy >=1) m_Gy = 0.99;
//		printf("m_Power %f W m_Gy %f\n", m_Power, m_Gy); 
//	}

//	tV = m_Voltage_adj * 100;
//	tA = m_Cureent_adj * 100;
	
	//tW = m_Power * 100; 
	//tT = (g_currentElectrCon+g_lastElectrCon) * 10000;
	//tT = (g_currentElectrCon) * 10000;
	  
//	data[0] = (tV >> 16) & 0xFF;    
//	data[1] = (tV >> 8) & 0xFF;    
//	data[2] =  tV & 0xFF;   
//		
//	data[3] = (tA >> 8) & 0xFF;   
//	data[4] =  tA & 0xFF;   
		
	//retBuf[8] = m_Gy*100;   
		
	// retBuf[9] = (tW >> 16) & 0xFF;   
	// retBuf[10] = (tW >> 8) & 0xFF;   
	// retBuf[11] = tW & 0xFF;   
		
	// retBuf[12] = (tT >> 24) & 0xFF;   
	// retBuf[13] = (tT >> 16) & 0xFF;   
	// retBuf[14] = (tT >> 8)  & 0xFF;   
	// retBuf[15] = tT & 0xFF;    
	//printf("EL: ");
	//printHex(uart2Pack.dataBuf, uart2Pack.packLen);
	
	sprintf(temp, "AC: %.2fV / %.2fA", m_Voltage_adj, m_Cureent_adj);
	if(needSend)
	buildAndSendDataToHMI(g_retBuf, 101, temp); 
}


/**********************************************************************************************************
 @ 功能： 计算并上报DC电能信息 
 @ 备注： 配合 ADC 和故障监测芯片 tps2492
 *********************************************************************************************************/
void updateElectInfoDC(u8 needSend)
{
	u8 data[10];
	float ADC_ConvertedValueLocal[NOFCHANEL];   // ADC结果
	 
	ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*3.3;
	ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1]/4096*3.3;
	ADC_ConvertedValueLocal[2] =(float) ADC_ConvertedValue[2]/4096*3.3;

	printf("CH0 %f I2 value = %f A \r\n",ADC_ConvertedValueLocal[0], ADC_ConvertedValueLocal[0] * dcScale_I2);
	printf("CH1 I1 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
	printf("CH2 %f V value = %f V \r\n",ADC_ConvertedValueLocal[0], ADC_ConvertedValueLocal[2] * dcScale_v);
	
	// 故障信息
	
	
	
	if(needSend){
		buildAndSendDataTo485(g_retBuf, REQUESTELEC, 5, data);
	}
}

/**********************************************************************************************************
 @ 功能： 返回基本信息 板子类型 板子地址
 @ 入口：  
 *********************************************************************************************************/
void updateBaseInfo()
{  
	rwTypeAndAddr(0, &boardAddr); 
	buildAndSendDataTo485(g_retBuf, REQUESTADDR, 3, boardAddr.addr);
}


/**********************************************************************************************************
 @ 功能： 设置基本信息 板子类型 板子地址
 @ 入口： 数据开始
 @ 返回： 成功返回0
 *********************************************************************************************************/
u8 setBoardBaseInfo(const u8 *pconten)
{
	BroadAddr addr;
	BroadAddr addrt;
	memcpy(addr.addr, pconten, 3);
	rwTypeAndAddr(1, &addr); 
	rwTypeAndAddr(0, &addrt);
	return hexCompaer(addr.addr, addrt.addr, 3) ;
}

/**********************************************************************************************************
 @ 功能： 需要间隔运行的程序
 @ 入口：  
 *********************************************************************************************************/
void IntervalProc()
{
		// 运行灯闪烁
		if(g_LEDBlinkCount > 1 && g_LEDBlinkCount < 110)
		{
			LEDContrl(LEDRUNPIN, LEDOFF);
			#ifdef MINIBOARD
			PCout(13) = LEDON; 
			#endif
		}
		else if(g_LEDBlinkCount > 110)
		{
			g_LEDBlinkCount = 0;
			LEDContrl(LEDRUNPIN, LEDON); 
			#ifdef MINIBOARD
			PCout(13) = LEDOFF;  
			#endif
		}

}


/**********************************************************************************************************
 @ 功能： 读写节点类型和地址到flash
 @ 入口： 0读 1写
 *********************************************************************************************************/
void rwTypeAndAddr(u8 rw, BroadAddr *baddr)
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
