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
 * 使用HSE时，设置系统时钟的步骤
 * 1、开启HSE ，并等待 HSE 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/* 设置 系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,最高只能是36M
 * 参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
 * 举例：User_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：8MHZ * 9 = 72MHZ
 *       User_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：8MHZ * 16 = 128MHZ，超频慎用
 *
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */

/* 设置 系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,最高只能是36M
 * 参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
 * 举例：HSE_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：8MHZ * 9 = 72MHZ
 *       HSE_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：8MHZ * 16 = 128MHZ，超频慎用
 *
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */

void HSE_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

    // 把RCC外设初始化成复位状态，这句是必须的
    RCC_DeInit();

    //使能HSE，开启外部晶振，秉火开发板用的是8M
    RCC_HSEConfig(RCC_HSE_ON);

    // 等待 HSE 启动稳定
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    // 只有 HSE 稳定之后则继续往下执行
    if (HSEStartUpStatus == SUCCESS)
    {
        //----------------------------------------------------------------------//
        // 使能FLASH 预存取缓冲区
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
        //----------------------------------------------------------------------//

        // AHB预分频因子设置为1分频，HCLK = SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        // APB2预分频因子设置为1分频，PCLK2 = HCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);

        // APB1预分频因子设置为1分频，PCLK1 = HCLK/2
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //-----------------设置各种频率主要就是在这里设置-------------------//
        // 设置PLL时钟来源为HSE，设置PLL倍频因子
        // PLLCLK = 8MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
        //------------------------------------------------------------------//

        // 开启PLL
        RCC_PLLCmd(ENABLE);

        // 等待 PLL稳定
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    {   // 如果HSE开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
        // 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
        // HSI是内部的高速时钟，8MHZ
        while (1)
        {
        }
    }
}

/*
 * 使用HSI时，设置系统时钟的步骤
 * 1、开启HSI ，并等待 HSI 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/* 设置 系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,最高只能是36M
 * 参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
 * 举例：HSI_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：4MHZ * 9 = 72MHZ
 *       HSI_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：4MHZ * 16 = 64MHZ
 *
 * HSI作为时钟来源，经过PLL倍频作为系统时钟，这是在HSE故障的时候才使用的方法
 * HSI会因为温度等原因会有漂移，不稳定，一般不会用HSI作为时钟来源，除非是迫不得已的情况
 * 如果HSI要作为PLL时钟的来源的话，必须二分频之后才可以，即HSI/2，而PLL倍频因子最大只能是16
 * 所以当使用HSI的时候，SYSCLK最大只能是4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

    // 把RCC外设初始化成复位状态，这句是必须的
    RCC_DeInit();

    //使能HSI
    RCC_HSICmd(ENABLE);

    // 等待 HSI 就绪
    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

    // 只有 HSI就绪之后则继续往下执行
    if (HSIStartUpStatus == RCC_CR_HSIRDY)
    {
        //----------------------------------------------------------------------//
        // 使能FLASH 预存取缓冲区
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
        //----------------------------------------------------------------------//

        // AHB预分频因子设置为1分频，HCLK = SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        // APB2预分频因子设置为1分频，PCLK2 = HCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);

        // APB1预分频因子设置为1分频，PCLK1 = HCLK/2
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //-----------------设置各种频率主要就是在这里设置-------------------//
        // 设置PLL时钟来源为HSE，设置PLL倍频因子
        // PLLCLK = 4MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
        //------------------------------------------------------------------//

        // 开启PLL
        RCC_PLLCmd(ENABLE);

        // 等待 PLL稳定
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    {   // 如果HSI开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
        // 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
        // HSI是内部的高速时钟，8MHZ
        while (1)
        {
        }
    }
}

void RCC_Configuration(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //中断复用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//串口【1】
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//串口【2】
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//串口【3】
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);        //解除RTC写保护

}



void LED_Configuration()
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;  
// 目标板子
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
  //C8T6 小开发板测试
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
 @ 功能： 日志输出脚配置
 @ 入口： 
 @ 出口： 
 @ 备注：
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
 @ 功能： 485方向控制脚配置
 @ 入口： 
 @ 出口： 
 @ 备注：
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
 @ 功能： TPS2492 GPIO配置
 @ 入口： 
 @ 出口： 
 @ 备注：
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
 @ 功能： 串口GPIO配置
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void Usart_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure_USART1; //串口【1】
    GPIO_InitTypeDef GPIO_InitStructure_USART2; //串口【2】 
    GPIO_InitTypeDef GPIO_InitStructure_USART3; //串口【3】
    GPIO_InitTypeDef GPIO_InitStructure_IO_USART; //模拟串口 PF6 TXD

    //----------串口【1】外设配置
    GPIO_InitStructure_USART1.GPIO_Pin = GPIO_Pin_9; //TX
    GPIO_InitStructure_USART1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_USART1.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART1);

    GPIO_InitStructure_USART1.GPIO_Pin = GPIO_Pin_10; //RX
    GPIO_InitStructure_USART1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART1);

    //----------串口【2】外设配置
//    GPIO_InitStructure_USART2.GPIO_Pin = GPIO_Pin_2; //TX
//    GPIO_InitStructure_USART2.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure_USART2.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure_USART2);

    GPIO_InitStructure_USART2.GPIO_Pin = GPIO_Pin_3; //RX
    GPIO_InitStructure_USART2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure_USART2);

    //----------串口【3】外设配置
    GPIO_InitStructure_USART3.GPIO_Pin = GPIO_Pin_10; //TX
    GPIO_InitStructure_USART3.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_USART3.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure_USART3);

    GPIO_InitStructure_USART3.GPIO_Pin = GPIO_Pin_11; //RX
    GPIO_InitStructure_USART3.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure_USART3);


		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
		// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能 
		 

    GPIO_InitStructure_IO_USART.GPIO_Pin = GPIO_Pin_1; //TX
    GPIO_InitStructure_IO_USART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_IO_USART.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure_IO_USART); 
		 
}

 
/***********************************************************************************************************
 @ 功能： NVIC
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/  
void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure_USART1; //串口1中断
	NVIC_InitTypeDef NVIC_InitStructure_USART2; //串口2中断 
	NVIC_InitTypeDef NVIC_InitStructure_TIM3; //定时器3

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


	//----------串口【1】配置 
	NVIC_InitStructure_USART1.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure_USART1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_USART1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure_USART1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_USART1);

	//----------串口【2】配置 
	NVIC_InitStructure_USART2.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure_USART2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure_USART2.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure_USART2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_USART2); 
	
	//----------TIM3配置
	NVIC_InitStructure_TIM3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure_TIM3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_TIM3);
}

  
/***********************************************************************************************************
 @ 功能： 串口模块初始化
 @ 入口： 
 @ 出口： 
 @ 备注：
 *********************************************************************************************************/
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;        //串口【1】
    USART_InitTypeDef USART_InitStructure_USART2; //串口【2】
    USART_InitTypeDef USART_InitStructure_USART3; //串口【3】
		Usart_GPIO_Configuration();
	
    //----------串口【1】配置
    USART_InitStructure.USART_BaudRate = 115200;                                    //一般设置为115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式

    USART_Init(USART1, &USART_InitStructure);      //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启中断
    USART_Cmd(USART1, ENABLE);                     //使能串口
    USART_ClearFlag(USART1, USART_FLAG_TC);
    //----------串口【2】配置
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
		
    //----------串口【3】配置
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

//----------闹钟初始化----------
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
 * 中断周期为 = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		/* 设置TIM2CLK 为 72MHZ */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    //TIM_DeInit(TIM2);
	
	/* 自动重装载寄存器周期的值(计数值) */
    TIM_TimeBaseStructure.TIM_Period=1000;
	
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
	  /* 时钟预分频数为72 */
    TIM_TimeBaseStructure.TIM_Prescaler= 71;
	
		/* 对外部时钟进行采样的时钟分频,这里没有用到 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
    TIM_Cmd(TIM2, ENABLE);																		
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);		/*先关闭等待使用*/    
}


void TIM_3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 启动定时器3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置定时器3的参数
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; // 72MHz / 7200 = 10kHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1; // 10ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 启用定时器3更新中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
		// START
	  TIM_Cmd(TIM3, ENABLE);
	
}


// 看门狗初始化函数
void WDG_Init(uint32_t timeout_ms) {
	uint32_t prescaler;
	uint32_t reload_value;
    // 1. 打开PWR和DBGMCU外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. 解锁PWR外设，以便对寄存器进行配置
    PWR_DeInit();

    // 3. 启用看门狗时钟
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // 4. 设置预分频值
    // 计算预分频值（计数时钟 = 40kHz，由APB1的16分频得到）
    prescaler = timeout_ms * 40 / 16;
    IWDG_SetPrescaler(prescaler);

    // 5. 设置重装载寄存器值
    // 注意，这个值必须在0到0xFFF之间，用来设定看门狗的溢出时间
    reload_value = timeout_ms * 40 / 16;
    IWDG_SetReload(reload_value);

    // 6. 使能看门狗
    IWDG_Enable();
}

//重定向c库函数printf到USART1 
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(DEBUGUSART, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUGUSART, USART_FLAG_TC) == RESET);		
	    
//#ifdef MINIBOARD	
//		// 改成IO模拟
//	  char output[1];
//		output[0] = (char)ch;
//		IO_USART_Send((u8*)output, sizeof(output));	 // 此时偶尔乱码是正常现象
//#endif	
		return (ch);
}

//重定向c库函数scanf到USART1
//int fgetc(FILE *f)
//{
//		/* 等待串口1输入数据 */
//		while (USART_GetFlagStatus(DEBUGUSART, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(DEBUGUSART);

//}

