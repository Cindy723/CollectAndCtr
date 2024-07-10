#ifndef  __OLED_H
#define  __OLED_H

#include "init.h" 
#include  "codetab.h" 

#define OLED_SCL PBout(8)
#define OLED_SDA PBout(9) 
 

#define high 1
#define low 0
 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define Brightness 255 //0~255设置亮度

void IIC_Start(void);// -- 开启I2C总线
void IIC_Stop(void);// -- 关闭I2C总线
void Write_IIC_Byte(unsigned char IIC_Byte);// -- 通过I2C总线写一个byte的数据
void OLED_WrDat(unsigned char dat);// -- 向OLED屏写数据
void OLED_WrCmd(unsigned char cmd);// -- 向OLED屏写命令
void OLED_Set_Pos(unsigned char x, unsigned char y);// -- 设置显示坐标
void OLED_Set_Pos_2(unsigned char x, unsigned char y);// 因为解决1.3寸oled右边竖线问题的时候对这个函数进行了修改  但是6*8字符不需要修改
void OLED_Fill(unsigned char bmp_dat);// -- 全屏显示(显示BMP图片时才会用到此功能) 
void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]); 
void OLED_CLS(void);// -- 复位/清屏
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Init(void);// -- OLED屏初始化程序，此函数应在操作屏幕之前最先调用


void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[],unsigned char against);// -- 6x8点整，用于显示ASCII码的最小阵列，不太清晰
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[],unsigned char against);// -- 8x16点整，用于显示ASCII码，非常清晰
void OLED_Show6x8(unsigned char x,unsigned char y,unsigned int N,unsigned char against);
void OLED_Show8x16(unsigned char x,unsigned char y,unsigned int N,unsigned char against); 
void OLED_P16x16Ch(unsigned char x, unsigned char y,unsigned char  table[],unsigned char N);
void display_ch(unsigned char x,unsigned char y,unsigned char n,unsigned char  temp[]);
void Xintiao(void);
 
#endif
