/**
  ******************************************************************************
  * @file    hk32f030xmxxa_awut.h
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F030XMXXA_AWUT_H
#define __HK32F030XMXXA_AWUT_H

/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa.h"
 
  
/** @addtogroup AWUT  
  * @{
  */
  
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
  
/** @defgroup AWUT_Private_Macros AWUT_Private_Macros
  * @{
  */


#define AWUT_CR_RESET_VALUE 0x00000000U
#define AWUT_SR_RESET_VALUE 0x00000000U

typedef enum
{
  AWUT_CLK_LSI,
  AWUT_CLK_IO,
}AWUT_CLK_TYPE;

#define IS_AWUT_CLK(AWUT_CLK) \
  (((AWUT_CLK) == AWUT_CLK_LSI) || \
   ((AWUT_CLK) == AWUT_CLK_IO))
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

/** @defgroup AWUT_Exported_Functions AWUT_Exported_Functions
  * @{
  */
void AWUT_DeInit(void);
void AWUT_CLKConfig(AWUT_CLK_TYPE eAWUT_CLK);
ErrorStatus AWUT_TimerCounterAndStart(uint32_t TimerCounter);
FlagStatus AWUT_GetFlagStatus(void);
/**
  * @}
  */
  
  /**
  * @}
  */
  
 
#endif /* __HK32F030XMXXA_AWUT_H */

