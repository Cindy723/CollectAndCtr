#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_iwdg.h"
#include "main.h"
#include "Init.h"
#include "Tools.h"
#include "ADC.h"
 
BroadAddr broadAddr;
BroadCfg broadCfg;
DCPowerSta dcPowerSta;
 
/*
 * ʹ��HSEʱ������ϵͳʱ�ӵĲ���
 * 1������HSE �����ȴ� HSE �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������User_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��8MHZ * 9 = 72MHZ
 *       User_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��8MHZ * 16 = 128MHZ����Ƶ����
 *
 * HSE��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�����ͨ��������
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������HSE_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��8MHZ * 9 = 72MHZ
 *       HSE_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��8MHZ * 16 = 128MHZ����Ƶ����
 *
 * HSE��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�����ͨ��������
 */

void HSE_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

    // ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
    RCC_DeInit();

    //ʹ��HSE�������ⲿ���񣬱��𿪷����õ���8M
    RCC_HSEConfig(RCC_HSE_ON);

    // �ȴ� HSE �����ȶ�
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    // ֻ�� HSE �ȶ�֮�����������ִ��
    if (HSEStartUpStatus == SUCCESS)
    {
        //----------------------------------------------------------------------//
        // ʹ��FLASH Ԥ��ȡ������
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
        // ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
        // ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
        // 0��0 < SYSCLK <= 24M
        // 1��24< SYSCLK <= 48M
        // 2��48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
        //----------------------------------------------------------------------//

        // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);

        // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //-----------------���ø���Ƶ����Ҫ��������������-------------------//
        // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
        // PLLCLK = 8MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
        //------------------------------------------------------------------//

        // ����PLL
        RCC_PLLCmd(ENABLE);

        // �ȴ� PLL�ȶ�
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    {   // ���HSE����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
        // ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
        // HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
        while (1)
        {
        }
    }
}

/*
 * ʹ��HSIʱ������ϵͳʱ�ӵĲ���
 * 1������HSI �����ȴ� HSI �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������HSI_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��4MHZ * 9 = 72MHZ
 *       HSI_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��4MHZ * 16 = 64MHZ
 *
 * HSI��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�������HSE���ϵ�ʱ���ʹ�õķ���
 * HSI����Ϊ�¶ȵ�ԭ�����Ư�ƣ����ȶ���һ�㲻����HSI��Ϊʱ����Դ���������Ȳ����ѵ����
 * ���HSIҪ��ΪPLLʱ�ӵ���Դ�Ļ����������Ƶ֮��ſ��ԣ���HSI/2����PLL��Ƶ�������ֻ����16
 * ���Ե�ʹ��HSI��ʱ��SYSCLK���ֻ����4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

    // ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
    RCC_DeInit();

    //ʹ��HSI
    RCC_HSICmd(ENABLE);

    // �ȴ� HSI ����
    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

    // ֻ�� HSI����֮�����������ִ��
    if (HSIStartUpStatus == RCC_CR_HSIRDY)
    {
        //----------------------------------------------------------------------//
        // ʹ��FLASH Ԥ��ȡ������
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
        // ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
        // ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
        // 0��0 < SYSCLK <= 24M
        // 1��24< SYSCLK <= 48M
        // 2��48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
        //----------------------------------------------------------------------//

        // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);

        // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //-----------------���ø���Ƶ����Ҫ��������������-------------------//
        // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
        // PLLCLK = 4MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
        //------------------------------------------------------------------//

        // ����PLL
        RCC_PLLCmd(ENABLE);

        // �ȴ� PLL�ȶ�
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    {   // ���HSI����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
        // ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
        // HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
        while (1)
        {
        }
    }
}

void RCC_Configuration(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //�жϸ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//���ڡ�1��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//���ڡ�2��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//���ڡ�3��
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);        //���RTCд����

}



void LED_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
// Ŀ�����
	GPIO_InitStructure.GPIO_Pin = LEDRUNPINCFG | LED2PINCFG | LED3PINCFG; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
 
	GPIO_InitStructure.GPIO_Pin = TYPEPINCFG; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	
//  //log 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	
#ifdef MINIBOARD
  //C8T6 С���������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
#endif
}


void Relay_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = RELAY1PINCFG | RELAY2PINCFG;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);  
} 


/***********************************************************************************************************
 @ ���ܣ� ��־���������
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void LogPin_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);  
}  

/***********************************************************************************************************
 @ ���ܣ� 485������ƽ�����
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void RE485Pin_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);  
}

/***********************************************************************************************************
 @ ���ܣ� TPS2492 GPIO����
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void TPS2492Pin_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);  
}
/***********************************************************************************************************
 @ ���ܣ� ����GPIO����
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void Usart_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure_USART1; //���ڡ�1��
    GPIO_InitTypeDef GPIO_InitStructure_USART2; //���ڡ�2�� 
    GPIO_InitTypeDef GPIO_InitStructure_USART3; //���ڡ�3��
    GPIO_InitTypeDef GPIO_InitStructure_IO_USART; //ģ�⴮�� PF6 TXD

    //----------���ڡ�1����������
    GPIO_InitStructure_USART1.GPIO_Pin = GPIO_Pin_9; //TX
    GPIO_InitStructure_USART1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_USART1.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART1);

    GPIO_InitStructure_USART1.GPIO_Pin = GPIO_Pin_10; //RX
    GPIO_InitStructure_USART1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART1);

    //----------���ڡ�2����������
//    GPIO_InitStructure_USART2.GPIO_Pin = GPIO_Pin_2; //TX
//    GPIO_InitStructure_USART2.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure_USART2.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure_USART2);

    GPIO_InitStructure_USART2.GPIO_Pin = GPIO_Pin_3; //RX
    GPIO_InitStructure_USART2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART2);

    //----------���ڡ�3����������
    GPIO_InitStructure_USART3.GPIO_Pin = GPIO_Pin_10; //TX
    GPIO_InitStructure_USART3.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_USART3.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure_USART3);

    GPIO_InitStructure_USART3.GPIO_Pin = GPIO_Pin_11; //RX
    GPIO_InitStructure_USART3.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure_USART3);


		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
		// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ�� 
		 

    GPIO_InitStructure_IO_USART.GPIO_Pin = GPIO_Pin_1; //TX
    GPIO_InitStructure_IO_USART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_IO_USART.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure_IO_USART); 
		 
}

 
/***********************************************************************************************************
 @ ���ܣ� NVIC
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/  
void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure_USART1; //����1�ж�
	NVIC_InitTypeDef NVIC_InitStructure_USART2; //����2�ж� 
	NVIC_InitTypeDef NVIC_InitStructure_TIM3; //��ʱ��3

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


	//----------���ڡ�1������ 
	NVIC_InitStructure_USART1.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure_USART1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_USART1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure_USART1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_USART1);

	//----------���ڡ�2������ 
	NVIC_InitStructure_USART2.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure_USART2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure_USART2.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure_USART2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_USART2); 
	
	//----------TIM3����
	NVIC_InitStructure_TIM3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_TIM3);
}

  
/***********************************************************************************************************
 @ ���ܣ� ����ģ���ʼ��
 @ ��ڣ� 
 @ ���ڣ� 
 @ ��ע��
 *********************************************************************************************************/
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;        //���ڡ�1��
    USART_InitTypeDef USART_InitStructure_USART2; //���ڡ�2��
    USART_InitTypeDef USART_InitStructure_USART3; //���ڡ�3��
		Usart_GPIO_Configuration();
	
    //----------���ڡ�1������
    USART_InitStructure.USART_BaudRate = 115200;                                    //һ������Ϊ115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure);      //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�����ж�
    USART_Cmd(USART1, ENABLE);                     //ʹ�ܴ���
    USART_ClearFlag(USART1, USART_FLAG_TC);
    //----------���ڡ�2������
#ifdef TESTHMI_DC	
    USART_InitStructure_USART2.USART_BaudRate = 115200;
#else	 
		USART_InitStructure_USART2.USART_BaudRate = 4800;
#endif
    USART_InitStructure_USART2.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure_USART2.USART_StopBits = USART_StopBits_1;
    USART_InitStructure_USART2.USART_Parity = USART_Parity_No;
    USART_InitStructure_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure_USART2);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC);
		
    //----------���ڡ�3������
    USART_InitStructure_USART3.USART_BaudRate = 115200;
    USART_InitStructure_USART3.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure_USART3.USART_StopBits = USART_StopBits_1;
    USART_InitStructure_USART3.USART_Parity = USART_Parity_No;
    USART_InitStructure_USART3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure_USART3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure_USART3);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TC);  
}

//----------���ӳ�ʼ��----------
void RTC_Configuration(void)
{
	/* RTC clock source configuration ------------------------------------------*/
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	} 

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* RTC configuration -------------------------------------------------------*/
	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(40000);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

 /*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * �ж�����Ϊ = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> �ж� ��TIMxCNT����Ϊ0���¼��� 
 */
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		/* ����TIM2CLK Ϊ 72MHZ */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    //TIM_DeInit(TIM2);
	
	/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    TIM_TimeBaseStructure.TIM_Period=1000;
	
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
	  /* ʱ��Ԥ��Ƶ��Ϊ72 */
    TIM_TimeBaseStructure.TIM_Prescaler= 71;
	
		/* ���ⲿʱ�ӽ��в�����ʱ�ӷ�Ƶ,����û���õ� */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
    TIM_Cmd(TIM2, ENABLE);																		
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);		/*�ȹرյȴ�ʹ��*/    
}


void TIM_3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // ������ʱ��3ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // ���ö�ʱ��3�Ĳ���
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; // 72MHz / 7200 = 10kHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1; // 10ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // ���ö�ʱ��3�����ж�
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
		// START
	  TIM_Cmd(TIM3, ENABLE);
	
}


// ���Ź���ʼ������
void WDG_Init(uint32_t timeout_ms) {
	uint32_t prescaler;
	uint32_t reload_value;
    // 1. ��PWR��DBGMCU����ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. ����PWR���裬�Ա�ԼĴ�����������
    PWR_DeInit();

    // 3. ���ÿ��Ź�ʱ��
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // 4. ����Ԥ��Ƶֵ
    // ����Ԥ��Ƶֵ������ʱ�� = 40kHz����APB1��16��Ƶ�õ���
    prescaler = timeout_ms * 40 / 16;
    IWDG_SetPrescaler(prescaler);

    // 5. ������װ�ؼĴ���ֵ
    // ע�⣬���ֵ������0��0xFFF֮�䣬�����趨���Ź������ʱ��
    reload_value = timeout_ms * 40 / 16;
    IWDG_SetReload(reload_value);

    // 6. ʹ�ܿ��Ź�
    IWDG_Enable();
}

//�ض���c�⺯��printf��USART1 
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(DEBUGUSART, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUGUSART, USART_FLAG_TC) == RESET);		
	    
//#ifdef MINIBOARD	
//		// �ĳ�IOģ��
//	  char output[1];
//		output[0] = (char)ch;
//		IO_USART_Send((u8*)output, sizeof(output));	 // ��ʱż����������������
//#endif	
		return (ch);
}

//�ض���c�⺯��scanf��USART1
//int fgetc(FILE *f)
//{
//		/* �ȴ�����1�������� */
//		while (USART_GetFlagStatus(DEBUGUSART, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(DEBUGUSART);

//}

