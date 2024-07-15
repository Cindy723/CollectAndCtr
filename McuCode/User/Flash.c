#include "flash.h"
#include "init.h"
#include "tools.h"
 
/* MCU OTA */
/*擦除指定的Flash页*/
typedef struct
{
  uint32_t TypeErase;   /*!< TypeErase: Mass erase or page erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */
  
  uint32_t Banks;       /*!< Select banks to erase when Mass erase is enabled.
                             This parameter must be a value of @ref FLASHEx_Banks */    
  
  uint32_t PageAddress; /*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled
                             This parameter must be a number between Min_Data = 0x08000000 and Max_Data = FLASH_BANKx_END 
                             (x = 1 or 2 depending on devices)*/
  
  uint32_t NbPages;     /*!< NbPages: Number of pagess to be erased.
                             This parameter must be a value between Min_Data = 1 and Max_Data = (max number of pages - value of initial page)*/
                                                          
} FLASH_EraseInitTypeDef;


/** @defgroup FLASHEx_Type_Erase Type Erase
  * @{
  */
void flash_erase_page(uint8_t flashPage , uint32_t addr_base)
{ 
    FLASH_EraseInitTypeDef f; 
		FLASH_Status status;
		u8 i = 0; 
		FLASH_Unlock();

    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr_base+flashPage*FLASH_PAGE_SIZE;
    f.NbPages = 1;
	 
	  FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
	  
		while(i++ < 5)
		{ 
		    status = FLASH_ErasePage(f.PageAddress);
				if(FLASH_COMPLETE != status) 		
				{			
					printf("flash_erase_page Faild \n"); 
					FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
				}
				else 
				{
					break;
				}
		}
	  if(i >=5)
		{
		  printf("flash_erase_page Faild > 200 !!! lasterro: %d\n", (u8)status); 
		}
	
		FLASH_Lock();
}

void flash_erase(uint32_t size , uint32_t addr_base)
{
    uint32_t flashPageSum;
		uint32_t i;
    /*如果小于1024做处理*/
    if(size < FLASH_PAGE_SIZE)
        size = FLASH_PAGE_SIZE;												//
    /* 计算需要擦写的Flash页 */
    if((size % FLASH_PAGE_SIZE) == 0)
    {
        flashPageSum = size / FLASH_PAGE_SIZE;				//小于一页擦除一页
    }
    else
    {
        flashPageSum = (size / FLASH_PAGE_SIZE) + 1;	//大于一页擦除n+1页
    }
    for(i = 0;i<flashPageSum;i++)
    { 
			printf("flash_erase_page i %d add %x\n", i, addr_base); 
			flash_erase_page(i,addr_base);								//基址累加擦除flash
    }
}
  
void readFlash(uint16_t * buf_to_get,uint16_t len , uint32_t readFlashAddr)
{
	uint16_t count=0;
	while(count<len)
	{
	 	buf_to_get[count]=*(uint16_t *)(readFlashAddr + count*2);
		count++;
	}
}


/**********************************************************************************************************
@ 功能: 读取flash
@ 参数: 
@ 返回: 
@ 备注: 
*********************************************************************************************************/
void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr)
{
    uint16_t ReadFlashTempLen = 0;//读Flash长度
    if(len%2 == 0)
    {
        ReadFlashTempLen = len;
        readFlash((uint16_t *)buf_to_get,ReadFlashTempLen/2 , rFlashAddr);
    }
    else
    {
        ReadFlashTempLen = len + 1;//因为Flash只能读半字
        readFlash((uint16_t *)buf_to_get,ReadFlashTempLen/2 , rFlashAddr);
    }
}

/**********************************************************************************************************
 @ 功能: 写入flash
 @ 参数: 
 @ 返回: 
 @ 备注: 
 *********************************************************************************************************/
void wFlashData(uint32_t StartAddress, uint8_t* Data, uint32_t Length) 
{
    uint32_t address = StartAddress;
    uint32_t i; 
	 
    FLASH_Unlock();    
	 
    for (i = 0; i < Length; i += 2) {
        uint16_t halfword = Data[i] | (Data[i+1] << 8);
        FLASH_ProgramHalfWord(address + i, halfword);
    } 
    FLASH_Lock();
}
  
/****IAP*****/
typedef  void (*iapfun)(void);				//??????`э???.
iapfun jump2app; 
u16 iapbuf[1024];   

//#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//设置栈顶指针
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//??????ˇ???.
	{ 
		printf("Stack Success!\r\n");
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//????????????????(?λ??)		
		MSR_MSP(*(vu32*)appxaddr);					//???APP????(??????????????????)
		jump2app();									//???APP.
	}
	else
	{
		printf("Stack Failed!\r\n");
	}
}
