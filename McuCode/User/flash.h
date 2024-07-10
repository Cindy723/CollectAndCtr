#ifndef _FLASH_
#define _FLASH_
#include "init.h"
   
#define CODE_SIZE 50*1024

#define STM32_FLASH_BASE 0x8000000
#define STM32_FLASH_SIZE 128
#define STM_SECTOR_SIZE 128


/* 地址和配置类参数存储 */
#define PARAM_SAVE_ADDR_BASE         STM32_FLASH_BASE + CODE_SIZE
#define PARAM_MAX_SIZE               1*1024
 

#define PIECE_MAX_LEN  256
#define FLASH_TYPEERASE_PAGES     0x00U  /*!<Pages erase only*/
#define FLASH_TYPEERASE_MASSERASE 0x02U  /*!<Flash mass erase activation*/
#define FLASH_PAGE_SIZE 0x400U

  
void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr);
void wFlashData(uint8_t * buf_to_save , uint16_t len , uint32_t wFlashAddr); 
void flash_erase(uint32_t size , uint32_t addr_base);
#endif
