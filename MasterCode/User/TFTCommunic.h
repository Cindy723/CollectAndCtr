#ifndef _TFTCOMMUNIC_H
#define _TFTCOMMUNIC_H
 
#include "main.h"  

 // TFT��ָ��
#define BUTTON_CMD 0XB1
#define USER_CMD 	 0XCD

#define USER_CMD_REGISTER 	 0XC1 			 // ע�ᰴť����
#define USER_CMD_NODE_PAGECHANGE 	 0XC3  // ���ý���ڵ��б��л�
#define USER_CMD_DISPP_AGECHANGE 	 0XC4  // ��ʾ����ҳ���л� 

// TFT ҳ�涨��
#define TFT_DISPNODE_NUM 1
#define TFT_PAGE_SETNUM 2

// ��ҳ�ڵ����
#define ONEPAGENODE 6 

extern u8 g_RequestNodeElecFlag; 
extern u8 g_currentRquesNodeIndex;
extern u8 g_nodeTotalCount;
extern u8 g_currentPage;
extern u8 g_currentNodeListPage;
extern u8 sendNodeDatabuf[12];
 
void buildAndSendStr2TFT(u8 pagid, u8 ctrlid, char* str);
void getTFTText(u8 pagid, u8 ctrlid);
void dispElec2TFT(NodeInfo *info);
void dispSetTips(char* tip);
void rebootTFT(void);
void getListPagef1(void);
void disp_hindTFTContrl(u8 pageid, u8 contrlid, u8 isdisplay);
void TFTanalysis(void); 
#endif
