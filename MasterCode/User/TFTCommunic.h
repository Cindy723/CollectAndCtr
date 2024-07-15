#ifndef _TFTCOMMUNIC_H
#define _TFTCOMMUNIC_H
 
#include "main.h"  

 // TFT��ָ��
#define BUTTON_CMD 0XB1
#define USER_CMD 	 0XCD

#define USER_CMD_REGISTER 	 0XC1 			 // ע�ᰴť����
#define USER_CMD_PSWENTER 	 0XC2  			 // ���ý��������������
#define USER_CMD_NODE_PAGECHANGE 	 0XC3  // ���ý���ڵ��б��л�
#define USER_CMD_DISP_PAGECHANGE 	 0XC4  // ��ʾ����ҳ���л� 
#define USER_CMD_DELETE_NODE		 	 0XC5  // ���ý���ɾ��ָ�� 

// TFT ҳ�涨��
#define TFT_DISP_PAGE 1
#define TFT_SET_PAGE 2

// ��ҳ�ڵ����
#define ONEPAGENODE 6 

// TFT ��ǰ��һЩ��Ҫ�����洢
typedef struct
{
	u8 Page; // ��ǰҳ��
	u8 setPageNodeListF1; // ���ý����µķ���
	u8 setListF2;				  // ���ú���ʾ�����µķ�ĸ
	u8 DispListPage;   		// ��ʾ�����µķ��� 
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
