#ifndef _TFTCOMMUNIC_H
#define _TFTCOMMUNIC_H
 
#include "main.h"  

 // TFT的指令
#define BUTTON_CMD 0XB1
#define USER_CMD 	 0XCD

#define USER_CMD_REGISTER 	 0XC1 			 // 注册按钮按下
#define USER_CMD_NODE_PAGECHANGE 	 0XC3  // 设置界面节点列表切换
#define USER_CMD_DISPP_AGECHANGE 	 0XC4  // 显示界面页面切换 

// TFT 页面定义
#define TFT_DISP_SETNUM 1
#define TFT_PAGE_SETNUM 2

// 整页节点个数
#define ONEPAGENODE 6 

extern u8 g_RequestNodeElecFlag; 
extern u8 g_currentRquesNodeIndex;
extern u8 g_nodeTotalCount;
extern u8 g_currentPage;
extern u8 g_currentNodeListPage;
extern u8 sendNodeDatabuf[12];
 
void TFTanalysis(void); 
void buildAndSendStr2TFT(u8 *pbuf, u8 pagid, u8 ctrlid, char* str);
void getTFTText(u8 *pbuf, u8 pagid, u8 ctrlid);
void dispElec2TFT(u8 *pbuf, NodeInfo *info);
 
 
#endif
