/**
  ******************************************************************************
  * @file    hk32f030xmxxa_flash.h
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0 
  ******************************************************************************
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F030XMXXA_FLASH_H
#define __HK32F030XMXXA_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa.h"


/** @addtogroup FLASH
  * @{
  */

/** @defgroup FLASH_Exported_Types FLASH_Exported_Types
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** FLASH Status  */ 
typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH_Exported_Constants
  * @{
  */

/** @defgroup FLASH_Latency FLASH_Latency
  * @{
  */

/** FLASH_Latency   */ 
#define FLASH_Latency_0                ((uint32_t)0x00000000)      /*HCLK=16Mhz*/
#define FLASH_Latency_1                ((uint32_t)0x00000001)      /*16Mhz<HCLK<=32Mhz*/
#define FLASH_Latency_2                ((uint32_t)0x00000002)      /*32Mhz<HCLK<=48Mhz*/
#define FLASH_Latency_3                ((uint32_t)0x00000003)     


#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_Latency_0) || \
                   ((LATENCY) == FLASH_Latency_1) || \
                   ((LATENCY) == FLASH_Latency_2) || \
                                   ((LATENCY) == FLASH_Latency_3))

/**
  * @}
  */
/** @defgroup FLASH_Interrupts FLASH_Interrupts
  * @{
  */
/**  FLASH_Interrupts   */   
#define FLASH_IT_EOP                   FLASH_CR_EOPIE  /*!< End of programming interrupt source */
#define FLASH_IT_ERR                   FLASH_CR_ERRIE  /*!< Error interrupt source */
#define IS_FLASH_IT(IT) ((((IT) & (uint32_t)0xFFFFEBFF) == 0x00000000) && (((IT) != 0x00000000)))
/**
  * @}
  */
  
/** @defgroup FLASH_Address FLASH_Address
  * @{
  */
/*  FLASH_Address  16K devices */
#define IS_FLASH_PROGRAM_ADDRESS(ADDRESS) (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x08003FFF))

#define FLASH_OB_RDP_ADDRESS       0x1FFFF800
#define FLASH_OB_USER_ADDRESS     0x1FFFF802
#define FLASH_OB_DATA0_ADDRESS     0x1FFFF804
#define FLASH_OB_DATA1_ADDRESS     0x1FFFF806
#define FLASH_OB_WRP_ADDRESS       0x1FFFF808
#define FLASH_OB_IWDG_ADDRESS     0x1FFFF810
#define FLASH_OB_LSI_LP_ADDRESS   0x1FFFF814
#define FLASH_OB_DBG_CLK_ADDRESS   0x1FFFF816

/*  EEPROM_Address  16K devices */
#define IS_EEPROM_PROGRAM_ADDRESS(ADDRESS) (((ADDRESS) >= 0x0C000000) && ((ADDRESS) <= 0x0C0001C0))
/**
  * @}
  */
  
/** @defgroup FLASH_Option_Bytes_Write_Protection FLASH_Option_Bytes_Write_Protection
  * @{
  */
/**  FLASH_Option_Bytes_Write_Protection   */
#define OB_WRP_Pages0to7               ((uint32_t)0x00000001) /* Write protection of page 0 to 7 */ 
#define OB_WRP_Pages8to15              ((uint32_t)0x00000002) /* Write protection of page 8 to 15 */ 
#define OB_WRP_Pages16to23             ((uint32_t)0x00000004) /* Write protection of page 16 to 23 */ 
#define OB_WRP_Pages24to31             ((uint32_t)0x00000008) /* Write protection of page 24 to 31 */ 
#define OB_WRP_Pages32to39             ((uint32_t)0x00000010) /* Write protection of page 32 to 39 */ 
#define OB_WRP_Pages40to47             ((uint32_t)0x00000020) /* Write protection of page 40 to 47 */ 
#define OB_WRP_Pages48to55             ((uint32_t)0x00000040) /* Write protection of page 48 to 55 */ 
#define OB_WRP_Pages56to63             ((uint32_t)0x00000080) /* Write protection of page 56 to 63 */ 
#define OB_WRP_Pages64to71             ((uint32_t)0x00000100) /* Write protection of page 64 to 71 */ 
#define OB_WRP_Pages72to79             ((uint32_t)0x00000200) /* Write protection of page 72 to 79 */ 
#define OB_WRP_Pages80to87             ((uint32_t)0x00000400) /* Write protection of page 80 to 87 */ 
#define OB_WRP_Pages88to95             ((uint32_t)0x00000800) /* Write protection of page 88 to 95 */ 
#define OB_WRP_Pages96to103            ((uint32_t)0x00001000) /* Write protection of page 96 to 103 */ 
#define OB_WRP_Pages104to111           ((uint32_t)0x00002000) /* Write protection of page 104 to 111 */ 
#define OB_WRP_Pages112to119           ((uint32_t)0x00004000) /* Write protection of page 112 to 119 */ 
#define OB_WRP_Pages120to127           ((uint32_t)0x00008000) /* Write protection of page 120 to 127 */ 

#define OB_WRP_Pages128to135           ((uint32_t)0x00010000) /* Write protection of page 128 to 135 */ 
#define OB_WRP_Pages136to143           ((uint32_t)0x00020000) /* Write protection of page 136 to 143 */ 
#define OB_WRP_Pages144to151           ((uint32_t)0x00040000) /* Write protection of page 144 to 151 */ 
#define OB_WRP_Pages152to159           ((uint32_t)0x00080000) /* Write protection of page 152 to 159 */ 
#define OB_WRP_Pages160to167           ((uint32_t)0x00100000) /* Write protection of page 160 to 167 */ 
#define OB_WRP_Pages168to175           ((uint32_t)0x00200000) /* Write protection of page 168 to 175 */ 
#define OB_WRP_Pages176to183           ((uint32_t)0x00400000) /* Write protection of page 176 to 183  */ 
#define OB_WRP_Pages184to191           ((uint32_t)0x00800000) /* Write protection of page 184 to 191 */ 
#define OB_WRP_Pages192to199           ((uint32_t)0x01000000) /* Write protection of page 192 to 199 */ 
#define OB_WRP_Pages200to207           ((uint32_t)0x02000000) /* Write protection of page 200 to 207  */ 
#define OB_WRP_Pages208to215           ((uint32_t)0x04000000) /* Write protection of page 208 to 215 */ 
#define OB_WRP_Pages216to223           ((uint32_t)0x08000000) /* Write protection of page 216 to 223  */ 
#define OB_WRP_Pages224to231           ((uint32_t)0x10000000) /* Write protection of page 224 to 231 */ 
#define OB_WRP_Pages232to239           ((uint32_t)0x20000000) /* Write protection of page 232 to 239  */ 
#define OB_WRP_Pages240to247           ((uint32_t)0x40000000) /* Write protection of page 240 to 247  */ 
#define OB_WRP_Pages248to255           ((uint32_t)0x80000000) /* Write protection of page 248 to 255 */ 

#define OB_WRP_AllPages                ((uint32_t)0xFFFFFFFF) /*!< Write protection of all Sectors */
#define OB_WRP_None                    ((uint32_t)0x00000000) /*!< Write protection of none */
/**
  * @}
  */
  
/** @defgroup FLASH_Option_Bytes_Read_Protection FLASH_Option_Bytes_Read_Protection
  * @{
  */
/**  FLASH_Option_Bytes_Read_Protection   */ 

/**  FLASH_Read Protection Level   */ 
#define OB_RDP_Level_0   ((uint16_t)0x55aa)
#define OB_RDP_Level_1   ((uint16_t)0x44bb)
/*#define OB_RDP_Level_2   ((uint16_t)0x33cc)*/ /* Warning: When enabling read protection level 2 
                                                it's no more possible to go back to level 1 or 0 */

#define IS_OB_RDP(LEVEL) (((LEVEL) == OB_RDP_Level_0)||\
                          ((LEVEL) == OB_RDP_Level_1))/*||\
                          ((LEVEL) == OB_RDP_Level_2))*/          
/**
  * @}
  */
  
/** @defgroup FLASH_Option_Bytes_IWatchdog FLASH_Option_Bytes_IWatchdog
  * @{
  */
/**  FLASH_Option_Bytes_IWatchdog   */

#define OB_IWDG_SW                     ((uint16_t)0x0001)  /*!< Software IWDG selected */
#define OB_IWDG_HW                     ((uint16_t)0x0000)  /*!< Hardware IWDG selected */
#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))

/**
  * @}
  */
/** @defgroup FLASH_Option_Bytes_nRST_STOP FLASH_Option_Bytes_nRST_STOP
  * @{
  */
/** defgroup FLASH_Option_Bytes_nRST_STOP   */

#define OB_STOP_NoRST                  ((uint16_t)0x0002) /*!< No reset generated when entering in STOP */
#define OB_STOP_RST                    ((uint16_t)0x0000) /*!< Reset generated when entering in STOP */
#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NoRST) || ((SOURCE) == OB_STOP_RST))

/**
  * @}
  */
  /** @defgroup FLASH_ADDRESS FLASH_ADDRESS
  * @{
  */
 /**
  * @brief  Macro used by the assert function in order to check the different 
  *         sensitivity values for the option bytes Address
  */

#define OPTION_BYTE_START_DATA1_ADDRESS  ((uint32_t)0x1FFFF804)
#define OPTION_BYTE_END_DATA1_ADDRESS  ((uint32_t)0x1FFFF806)

#define IS_OB_DATA_ADDRESS(ADDRESS)  (((ADDRESS) >= OPTION_BYTE_START_DATA1_ADDRESS) && \
    ((ADDRESS) <= OPTION_BYTE_END_DATA1_ADDRESS))
  /**
  * @}
  */
 
  /** @defgroup FLASH_Flags FLASH_Flags
  * @{
  */
/**  FLASH_Flags   */ 

#define FLASH_FLAG_BSY                 FLASH_SR_BSY     /*!< FLASH Busy flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPRTERR  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP     /*!< FLASH End of Programming flag */
 
#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCB) == 0x00000000) && ((FLAG) != 0x00000000))
#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_EOP))
/**
  * @}
  */
  
/** @defgroup FLASH_Timeout_definition FLASH_Timeout_definition
  * @{
  */
/** FLASH_Timeout_definition   */ 
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000FFFFF)
/**
  * @}
  */

/** @defgroup FLASH_Legacy FLASH_Legacy
  * @{
  */
/** FLASH_Legacy   */
#define FLASH_WRProt_Pages0to3         OB_WRP_Pages0to3
#define FLASH_WRProt_Pages4to7         OB_WRP_Pages4to7
#define FLASH_WRProt_Pages8to11         OB_WRP_Pages8to11
#define FLASH_WRProt_Pages12to15     OB_WRP_Pages12to15
#define FLASH_WRProt_Pages16to19     OB_WRP_Pages16to19
#define FLASH_WRProt_Pages20to23     OB_WRP_Pages20to23
#define FLASH_WRProt_Pages24to27     OB_WRP_Pages24to27
#define FLASH_WRProt_Pages28to31     OB_WRP_Pages28to31
#define FLASH_WRProt_Pages32to35     OB_WRP_Pages32to35
#define FLASH_WRProt_Pages36to39     OB_WRP_Pages36to39
#define FLASH_WRProt_Pages40to43     OB_WRP_Pages40to43
#define FLASH_WRProt_Pages44to47     OB_WRP_Pages44to47
#define FLASH_WRProt_Pages48to51     OB_WRP_Pages48to51
#define FLASH_WRProt_Pages52to55     OB_WRP_Pages52to55
#define FLASH_WRProt_Pages56to59     OB_WRP_Pages56to59
#define FLASH_WRProt_Pages60to63     OB_WRP_Pages60to63
#define FLASH_WRProt_Pages64to67         OB_WRP_Pages64to67  
#define FLASH_WRProt_Pages68to71         OB_WRP_Pages68to71  
#define FLASH_WRProt_Pages72to75         OB_WRP_Pages72to75  
#define FLASH_WRProt_Pages76to79         OB_WRP_Pages76to79  
#define FLASH_WRProt_Pages80to83         OB_WRP_Pages80to83  
#define FLASH_WRProt_Pages84to87         OB_WRP_Pages84to87  
#define FLASH_WRProt_Pages88to91         OB_WRP_Pages88to91  
#define FLASH_WRProt_Pages92to95         OB_WRP_Pages92to95  
#define FLASH_WRProt_Pages96to99         OB_WRP_Pages96to99  
#define FLASH_WRProt_Pages100to103       OB_WRP_Pages100to103
#define FLASH_WRProt_Pages104to107       OB_WRP_Pages104to107
#define FLASH_WRProt_Pages108to111       OB_WRP_Pages108to111
#define FLASH_WRProt_Pages112to115       OB_WRP_Pages112to115
#define FLASH_WRProt_Pages116to119       OB_WRP_Pages116to119
#define FLASH_WRProt_Pages120to123       OB_WRP_Pages120to123
#define FLASH_WRProt_Pages124to127       OB_WRP_Pages124to127


#define FLASH_WRProt_AllPages          OB_WRP_AllPages


/**
  * @}
  */
 
/**
  * @}
  */
/**
  * @}
  */  
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
  
 
/* FLASH Interface configuration functions ************************************/
void FLASH_SetLatency(uint32_t FLASH_Latency);


/* FLASH Memory Programming functions *****************************************/
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

/* FLASH Option Bytes Programming functions *****************************************/
void FLASH_OB_Unlock(void);
void FLASH_OB_Lock(void);
FLASH_Status FLASH_OB_EraseByte(uint32_t Address);

FLASH_Status FLASH_OB_WRPConfig(uint32_t OB_WRP);
FLASH_Status FLASH_OB_RDPConfig(uint16_t OB_RDP);
FLASH_Status FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP );
FLASH_Status FLASH_OB_IWDG_RLRConfig(uint16_t OB_IWDG_RLR, FunctionalState NewState);
FLASH_Status FLASH_OB_LSILPConfig(FunctionalState NewState);
FLASH_Status FLASH_OB_DBGCLKConfig(FunctionalState NewState);
FLASH_Status FLASH_OB_WriteUser(uint16_t OB_USER);
FLASH_Status FLASH_OB_ProgramData(uint32_t Address, uint16_t Data);
uint8_t FLASH_OB_GetUser(void);
uint32_t FLASH_OB_GetWRP(void);
FlagStatus FLASH_OB_GetRDP(void);

/* FLASH Interrupts and flags management functions **********************************/
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);


FLASH_Status EEPROM_EraseByte(uint32_t Address);
FLASH_Status EEPROM_ProgramByte(uint32_t Address, uint8_t Data);


void Sys_GetDevice64BitUID(uint32_t *UID);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F030XMXXA_FLASH_H */


