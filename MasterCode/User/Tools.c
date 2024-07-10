#include "Tools.h" 
#include "Init.h" 
#include "stm32f10x_it.h"      
#include "Uart.h" 
#include "oled.h"
  
 
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
 @ 功能：  OLED 显示十六进制
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void displayHex2oled(const unsigned char* data, uint8_t length, uint8_t x, uint8_t y) 
{
    size_t i;
    unsigned char disp[121]; // 调整大小为121，以确保足够容纳全部数据和终止符

    if (length > 60) {
        length = 60; // 确保 length 不超过 60，避免数组越界
    }

    for (i = 0; i < length; i++) { 
        sprintf((char*)&disp[i * 2], "%02X", data[i]);
    }
    disp[2 * length] = '\0'; // 正确设置字符串终止符

    OLED_P6x8Str(x, y, disp, 0);  
}

/***********************************************************************************************************
 @ 功能： 十六进制数据比对
 @ 入口： 要比较的目标数据  比较数据 比较长度
 @ 出口： 比对成功返回0 比对失败返回错误位置 从1开始
 @ 备注：  
 *********************************************************************************************************/
u8 hexCompaer(u8* desp, u8* srcp, u8 len)
{
   u8 i = 0, j = 0;
	for(i = 0; i < len; i++)
	{
		if(desp[i] == srcp[i])
		{
			j++;
		}else 
		{
			if(j == 0) return 1;
			else return j+1;
		}
	}
	
	if(j == len-1)
	{
		printf("hexCompaer ok\n");
		return 0;
	}
	
	return 255;
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
 
