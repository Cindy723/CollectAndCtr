#include "Tools.h" 
#include "Init.h" 
#include "stm32f10x_it.h"   
#include "Application.h"      
#include "Uart.h"
  
u16 E_old_reg = 0; 
u16 kCount = 0;  // 电量脉冲 监测是否有更新
   
char g_ChipIDi[12] = { 0 };
char g_ChipIDS[25] = "";  // 3*8 +1 

/**********************************************************************************************************
 @ 功能：HLW8023电量计算
 @ 参数：[I] : 电能数据指针
		 [O] : 
 @ 返回：
 @ 备注：
 *********************************************************************************************************/
double EconAnalysis(u8 *pbuf)
{  
	unsigned int PF_COUNT = 0, PF = 0, PP_REG = 0;
	double E_con = 0;
	
	if((pbuf[20]&0x80)!=E_old_reg)//判断数据更新寄存器最高位有没有翻转
	{
		kCount++;
		E_old_reg=pbuf[20]&0x80;
	}
	
	PP_REG=pbuf[14]*65536+pbuf[15]*256+pbuf[16];	//计算功率参数寄存  
	PF=(kCount*65536)+(pbuf[21]*256)+pbuf[22];		//计算已用电量脉冲数
	PF_COUNT=((100000*3600)/(PP_REG*1.88))*10000;	//计算1度电对应的脉冲数量
	E_con=((PF*10000)/PF_COUNT)/10000.0;//计算已用电量 
	//printf("PF %d  PF_COUNT %d  已用电量：%0.4f°\r\n",PF,PF_COUNT,E_con); 
	 
 
	return E_con;
}
 

/***********************************************************************************************************
 @ 功能： 32位16进制数转字符串
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void uint32_Str(uint32_t *id, char* desbuf, uchar uint32len)
{
  int index = 0;
	int i = 0; 
	for (; i < uint32len; i++) { 
			sprintf(&desbuf[index], "%08x", id[i]); 
			index += 8;
	}
    desbuf[uint32len * 8] = '\0'; 
}
 

/***********************************************************************************************************
 @ 功能： IO模拟串口输出
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void IO_TXD(u8 Data)
{ 
	static int j = 0;
	u8 i = 0;
	OI_TXD = 0;  
	j=8; //115200 
	delay_us(j);
	for(i = 0; i < 8; i++)
	{
		if(Data&0x01)
			OI_TXD = 1;  
		else
			OI_TXD = 0; 	
		
		delay_us(j);
		Data = Data>>1;
	}
	OI_TXD = 1;
	delay_us(j);
}
	
void IO_USART_Send(u8 *buf, u8 len)
{
	u8 t;
	for(t = 0; t < len; t++)
	{
		IO_TXD(buf[t]);
	}
}
 
/***********************************************************************************************************
 @ 功能： 计算校验和
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
uchar CheckSum(uchar* buf, uchar len)
{
	uchar i = 0;
	uchar sum = 0;
	for(i = 0; i < len; ++i)
	{
		sum += buf[i];
	}
	return sum;
}
 

/***********************************************************************************************************
 @ 功能：  打印十六进制数据的函数实现
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void printHex(const unsigned char* data, size_t length) 
{
		size_t i;
    for (i = 0; i < length; i++) { 
        printf("%02X ", data[i]);
    }
    // 打印换行符
    printf("\n");
}

/***********************************************************************************************************
 @ 功能： 十六进制数据比对
 @ 入口： 要比较的目标数据  比较数据 比较长度
 @ 出口： 比对成功返回0 比对失败返回错误位置 从1开始
 @ 备注：  
 *********************************************************************************************************/
u8 hexCompaer(u8* desp, u8* srcp, u8 len)
{
	u8 i = 0;
    if (len == 0)
    {
        printf("hexCompaer error: len is 0\n");
        return 255;
    }

    for(; i < len; i++)
    {
        if(desp[i] != srcp[i])
        {
            return i + 1;
        }
    }

   // printf("hexCompaer ok\n");
    return 0;
}


/***********************************************************************************************************
 @ 功能： 十六进制字符串转byte 
 @ 备注： "ccdd" -> cc dd
 *********************************************************************************************************/
int StrToHexByte(unsigned char *str, unsigned char *hex)
{
    int input_len = strlen((const char*)str);
    int output_len = 0;
		int i = 0;
		char byte[3];
	
    if (input_len % 2 != 0) {
        printf("Invalid input length\n");
        return -1;
    }

    output_len = input_len / 2;
    for (; i < output_len; i++) {
			byte[0] = str[i*2];
			byte[1] =str[i*2 + 1];
			byte[2] ='\0';
      hex[i] = (unsigned char)strtol(byte, NULL, 16);
		}

	return output_len;
}



/**********************************************************************************************************
 @ 功能：获取F1O3 芯片id
 @ 参数：[I] : 
		 [O] : 
 @ 返回：
 @ 备注：
 *********************************************************************************************************/
void getChipIdStr(char *pbuf)
{
	uint32_t chipid[3] = { 0 }; // 96bit 芯片唯一id
	uchar j = 0;
	
	chipid[0] = *(uint32_t*)(0x1FFFF7e8);
	chipid[1] = *(uint32_t*)(0x1FFFF7e8+4);
	chipid[2] = *(uint32_t*)(0x1FFFF7e8+8);
	printf("ChipId =%#x %#x %#x\n",chipid[0], chipid[1], chipid[2]); 
	uint32_Str(chipid, pbuf, 3);
	printf("ChipIdStr: %s\n", pbuf); 
	 
	for(; j < 3; ++j)
	{
		g_ChipIDi[4*j] = (chipid[j] >> 24) & 0xFF;
		g_ChipIDi[4*j+1] = (chipid[j] >> 16) & 0xFF;
		g_ChipIDi[4*j+2] = (chipid[j] >> 8) & 0xFF;
		g_ChipIDi[4*j+3] = chipid[j] & 0xFF; 
	}
}
