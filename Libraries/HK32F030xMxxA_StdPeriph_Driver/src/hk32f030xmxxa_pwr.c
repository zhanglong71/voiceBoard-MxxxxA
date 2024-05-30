/**
  ******************************************************************************
  * @file    hk32f030xmxxa_pwr.c
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa_pwr.h"
#include "hk32f030xmxxa_rcc.h"
#include "hk32f030xmxxa.h"

/** @addtogroup HK32F030xMxxA_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR  PWR
  * @brief PWR driver modules
  * @{
  */

/** @defgroup PWR_Private_Defines PWR_Private_Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 Backup Domain Access function
 *  @brief   Backup Domain Access function
 *
@verbatim
  ==============================================================================
                   ##### Backup Domain Access function #####
  ==============================================================================

    [..] After reset, the Backup Domain Registers (RCC BDCR Register, RTC registers
         and RTC backup registers) are protected against possible stray write accesses.
    [..] To enable access to Backup domain use the PWR_BackupAccessCmd(ENABLE) function.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @retval None
  */

void PWR_DeInit(void)
{
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_PWR, ENABLE);
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_PWR, DISABLE);
}

/**
  * @brief  Enables or disables access to the  backup registers.
  * @param  NewState: new state of the access to the  backup registers.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState == ENABLE)
  {
    PWR->CR |= PWR_DBP_MSK;
  }
  else
  {
    PWR->CR &= (uint32_t)~PWR_DBP_MSK;
  }
}

/**
  * @}
  */

/** @defgroup PWR_Group2 Low Power modes configuration functions
 *  @brief   Low Power modes configuration functions
 *
@verbatim
  ==============================================================================
              ##### Low Power modes configuration functions #####
  ==============================================================================

    [..] The devices feature three low-power modes:
    (+) Sleep mode: Cortex-M0 core stopped, peripherals kept running.
    (+) Stop mode: all clocks are stopped, regulator running, regulator in low power mode
    (+) Standby mode: VCORE domain powered off

  *** Sleep mode ***
  ==================
  [..]
    (+) Entry:
        (++) The Sleep mode is entered by executing the WFE() or WFI() instructions.
    (+) Exit:
        (++) Any peripheral interrupt acknowledged by the nested vectored interrupt
             controller (NVIC) can wake up the device from Sleep mode.

  *** Stop mode ***
  =================
  [..] In Stop mode, all clocks in the VCORE domain are stopped, the HSI are disabled.
          Internal SRAM and register contents are preserved.
       The voltage regulator can be configured either in normal or low-power mode.

    (+) Entry:
        (++) The Stop mode is entered using the PWR_EnterSTOPMode(PWR_Regulator_LowPower,)
             function with regulator in LowPower or with Regulator ON.
    (+) Exit:
        (++) Any EXTI Line (Internal or External) configured in Interrupt/Event mode
             or any internal IPs (I2C, UASRT or CEC) wakeup event.

  *** Standby mode ***
  ====================
  [..] The Standby mode allows to achieve the lowest power consumption. It is based
       on the Cortex-M0 deepsleep mode, with the voltage regulator disabled.
       The VCORE domain is consequently powered off. the HSI are also switched off.
       SRAM and register  contents are lost except for the Backup domain (RTC registers,
       RTC backup registers and Standby circuitry).

  [..] The voltage regulator is OFF.

    (+) Entry:
        (++) The Standby mode is entered using the PWR_EnterSTANDBYMode() function.
    (+) Exit:
        (++) WKUP pin rising edge, STBAWUT  event, time-stamp event, external reset in NRST pin,
        IWDG reset.

  *** Auto-wakeup (AWUT) from low-power mode ***
  =============================================
  [..] The MCU can be woken up from low-power mode by an STBAWUT event, a tamper
       event, a time-stamp event, or a comparator event, without depending on an
       external interrupt (Auto-wakeup mode).
    (+) STBAWUT from the Standby mode
        (++) To wake up from the Standby mode with an STBAWUT event, it is necessary to:
             (+++) Enable the STBAWUT Interrupt using the PWR_StandbyAutoWakeupTimer_Cmd function.
             (+++) Configure the RTC to generate the RTC alarm using the PWR_SetStandbyWakeupTimer
                   functions.
        (++) To wake up from the Standby mode with an RTC Tamper or time stamp event, it
             is necessary to:
             (+++) Enable the RTC Tamper or time stamp Interrupt using the RTC_ITConfig()
                   function.
             (+++) Configure the RTC to detect the tamper or time stamp event using the
                   RTC_TimeStampConfig(), RTC_TamperTriggerConfig() and RTC_TamperCmd()
                   functions.

    (+) Comparator auto-wakeup (AWUT) from the Stop mode
        (++) To wake up from the Stop mode with an comparator 1 or comparator 2 wakeup
             event, it is necessary to:
             (+++) Configure the EXTI Line 21 for comparator 1 or EXTI Line 22 for comparator 2
                   to be sensitive to to the selected edges (falling, rising or falling
                   and rising) (Interrupt or Event modes) using the EXTI_Init() function.
             (+++) Configure the comparator to generate the event.

@endverbatim
  * @{
  */

/**
  * @brief  Enters Sleep mode.
  * @note   In Sleep mode, all I/O pins keep the same state as in Run mode.
  * @param  PWR_Entry: specifies if SLEEP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *             @arg PWR_Entry_WFI: enter SLEEP mode with WFI instruction
  *             @arg PWR_Entry_WFE: enter SLEEP mode with WFE instruction
  * @retval None
  */

void PWR_EnterSleepMode(uint8_t PWR_Entry)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_ENTRY(PWR_Entry));

  /* Select the regulator state in Sleep mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  /* Store the new value */
  PWR->CR = tmpreg;

  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_Entry == PWR_Entry_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }
}


/**
  * @brief  Enters STOP mode.
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event,
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional
  *         startup delay is incurred when waking up from Stop mode.
  *         By keeping the internal regulator ON during Stop mode, the consumption
  *         is higher although the startup time is reduced.
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *         This parameter can be one of the following values:
  *             @arg PWR_Regulator_ON: STOP mode with regulator ON
  *             @arg PWR_Regulator_LowPower: STOP mode with regulator in low power mode
  * @param  PWR_Entry: specifies if STOP mode in entered with WFI or WFE instruction.
  *         This parameter can be one of the following values:
  *             @arg PWR_Entry_WFI: enter STOP mode with WFI instruction
  *             @arg PWR_Entry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
void PWR_EnterStopMode(uint32_t PWR_Regulator, uint8_t PWR_Entry)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));
  assert_param(IS_PWR_ENTRY(PWR_Entry));

  // enable rcc of pwr
  RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_PWR, ENABLE);

  /* Select the regulator state in Stop mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  /* Set LPDS bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  /* Store the new value */
  PWR->CR = tmpreg;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;

  /* Select Stop mode entry --------------------------------------------------*/
  if(PWR_Entry == PWR_Entry_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI();


  }
  else
  {
    // wait the AWUT is IDE and AWUT_BUSY is 0
    while(AWUT->SR & 0x00000001) {};

    //  detect and clear the AWUT_EXTILINE11
    if(EXTI_GetFlagStatus(EXTI_Line11) == SET)
    {
      EXTI_ClearFlag(EXTI_Line11);
    }

    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }

  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
}

/**
  * @brief  Enters STANDBY mode.
  * @note   In Standby mode, all I/O pins are high impedance except for:
  *          - Reset pad (still available)
  *          -  tamper, time-stamp,
  *          - WKUP pin 1 if enabled.
  *          - PC6£¬PC3£¬PD6 if configured for IO_KEEP PIN
  * @note The Wakeup flag (WUF) need to be cleared at application level before to call this function
  * @retval None
  */
void PWR_EnterStandbyMode(void)
{
  // enable rcc of pwr
  RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_PWR, ENABLE);

  /* Clear Wake-up flag */
  PWR->CR |= PWR_CR_CWUF;
  /* Select Standby mode */
  PWR->CR |= PWR_CR_PDDS;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;
  /* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @}
  */

/** @defgroup PWR_Group3 Flags management functions
 *  @brief   Flags management functions
 *
@verbatim
  ==============================================================================
                       ##### Flags management functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  *     @arg PWR_FLAG_LDORDY: Internal CPU LDO ready flag
  * @retval The new state of PWR_FLAG (SET or RESET).
  */



FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));

  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
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
  * @brief  Clears the PWR's pending flags.
  * @param  PWR_FLAG: specifies the flag to clear.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  * @retval None
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  /* Check the parameters */
  assert_param(IS_PWR_CLEAR_FLAG(PWR_FLAG));

  PWR->CR |=  PWR_FLAG << 2;
}

/**
  * @brief  Enables or disables the WakeUp Pin functionality.
  * @param  NewState: new state of the WakeUp Pin functionality.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState == ENABLE)
  {
    PWR->CSR |= PWR_EWUP1_MSK;
  }
  else
  {
    PWR->CSR &= (uint32_t)~PWR_EWUP1_MSK;
  }
}

/**
  * @brief  Config wakeup pin polarity.
  * @param  eWakePolarity: specifies wakeup pin polarity.
  *   This parameter can be one of the following values:
  *     @arg RISING or FALLING
  * @retval None.
  */
void PWR_WakeUpPinPolarity(WakePolarity eWakePolarity)
{
  /* Check the parameters */
  if(eWakePolarity == RISING)
  {
    PWR->WUP_POL &=  ~PWR_WUPOL_MSK;
  }
  else
  {
    PWR->WUP_POL |=  PWR_WUPOL_MSK;
  }

}

/**
  * @brief  Enables or disables PDR(power down detect) when enter standby mode .
  * By setting this register, the power-off detection PDR will be turned off in Standby mode to further reduce standby power consumption (about 50nA)
  * @param  NewState: This parameter can be: ENABLE or DISABLE
          ENABLE: power off PDR when enter standby mode.
          DISABLE: power on PDR when enter standby mode .
  * @retval None
  */

void PWR_PowerDownDetect_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  // unlock PDR_PD Config
  PWR->PORPDR_CFG = 0x0000;
  PWR->PORPDR_CFG = 0x5500;
  PWR->PORPDR_CFG = 0xAA00;
  PWR->PORPDR_CFG = 0x5A00;
  PWR->PORPDR_CFG = 0xA500;
  PWR->PORPDR_CFG = 0xC800;
  PWR->PORPDR_CFG = 0x8C00;
  PWR->PORPDR_CFG = 0x6900;


  if(NewState == ENABLE)
  {
    // power off  PDR
    PWR->PORPDR_CFG = 0x9601;
  }
  else
  {
    // power on PDR
    PWR->PORPDR_CFG = 0x9600;
  }
}


/**
  * @brief  Enables or disables StandbyAutoWakeupTimer .
  * @param  NewState: Enables or disables StandbyAutoWakeupTimer .
  * @retval None
  */
void PWR_StandbyAutoWakeupTimer_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState == ENABLE)
  {
    PWR->CSR |= PWR_CSR_SAWU;
  }
  else
  {
    PWR->CSR &= ~PWR_CSR_SAWU;
  }

}


/**
  * @brief  Set stanby wakeup timer(STBAWU)
  * @param  Timercount: timer count
  * @retval None
  */
void PWR_SetStandbyWakeupTimer(uint32_t Timercount)
{
  PWR->STBAWU |= Timercount;
}

/**
  * @brief  Set data to backup 0 register in standy mode
  * @param  BackupData0: data
  *  16bit side
  *  half word/word to wrrite/read
  *  data will be saved in Standby
  * @retval None
  */
void PWR_StandbyMode_BKP0(uint32_t BackupData0)
{

  PWR->SBKP0 = BackupData0;


}

/**
  * @brief  Set data to backup 1 register in standy mode
  * @param  BackupData1: data
  *  16bit side
  *  half word/word to wrrite/read
  *  data will be saved in Standby
  * @retval None
  */
void PWR_StandbyMode_BKP1(uint32_t BackupData1)
{
  PWR->SBKP1 = BackupData1;
}

void PWR_Standby_IO_KEEP(uint16_t IOLUPeriph, FunctionalState NewState)
{

  uint16_t temp = PWR->IO_KEEP ;

  if (NewState != DISABLE)
  {
    temp |= (IOLUPeriph);
  }
  else
  {
    temp |= IOLUPeriph;
    temp = (temp ^ IOLUPeriph);
  }

  temp = temp + 0x9600;
  PWR->IO_KEEP = temp;
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

/**
 * @}
 */

