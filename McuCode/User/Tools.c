#include "Tools.h" 
#include "Init.h" 
#include "stm32f10x_it.h"   
#include "Application.h"      
#include "Uart.h"
  
u16 E_old_reg = 0; 
u16 kCount = 0;  // �������� ����Ƿ��и���
   
char g_ChipIDi[12] = { 0 };
char g_ChipIDS[25] = "";  // 3*8 +1 

/**********************************************************************************************************
 @ ���ܣ�HLW8023��������
 @ ������[I] : ��������ָ��
		 [O] : 
 @ ���أ�
 @ ��ע��
 *********************************************************************************************************/
double EconAnalysis(u8 *pbuf)
{  
	unsigned int PF_COUNT = 0, PF = 0, PP_REG = 0;
	double E_con = 0;
	
	if((pbuf[20]&0x80)!=E_old_reg)//�ж����ݸ��¼Ĵ������λ��û�з�ת
	{
		kCount++;
		E_old_reg=pbuf[20]&0x80;
	}
	
	PP_REG=pbuf[14]*65536+pbuf[15]*256+pbuf[16];	//���㹦�ʲ����Ĵ�  
	PF=(kCount*65536)+(pbuf[21]*256)+pbuf[22];		//�������õ���������
	PF_COUNT=((100000*3600)/(PP_REG*1.88))*10000;	//����1�ȵ��Ӧ����������
	E_con=((PF*10000)/PF_COUNT)/10000.0;//�������õ��� 
	//printf("PF %d  PF_COUNT %d  ���õ�����%0.4f��\r\n",PF,PF_COUNT,E_con); 
	 
 
	return E_con;
}
 

/***********************************************************************************************************
 @ ���ܣ� 32λ16������ת�ַ���
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
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
 @ ���ܣ� IOģ�⴮�����
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
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
 @ ���ܣ� ����У���
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
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
 @ ���ܣ�  ��ӡʮ���������ݵĺ���ʵ��
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void printHex(const unsigned char* data, size_t length) 
{
		size_t i;
    for (i = 0; i < length; i++) { 
        printf("%02X ", data[i]);
    }
    // ��ӡ���з�
    printf("\n");
}

/***********************************************************************************************************
 @ ���ܣ� ʮ���������ݱȶ�
 @ ��ڣ� Ҫ�Ƚϵ�Ŀ������  �Ƚ����� �Ƚϳ���
 @ ���ڣ� �ȶԳɹ�����0 �ȶ�ʧ�ܷ��ش���λ�� ��1��ʼ
 @ ��ע��  
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
 @ ���ܣ� ʮ�������ַ���תbyte 
 @ ��ע�� "ccdd" -> cc dd
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
 @ ���ܣ���ȡF1O3 оƬid
 @ ������[I] : 
		 [O] : 
 @ ���أ�
 @ ��ע��
 *********************************************************************************************************/
void getChipIdStr(char *pbuf)
{
	uint32_t chipid[3] = { 0 }; // 96bit оƬΨһid
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
