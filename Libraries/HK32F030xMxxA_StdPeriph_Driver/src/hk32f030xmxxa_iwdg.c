/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
* @Filename: hk32f030xmxxa_iwdg.c
* @brief  : This file provides firmware functions to manage the following
  *          functionalities of the Independent watchdog (IWDG) peripheral:
  *           + Prescaler and Counter configuration
  *           + IWDG activation
  *           + Flag management
* @Author : AE Team
* @Version:V1.0.0/2022-05-14
* 1.初始版本
 ********************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa_iwdg.h"

/** @addtogroup HK32F030xMxxA_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDG IWDG
  * @brief IWDG driver modules
  * @{
    @verbatim
                          ##### IWDG features #####
    [..] The IWDG can be started by either software or hardware (configurable
         through option byte).

    [..] The IWDG is clocked by its own dedicated low-speed clock (LSI) and
         thus stays active even if the main clock fails.
         Once the IWDG is started, the LSI is forced ON and cannot be disabled
         (LSI cannot be disabled too), and the counter starts counting down from
         the reset value of 0xFFF. When it reaches the end of count value (0x000)
         a system reset is generated.
         The IWDG counter should be reloaded at regular intervals to prevent
         an MCU reset.

    [..] The IWDG is implemented in the VDD voltage domain that is still functional
         in STOP and STANDBY mode (IWDG reset can wake-up from STANDBY).

    [..] IWDGRST flag in RCC_CSR register can be used to inform when a IWDG
         reset occurs.

    [..] Min-max timeout value @114KHz (LSI): ~0.035ms / ~9.2s
         The IWDG timeout may vary due to LSI frequency dispersion. HK32F030xm
         devices provide the capability to measure the LSI frequency (LSI clock
         should be seleted as RTC clock which is internally connected to TIM10 CH1
         input capture). The measured value can be used to have an IWDG timeout with
         an acceptable accuracy.
         For more information, please refer to the HK32F030xm Reference manual.

                          ##### How to use this driver #####

        (++) Start the IWDG using IWDG_Enable() function, when the IWDG is used
             in software mode (no need to enable the LSI, it will be enabled
             by hardware).

        (++) Enable write access to IWDG_PR and IWDG_RLR registers using
             IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) function.

        (++) Configure the IWDG prescaler using IWDG_SetPrescaler() function.

        (++) Configure the IWDG counter value using IWDG_SetReload() function.
             This value will be loaded in the IWDG counter each time the counter
             is reloaded, then the IWDG will start counting down from this value.

        (++) Wait for the IWDG registers to be updated using IWDG_GetFlagStatus() function.

        (++) Configure the IWDG refresh window using IWDG_SetWindowValue() function.


       [..] 这个驱动程序允许使用IWDG外设与任一窗口选项启用或禁用。 要做到这一点，请遵循以下两个步骤之一。

       选项启用:

       (++)  当使用IWDG时，在软件模式下使用IWDG_Enable()函数启动IWDG(不需要使能LSI，它会通过硬件自动使能)。

       (++)  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable)的功能是启用对IWDG_PR和IWDG_RLR寄存器的写访问。

       (++)  使用IWDG_SetPrescaler()函数配置IWDG预选器。

       (++)  使用IWDG_SetReload()函数配置IWDG计数器值，该值将在每次计数器加载到IWDG计数器中，IWDG将从该值开始递减。

       (++)  使用IWDG_GetFlagStatus()函数等待IWDG寄存器更新。

       (++)  使用IWDG_SetWindowValue()函数配置IWDG刷新窗口。

  @endverbatim

*******************************************************************************
*/


/** @defgroup IWDG_Private_Functions IWDG_Private_Functions
  * @{
  */

#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)
#define KR_KEY_ENABLE    ((uint16_t)0xCCCC)

/**
  * @}
  */

/** @defgroup IWDG_Group1 Prescaler and Counter configuration functions
 *  @brief   Prescaler and Counter configuration functions
 *
@verbatim
  ==============================================================================
            ##### Prescaler and Counter configuration functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  IWDG_WriteAccess: new state of write access to IWDG_PR and IWDG_RLR registers.
  *          This parameter can be one of the following values:
  *            @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  *            @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None
  */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
  IWDG->KR = IWDG_WriteAccess;
}

/**
  * @brief  Sets IWDG Prescaler value.
  * @param  IWDG_Prescaler: specifies the IWDG Prescaler value.
  *          This parameter can be one of the following values:
  *            @arg IWDG_Prescaler_4: IWDG prescaler set to 4
  *            @arg IWDG_Prescaler_8: IWDG prescaler set to 8
  *            @arg IWDG_Prescaler_16: IWDG prescaler set to 16
  *            @arg IWDG_Prescaler_32: IWDG prescaler set to 32
  *            @arg IWDG_Prescaler_64: IWDG prescaler set to 64
  *            @arg IWDG_Prescaler_128: IWDG prescaler set to 128
  *            @arg IWDG_Prescaler_256: IWDG prescaler set to 256
  * @retval None
  */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
  IWDG->PR = IWDG_Prescaler;
}

/**
  * @brief  Sets IWDG Reload value.
  * @param  Reload: specifies the IWDG Reload value.
  *          This parameter must be a number between 0 and 0x0FFF.
  * @retval None
  */
void IWDG_SetReload(uint16_t Reload)
{
  /* Check the parameters */
  assert_param(IS_IWDG_RELOAD(Reload));
  IWDG->RLR = Reload;
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  *   (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @retval None
  */
void IWDG_ReloadCounter(void)
{
  IWDG->KR = KR_KEY_RELOAD;
}


/**
  * @brief  Sets the IWDG window value.
  * @param  WindowValue: specifies the window value to be compared to the downcounter.
  * @retval None
  */
void IWDG_SetWindowValue(uint16_t WindowValue)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WINDOW_VALUE(WindowValue));
  IWDG->WINR = WindowValue;
}

/**
  * @}
  */

/** @defgroup IWDG_Group2 IWDG activation function
 *  @brief   IWDG activation function
 *
@verbatim
 ==============================================================================
                          ##### IWDG activation function #####
 ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @retval None
  */
void IWDG_Enable(void)
{
  IWDG->KR = KR_KEY_ENABLE;
}

/**
  * @}
  */

/** @defgroup IWDG_Group3 Flag management function
 *  @brief  Flag management function
 *
@verbatim
 ===============================================================================
                      ##### Flag management function #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified IWDG flag is set or not.
  * @param  IWDG_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg IWDG_FLAG_PVU: Prescaler Value Update on going
  *            @arg IWDG_FLAG_RVU: Reload Value Update on going
  *            @arg IWDG_FLAG_WVU: Counter Window Value Update on going
  * @retval The new state of IWDG_FLAG (SET or RESET).
  */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_IWDG_FLAG(IWDG_FLAG));

  if ((IWDG->SR & IWDG_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }

  /* Return the flag status */
  return bitstatus;
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


