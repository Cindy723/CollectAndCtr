#include "flash.h"
#include "init.h"
#include "tools.h"
 
/* MCU OTA */
/*����ָ����Flashҳ*/
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
	  
		while(i++ < 200)
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
	  if(i >=200)
		{
		  printf("flash_erase_page Faild > 200 !!! lasterro: %d\n", (u8)status); 
		}
	
		FLASH_Lock();
}

void flash_erase(uint32_t size , uint32_t addr_base)
{
    uint32_t flashPageSum;
		uint32_t i;
    /*���С��1024������*/
    if(size < FLASH_PAGE_SIZE)
        size = FLASH_PAGE_SIZE;												//
    /* ������Ҫ��д��Flashҳ */
    if((size % FLASH_PAGE_SIZE) == 0)
    {
        flashPageSum = size / FLASH_PAGE_SIZE;				//С��һҳ����һҳ
    }
    else
    {
        flashPageSum = (size / FLASH_PAGE_SIZE) + 1;	//����һҳ����n+1ҳ
    }
    for(i = 0;i<flashPageSum;i++)
    { 
			printf("flash_erase_page i %d add %x\n", i, addr_base); 
			flash_erase_page(i,addr_base);								//��ַ�ۼӲ���flash
    }
}
 

void writeFlash(uint16_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
    uint16_t count = 0,i = 0;
		FLASH_Status status;
	
    if(wFlashAddr >= 0x08010000)
    {
#ifdef DEBUG
        printf("Waring:Flash Write Addr Error\r\n");
#endif
        //flashWriteOffset = SYS_APP_BAK_SAVE_ADDR_BASE;
        return;
    } 
		
	  FLASH_Unlock();     
	  FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
		while(count < len)
		{ 
			  //FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
				status = FLASH_ProgramHalfWord((wFlashAddr + count*2),buf_to_save[count]);
				while(FLASH_COMPLETE != status) 
				{			
					FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
					i++;
					printf("FLASH_ProgramHalfWord ei %d, %d/%d \n", i, count, len);
					if(i > 5) 
					{ 
						FLASH_ClearFlag(FLASH_FLAG_EOP |   FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
						printf("FLASH_ProgramHalfWord Faild > 200 !!! lasterro: %d\n", (u8)status);
						FLASH_Lock();
						break;
					}
					continue;
				}
			  
				count ++;     
		}
		
		FLASH_Lock();
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

/*дFlash,����д����,Flash��ַƫ��*/
void wFlashData(uint8_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
    uint8_t WriteFlashTempBuf[PIECE_MAX_LEN];//дFlash��ʱ������
    uint16_t WriteFlashTempLen = 0;//дFlash����
    memset(WriteFlashTempBuf,0,sizeof(WriteFlashTempBuf));//дFlash��ʱ����������ȫ�����0xEE
    memcpy(WriteFlashTempBuf,buf_to_save,len);//��ʱ������
    WriteFlashTempLen = len;
    if(len%2 != 0)
        WriteFlashTempLen += 1;//��ΪFlashֻ��д����
		
    writeFlash((uint16_t *)&WriteFlashTempBuf ,  WriteFlashTempLen/2 , wFlashAddr);
}


void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr)
{
    uint8_t ReadFlashTempBuf[PIECE_MAX_LEN];//��Flash��ʱ������
    uint16_t ReadFlashTempLen = 0;//��Flash����
    
    if(len%2 == 0)
    {
        ReadFlashTempLen = len;
        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
        memcpy(buf_to_get,ReadFlashTempBuf,len);
    }
    else
    {
        ReadFlashTempLen = len + 1;//��ΪFlashֻ�ܶ�����
        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
        memcpy(buf_to_get,ReadFlashTempBuf,len);
    }
}
/****IAP*****/
typedef  void (*iapfun)(void);				//??????`��???.
iapfun jump2app; 
u16 iapbuf[1024];   

//#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//����ջ��ָ��
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//??????��???.
	{ 
		printf("Stack Success!\r\n");
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//????????????????(?��??)		
		MSR_MSP(*(vu32*)appxaddr);					//???APP????(??????????????????)
		jump2app();									//???APP.
	}
	else
	{
		printf("Stack Failed!\r\n");
	}
}
