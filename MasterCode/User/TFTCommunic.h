#ifndef _TFTCOMMUNIC_H
#define _TFTCOMMUNIC_H
 
#include "main.h"  

 // TFT的指令
#define BUTTON_CMD 0XB1
#define USER_CMD 	 0XCD

#define USER_CMD_REGISTER 	 0XC1 			 // 注册按钮按下
#define USER_CMD_PSWENTER 	 0XC2  			 // 设置界面密码输入结束
#define USER_CMD_NODE_PAGECHANGE 	 0XC3  // 设置界面节点列表切换
#define USER_CMD_DISP_PAGECHANGE 	 0XC4  // 显示界面页面切换 
#define USER_CMD_DELETE_NODE		 	 0XC5  // 设置界面删除指令 

// TFT 页面定义
#define TFT_DISP_PAGE 1
#define TFT_SET_PAGE 2

// 整页节点个数
#define ONEPAGENODE 6 

// TFT 当前的一些重要变量存储
typedef struct
{
	u8 Page; // 当前页面
	u8 setPageNodeListF1; // 设置界面下的分子
	u8 setListF2;				  // 设置和显示界面下的分母
	u8 DispListPage;   		// 显示界面下的分子 
}CurrentTFTVariate;

extern CurrentTFTVariate currentTFTV;

extern u8 g_RequestNodeElecFlag; 
extern u8 g_currentRquesNodeIndex;
extern u8 g_nodeTotalCount; 
extern u8 g_currentsetPageNodeListF1;
extern u8 sendNodeDatabuf[12];
 
void buildAndSendStr2TFT(u8 pagid, u8 ctrlid, char* str);
void getTFTText(u8 pagid, u8 ctrlid);
void dispElec2TFT(NodeInfo *info);
void dispSetTips(char* tip);
void rebootTFT(void);
void getListPagef1(void);
void disp_hindTFTContrl(u8 pageid, u8 contrlid, u8 isdisplay);
void TFTanalysis(void); 
void updateRegisterCount(void);
void updateListPagef2(void);
#endif
