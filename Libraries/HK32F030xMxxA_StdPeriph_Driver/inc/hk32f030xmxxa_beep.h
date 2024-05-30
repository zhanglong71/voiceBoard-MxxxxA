/**
  ******************************************************************************
  * @file    hk32f030xmxxa_beep.h
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
  ******************************************************************************
  */


#ifndef __HK32F030XMXXA_BEEP_H
#define __HK32F030XMXXA_BEEP_H

#ifdef __cplusplus
  extern "C"{
#endif  
    
#include "hk32f030xmxxa.h"
/** @addtogroup BEEP
  * @{
  */
  
/* Exported types ------------------------------------------------------------*/
/** @defgroup BEEP_Exported_Types BEEP_Exported_Types
  * @{
  */
/** 
  * @brief  BEEP Block Init structure definition  
  */
  
typedef struct
{
  uint8_t  BEEP_Prescaler;
  uint8_t  BEEP_Clock;
  uint8_t BEEP_TRGOPrescaler;
  FunctionalState BEEP_TRGOCmd;
}BEEP_InitTypeDef;
/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @defgroup BEEP_Exported_Macros BEEP_Exported_Macros
  * @{
  */   
#define BEEP_BUSY_FLAG             ((uint32_t)0x80000000U)
#define BEEP_CFGR_Value            ((uint32_t)0x0000000AU)
#define BEEP_CR_Value            ((uint32_t)0x00000003U)
#define BEEP_CR_BEEP_Mask             ((uint32_t)0xFFFFFFF9U)
#define BEEP_CR_TRGO_Mask              ((uint32_t)0xFFFFFFE7U)
    
#define BEEP_Prescaler_16          ((uint32_t)0x00000006U)
#define BEEP_Prescaler_32          ((uint32_t)0x00000004U)
#define BEEP_Prescaler_64          ((uint32_t)0x00000002U)
#define BEEP_Prescaler_128          ((uint32_t)0x00000000U)
#define IS_BEEP_PRESCALER(PRESCALER)      (((PRESCALER)==BEEP_Prescaler_16)  ||\
                      ((PRESCALER)==BEEP_Prescaler_32)  ||\
                      ((PRESCALER)==BEEP_Prescaler_64)  ||\
                      ((PRESCALER)==BEEP_Prescaler_128))


#define BEEP_CLOCK_HSE             ((uint32_t)0x00000001U)
#define BEEP_CLOCK_LSI             ((uint32_t)0x00000000U)

#define IS_BEEP_CLOCK(CLOCK)         ((CLOCK==(BEEP_CLOCK_HSE))||\
                      CLOCK==(BEEP_CLOCK_LSI))


#define BEEP_TRGO_Prescaler_32        ((uint32_t)0x00000010U)
#define BEEP_TRGO_Prescaler_64        ((uint32_t)0x00000008U)
#define BEEP_TRGO_Prescaler_128        ((uint32_t)0x00000000U)
#define IS_BEEP_TRGO_PRESCALER(PRESCALER)    (((PRESCALER)==BEEP_TRGO_Prescaler_32)  ||\
                      ((PRESCALER)==BEEP_TRGO_Prescaler_64)  ||\
                      ((PRESCALER)==BEEP_TRGO_Prescaler_128))

/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/ 
/** @defgroup BEEP_Exported_Functions BEEP_Exported_Functions
  * @{
  */
void BEEP_DeInit(void);
void BEEP_Init(BEEP_InitTypeDef * BEEP_InitStruct);
void BEEP_Cmd(FunctionalState NewState);
void BEEP_ClockSelect(uint8_t BEEP_CLOCK);
void BEEP_SetPrescaler(uint8_t BEEP_Prescaler);
void BEEP_SetTRGOPrescaler(uint8_t BEEP_TGRO_Prescaler);
FlagStatus BEEP_ReadBeepStatus(void);
void BEEP_TRGOCmd(FunctionalState NewState);
/**
  * @}
  */  
/**
  * @}
  */      
#ifdef __cplusplus
}
#endif

#endif
