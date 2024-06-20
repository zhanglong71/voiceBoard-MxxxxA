
#include <string.h>
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "version.h"

#include "l_flash.h"
#include "hk32f030xmxxa.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000080)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08007E80)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08007F00)   /* End @ of user Flash area */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// uint32_t Data = 0x3210ABCD;
// __IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
// const char magic[] = "DIISEA";
/*******************************************************************************
 * flashPage_update: 128bytes/page
 *******************************************************************************/
RetStatus flashPage_update(char* inData)
{
    uint16_t *src = NULL;
    uint32_t Address = FLASH_USER_START_ADDR;
    /* Unlock the Flash to enable the flash control register access *************/
    FLASH_Unlock();
    
    /* Erase the user Flash area
       (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) *********/
    
    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);
    
    /* Erase the FLASH pages */
    if(FLASH_ErasePage(FLASH_USER_START_ADDR) != FLASH_COMPLETE) {
        /* Error occurred while sector erase.
            User can add here some code to deal with this error */
        return PERASE;
    }
    
    /* Program the user Flash area half-word by half-word
      (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
    // Address = FLASH_USER_START_ADDR;
    src = (uint16_t *)inData;
    while (Address < FLASH_USER_END_ADDR) {
        if (FLASH_ProgramHalfWord(Address, *src) == FLASH_COMPLETE) {
            Address = Address + 2;
            src++;
        } else {
            /* Error occurred while writing data in Flash memory.
               User can add here some code to deal with this error */
            return PWRITE;
        }
    }
    
    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    FLASH_Lock();

    return POK;
}

RetStatus userData_update(char* inData)
{
    inData[122] = 'D';
    inData[123] = 'I';
    inData[124] = 'I';
    inData[125] = 'S';
    inData[126] = 'E';
    inData[127] = 'A';  // magic 
    return flashPage_update(inData);
}

char* flashPage_get(char* outData)
{
    /** use uint16_t not char*, the reson is the operator consistent to FLASH_ProgramHalfWord() **/
    uint16_t* src = (uint16_t*)FLASH_USER_START_ADDR;
    uint16_t* dst = (uint16_t*)outData;
  
    for (int i = 0; i < (FLASH_PAGE_SIZE/sizeof(uint16_t)); i++) {
        dst[i] = src[i];
    }
    return (char*)FLASH_USER_START_ADDR;
}

