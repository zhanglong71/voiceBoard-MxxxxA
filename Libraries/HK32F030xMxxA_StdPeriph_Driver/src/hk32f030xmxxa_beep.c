/**
  ******************************************************************************
  * @file    hk32f030xmxxa_beep.c
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
  ******************************************************************************
  */

#include "hk32f030xmxxa_beep.h"
#include "hk32f030xmxxa.h"


/** @addtogroup HK32F030xMxxA_StdPeriph_Driver
  * @{
  */
/** @defgroup BEEP BEEP
  * @brief BEEP driver modules
  * @{
  */

/**
  * @brief  Initialize the beeper peripheral register to the default
  *          value reset value
  * @retval None
  */

void BEEP_DeInit(void)
{
  /*BEEP clock:LSI,BEEP_Prescaler:64,BEEP_TRGO_Prescaler:64*/
  BEEP->CFGR = BEEP_CFGR_Value;

  /*BEEP:ENABLE,TRGO:ENABLE*/
  while(BEEP->CR & BEEP_BUSY_FLAG);

  BEEP->CR |= BEEP_CR_Value;
}

/**
  * @brief  Initialize the BEEP peripheral register
  * @param  BEEP_InitStruct: pointer to a BEEP_InitTypeDef structure which will be initialized.
  * @retval None
  */
void BEEP_Init(BEEP_InitTypeDef *BEEP_InitStruct)
{
  assert_param(IS_BEEP_CLOCK(BEEP_InitStruct->BEEP_Clock));
  assert_param(IS_BEEP_TRGO_PRESCALER(BEEP_InitStruct->BEEP_TRGOPrescaler));
  assert_param(IS_BEEP_PRESCALER(BEEP_InitStruct->BEEP_Prescaler));
  assert_param(IS_FUNCTIONAL_STATE(BEEP_InitStruct->BEEP_TRGOCmd));

  uint32_t temp = 0;

  /*Set the BEEP clock*/
  if(BEEP_InitStruct->BEEP_Clock == BEEP_CLOCK_HSE)
  {
    BEEP->CFGR |= BEEP_CLOCK_HSE;
  }
  else
  {
    BEEP->CFGR &= ~(BEEP_CLOCK_HSE);
  }


  /*Clear [2:1]bits*/
  temp = BEEP->CFGR;

  temp &= BEEP_CR_BEEP_Mask;
  /*Set the frequency division coefficient*/
  temp |= BEEP_InitStruct->BEEP_Prescaler;
  /*To transfer a value into a register*/
  BEEP->CFGR = temp;

  /*Clear [4:3]bits*/
  temp &= BEEP_CR_TRGO_Mask;
  /*Set the frequency division coefficient*/
  temp |= BEEP_InitStruct->BEEP_TRGOPrescaler;

  /*To transfer a value into a register*/
  BEEP->CFGR = temp;

  /*Enable or disable TRGO*/
  if(BEEP_InitStruct->BEEP_TRGOCmd)
  {
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR |= BEEP_CR_TRGOEN;
  }
  else
  {
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR &= (uint32_t)~((uint32_t)BEEP_CR_TRGOEN);
  }
}

/**
  * @brief  Enables or disables the specified BEEP peripheral.
  * @param  NewState: new state of the BEEP peripheral.
  * @retval None
  */
void BEEP_Cmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState != DISABLE)
  {
    /*Enable BEEP*/
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR |= BEEP_CR_BEEPEN;
  }
  else
  {
    /*Disable BEEP*/
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR &= (uint32_t)~((uint32_t)BEEP_CR_BEEPEN);
  }
}


/**
  * @brief  Select the BEEP clock.
  * @param  BEEP_CLOCK: Clock source selection.
  *          This parameter can be one of the following values:
  *            @arg BEEP_CLOCK_HSE
  *            @arg BEEP_CLOCK_LSI
  * @retval None
  */
void BEEP_ClockSelect(uint8_t BEEP_CLOCK)
{
  assert_param(IS_BEEP_CLOCK(BEEP_CLOCK));

  if(BEEP_CLOCK_HSE == BEEP_CLOCK)
  {
    /*Set HSE as the clock source*/
    BEEP->CFGR |= BEEP_CLOCK_HSE;
  }
  else
  {
    /*Set LSI as the clock source*/
    BEEP->CFGR &= ~(BEEP_CLOCK_HSE);
  }
}


/**
  * @brief  Set the BEEP frequency division coefficient.
  * @param  BEEP_Prescaler: frequency division coefficient selection.
  *          This parameter can be one of the following values:
  *            @arg BEEP_Prescaler_16
  *            @arg BEEP_Prescaler_32
  *             @arg BEEP_Prescaler_64
  *             @arg BEEP_Prescaler_128
  * @retval None
  */
void BEEP_SetPrescaler(uint8_t BEEP_Prescaler)
{
  uint32_t temp = 0;
  assert_param(IS_BEEP_PRESCALER(BEEP_Prescaler));

  /*Clear [2:1]bits*/
  temp = BEEP->CFGR;
  temp &= BEEP_CR_BEEP_Mask;
  /*Set the frequency division coefficient*/
  temp |= BEEP_Prescaler;
  /*To transfer a value into a register*/
  BEEP->CFGR = temp;
}



/**
  * @brief  Set the TGRO frequency division coefficient.
  * @param  BEEP_TRGO_Prescaler: frequency division coefficient selection.
  *          This parameter can be one of the following values:
  *            @arg BEEP_TRGO_Prescaler_32
  *            @arg BEEP_TRGO_Prescaler_64
  *             @arg BEEP_TRGO_Prescaler_128
  * @retval None
  */
void BEEP_SetTRGOPrescaler(uint8_t BEEP_TRGO_Prescaler)
{
  uint32_t temp = 0;
  assert_param(IS_BEEP_TRGO_PRESCALER(BEEP_TRGO_Prescaler));

  /*Clear [4:3]bits*/
  temp = BEEP->CFGR;
  temp &= BEEP_CR_TRGO_Mask;
  /*Set the frequency division coefficient*/
  temp |= BEEP_TRGO_Prescaler;
  /*To transfer a value into a register*/
  BEEP->CFGR = temp;
}


/**
  * @brief  Read BEEP register status.
  * @retval register status
  */
FlagStatus BEEP_ReadBeepStatus(void)
{
  if((BEEP->CR & (BEEP_BUSY_FLAG)) == BEEP_BUSY_FLAG)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}


/**
  * @brief  Enables or disables the TRGO function.
  * @param  NewState: new state of the BEEP peripheral.
  * @retval None
  */
void BEEP_TRGOCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState)
  {
    /*Enable TRGO*/
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR |= (uint32_t)BEEP_CR_TRGOEN;
  }
  else
  {
    /*Disable TRGO*/
    while(BEEP->CR & BEEP_BUSY_FLAG);

    BEEP->CR &= (uint32_t)~((uint32_t)BEEP_CR_TRGOEN);
  }
}


/**
  * @}
  */

/**
  * @}
  */
