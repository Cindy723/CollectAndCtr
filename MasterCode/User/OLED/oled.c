#include  "oled.h"
#include "main.h"

// ------------------------------------------------------------
// IO口模拟I2C通信
// SCL接PB^8
// SDA接PB^9
// ------------------------------------------------------------
void OLED_GPIO_Init(void)
{
	
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	  GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); 	//PB8,PB9 输出高点平
	
}

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{   
	 OLED_SDA = high;
	 delay_us(1);
   OLED_SCL = high;
   delay_us(1);
   OLED_SDA = low;
	 delay_us(1);
   OLED_SCL = low;
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{ 
	 OLED_SDA = low;
	 delay_us(1);
   OLED_SCL = low;
   delay_us(1);
   OLED_SDA = high;
	 delay_us(1);
   OLED_SCL = high;
}

/**********************************************
// 通过I2C总线写一个字节
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
  for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
			OLED_SDA=high;
		else
			OLED_SDA=low;
		    delay_us(1);
		OLED_SCL=high;
        delay_us(1);  //必须有保持SCL脉冲的延时
		OLED_SCL=low;
		    delay_us(1);
		IIC_Byte<<=1;
	}
    //原程序这里有一个拉高SDA，根据OLED的DATASHEET，此句必须去掉。
	OLED_SCL=1;
    delay_us(1);
	OLED_SCL=0;
}

/*********************OLED写数据*********************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}
/*********************OLED写命令**********************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);          //Slave address,SA0=0
	Write_IIC_Byte(0x00);			     //write command
	Write_IIC_Byte(IIC_Command);
	IIC_Stop();
}
/*********************OLED 设置坐标*******************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);  
	OLED_WrCmd((x&0x0f)|0x00);  
	//OLED_WrCmd((x&0x0f)|0x01);                                  /*解决边缘竖线问题！！默认0x01 ，改成0x00向左移  0x02向右移*/
	//OLED_WrCmd((x&0x0f)|0x02);               
} 


/*********************OLED 设置坐标*******************************/
void OLED_Set_Pos_2(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);  
	OLED_WrCmd((x&0x0f)|0x01);                                     /*6*8  ascall 专用通道*/               
} 
/*********************OLED全屏************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y); 
		OLED_WrCmd(0x00);  
		//OLED_WrCmd(0x01);                                          /*解决边缘竖线问题！！默认0x01 ，改成0x00向左移  0x02向右移*/
		//OLED_WrCmd(0x02);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_dat);
	}
}

/*----------------------------------------------------------------------------------------

	功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
	Draw_BMP(0,0,128,8,BMP1);  显示BMP1图片
	若改变了图片的起点坐标 对应的图片坐标也相应的增减
	例子：  0+10  0+2    32+10    (32/8)+2    [要显示的图片是32*32]
	Draw_BMP(10,   2,     42,      6,        BMP_XIN3);
------------------------------------------------------------------------------------------*/
 void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WrDat(BMP[j++]);
	    }
	}
}

/*********************OLED复位************************************/
void OLED_CLS(void)
{
   OLED_Fill(0x00);
}
//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	OLED_WrCmd(0X8D);  //设置电荷泵
	OLED_WrCmd(0X14);  //开启电荷泵
	OLED_WrCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	OLED_WrCmd(0X8D);  //设置电荷泵
	OLED_WrCmd(0X10);  //关闭电荷泵
	OLED_WrCmd(0XAE);  //OLED休眠
}
/*********************OLED初始化************************************/
void OLED_Init(void)
{
	OLED_GPIO_Init();
	delay_ms(100);//初始化之前的延时很重要！
	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa0);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00); //初始清屏
	OLED_Set_Pos(0,0);
} 
 

/*功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7  一共8行 0 1 2 3 4 5 6 7 */
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[],unsigned char against)
{
	unsigned char c=0,i=0,j=0;
	unsigned char Temp;
		if(!against)Temp = 0x00;
	else Temp = 0xFF;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos_2(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]^Temp);
		x+=6;
		j++;
	}
}

/*功能描述：显示 6*8 点阵 显示的坐标（x,y），y为页范围0～7*/
void OLED_Show6x8(unsigned char x,unsigned char y,unsigned int N,unsigned char against)
{
	unsigned char i = 0;
	unsigned int  adder = 6*N;
	unsigned char Temp;
		if(!against)Temp = 0x00;
	else Temp = 0xFF;
  if(x>126)
     x=0,y++;
	OLED_Set_Pos(x,y);
  for(i=0;i < 6;i++)OLED_WrDat(My6x8[adder++]^Temp);
}

/*功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～6  一共4行 0 2 4 6*/
void OLED_P8x16Str(unsigned char x,unsigned  char y,unsigned char ch[],unsigned char against)
{
	unsigned char c=0,i=0,j=0;
	unsigned char Temp;
	  if(!against)Temp = 0x00;
	else Temp = 0xFF;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
	 OLED_WrDat(F8X16[c*16+i]^Temp);
	 OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
	 OLED_WrDat(F8X16[c*16+i+8]^Temp);
		x+=8;
		j++;
	}
}



  

/*-------------------------------------------------------------------------------------------

	功能描述：显示16*16汉字的最小阵列(单字)  显示的坐标（x,y）
	形参    ：x为横坐标 0-112 (最大128-16)  y列坐标 0～7（竖着平分为8份） 
						table[]为传入的要显示的整个数组的地址
						N 为传来的数组位置(取哪一组16*16数据)，经过运算赋值给adder

--------------------------------------------------------------------------------------------*/					 

void OLED_P16x16Ch(unsigned char x,unsigned char  y,unsigned char  table[],unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder = 32*N;
	OLED_Set_Pos(x , y);			//传数据前设置地址，地址通过程序调用传进
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(table[adder]);	 //根据传进的地址读取相应的数组的16个内容
		adder += 1;
		
		
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(table[adder]); 
		adder += 1;
		
	} 	  	
}
/*-----------------------------------------------------------------------
			功能：		连续显示一个数组的可调节长度(多字)
			
			形参：  @ x ：起始位置(能被16整除，最大112)  
								例：0,16,32,48,64,80,96,112  （对应八个字）
							改进：直接转入第几个16*16点阵位置 0 - 7  通过local算出实际位置
							@ y ：第几页(0-7行)
							@ n : 发送长度
--------------------------------------------------------------------------*/	
														//3  2  6      hello   即在48，2页连续显示到temp第6个元素
void display_ch(unsigned char x,unsigned char y,unsigned char n,unsigned char  temp[])
{
	 unsigned char count=0,local;  //储存计算后的x位置
	 local = x*16;
	  
   	for(count = 0 ;count<n+1; count++ )  //n+1是实测后调整  不争论
	{
											//        x, y,  数据,哪个数组,第几个元素
				OLED_P16x16Ch( local+ count*16 , y ,temp , count );  //hello数组的第0个元素  的显示位置
		
	}
	
 
}



  /******************************************************************************
  *
  *   函数名称: 心脏在OLDE左上角跳动
  *   功能备注: 无
	*
  ******************************************************************************/
void Xintiao(void)
{
			Draw_BMP(0+5,1,32+5,5,BMP_XIN1);  
			delay_ms(50);
			Draw_BMP(0+5,1,32+5,5,BMP_XIN2); 
			delay_ms(50);
			Draw_BMP(0+5,1,32+5,5,BMP_XIN3); 
}
