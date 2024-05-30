/**
  ******************************************************************************
  * @file    hk32f030xmxxa_awut.c
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa_awut.h"

/** @addtogroup HK32F030xMxxA_StdPeriph_Driver
  * @{
  */

/** @defgroup AWUT  AWUT
* @brief AWUTS driver modules
* @{
*/

/* Public functions ----------------------------------------------------------*/

/**
  * @defgroup AWUT_Public_Functions AWUT_Public_Functions
  * @{
  */

/**
  * @brief  Deinitializes the AWUT peripheral registers to their default reset
  * values.
  * @note   None
  * @retval None
  */
void AWUT_DeInit(void)
{
  AWUT->CR = AWUT_CR_RESET_VALUE;
}

/**
  * @brief  config the AWUT clock
  * @param   eAWUT_CLK :
  *  arg AWUT_CLK_LSI
  *  arg AWUT_CLK_IO
  * @retval None
  * @par Required preconditions:
  * The LS RC calibration must be performed before calling this function.
  */
void AWUT_CLKConfig(AWUT_CLK_TYPE eAWUT_CLK)
{
  uint32_t temp = 0;
  /* Check parameter */
  assert_param(IS_AWUT_CLK(eAWUT_CLK));

  temp =  AWUT->CR;
  /*clear Bit AWUT_CKSEL*/
  temp &= 0xFFFFFFFE;
  /* config AWUT timer clk*/
  temp |= eAWUT_CLK;
  /*set the register*/
  AWUT->CR |= temp;
}

/**
  * @brief  loade the AWUT timer counter,This load value will be automatically loaded into the 22-bit timer inside the AWUT
  *  when the mcu enters stop mode and start timing.
  * @param   TimerCounter : the AWUT timer counter
  * @note When AWUT_rlr [22:1] is '0' or is '1' , the loading behavior will not occur and AWUT will not start working .
  *       when AWUT_wbusy =1 ,the write operation on the AWUT-rlr register will be invalid.
  * @return ErrorStatus: the AWUT result
  *       SUCCESS:AWUT timer start success
  *       ERROR:AWUT timer start error
  */
ErrorStatus AWUT_TimerCounterAndStart(uint32_t TimerCounter)
{
  uint32_t temp = 0;
  uint32_t TimeoutCnt = 0;

  while (TimeoutCnt ++ <= 0x0fff)
  {
    //  AWUT_APB bus is idle
    if ((AWUT->CR & 0x80000000) == 0x00000000)
    {
      temp = AWUT->CR;
      temp &= 0xFF800001;
      temp |= ( TimerCounter << 1);
      AWUT->CR |= temp;
      return SUCCESS;
    }
    else
    {
      /* when AWUT_wbusy =1 ,the write operation on the AWUT-rlr register will be invalid.*/
    }
  }

  return ERROR;

}

/**
  * @brief  Returns status of the AWUT peripheral flag.
  * @note  None
  * @retval FlagStatus : Status of the AWUT flag.
  */
FlagStatus AWUT_GetFlagStatus(void)
{
  return((FlagStatus)(((uint8_t)(AWUT->SR & AWUT_SR_BUSY) == (uint8_t)0x00) ? RESET : SET));
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
