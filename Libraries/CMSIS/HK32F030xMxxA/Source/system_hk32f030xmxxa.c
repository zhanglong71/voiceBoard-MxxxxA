/**
  ******************************************************************************
  * @file    system_hk32f030xmxxa.c
  * @author  AE Team
  * @version V1.0.0/2022-04-14
  *          1.初始版本
  * @log  V1.0.0
  ******************************************************************************
*/ 

 /*
 This file configures the system clock as follows:
  *=============================================================================
  *                         Supported hk32f030xmxxa device
  *-----------------------------------------------------------------------------
  *        System Clock source                    | HSI32M / HSI48M
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 32000000 / 48000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 32000000  / 48000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *=============================================================================
  ******************************************************************************

  */
#include "hk32f030xmxxa.h"
#include "hk32f030xmxxa_conf.h"

#ifndef SYSCLK_SOURCE
  #error "Please define the value of SYSCLK_SOURCE in hk32f030xmxxa_conf.h file!"
#endif


/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
#if(SYSCLK_SOURCE == SYSCLK_SRC_HSI)
  #define SYSCLK_FREQ_HSI        HSI_VALUE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_HSI;                 /*!< System Clock Frequency (Core Clock) */
    static void SetSysClockToHSI(void);     
#elif(SYSCLK_SOURCE == SYSCLK_SRC_LSI)
  #define SYSCLK_FREQ_LSI           LSI_VALUE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_LSI;               /*!< System Clock Frequency (Core Clock) */
  static void SetSysClockToLSI(void); 
#elif(SYSCLK_SOURCE == SYSCLK_SCR_EXTCLK_IO)
  #define SYSCLK_FREQ_EXTCLK          EXTCLK_VALUE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_EXTCLK;                /*!< System Clock Frequency (Core Clock) */
  static void SetSysClockToEXTCLK(void);  
#endif

static void SetSysClock(void);

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the default HSI clock source, vector table location and the PLL configuration is reset.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0] and MCOSEL[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB81C;

  /* Reset USARTSW[1:0], I2CSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFFEC;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  SetSysClock();

#ifdef VECT_TAB_SRAM
  SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE; /* Vector Table Relocation in Internal SRAM. */
#else
    FLASH->INT_VEC_OFFSET = VECT_TAB_OFFSET ; /* Vector Table Relocation in Internal FLASH. */
#endif   
}

/**
  * @brief  Configures the System clock frequency, HCLK, PCLK prescalers.
  * @param  None
  * @retval None
  */
static void SetSysClock(void)
{
  /*reload the  hsi trimming value to the bit3~bit13  of RCC_CR register */
   
  uint32_t u32RCC_CR = 0;
  uint32_t u32HSITemp = 0;  
  uint16_t u16PMUTemp = 0; 
  
  uint32_t u32HSIFLASH = *(uint32_t *) 0x1FFFF820;
  uint16_t u16HSITempH = (uint16_t)(u32HSIFLASH>>16);
  uint16_t u16HSITempL = (uint16_t)(u32HSIFLASH);
    
   uint32_t u32PMUFLASH = *(uint32_t *) 0x1FFFF82C;
  uint16_t u16PMUTempH = (uint16_t)(u32PMUFLASH>>16);
  uint16_t u16PMUTempL = (uint16_t)(u32PMUFLASH);
    
  if(!(u16HSITempH & u16HSITempL))
  {
    u32HSITemp = RCC->CR;
    u32HSITemp &= (uint32_t)((uint32_t)~(RCC_CR_HSITRIM|RCC_CR_HSICAL));
    u32RCC_CR = (uint32_t)(((u16HSITempL & 0x001F) <<3) | (((u16HSITempL>>5) & 0x003F)<<8)); 
    RCC->CR |= u32RCC_CR;      
  } 
  
  if(!(u16PMUTempH & u16PMUTempL))
  { 
    RCC->APBENR1 |= RCC_APBENR1_PWREN;  
    u16PMUTemp = (uint16_t)(u16PMUTempL); 

    *(uint32_t *)0x4000704c = 0x00001985;
    *(uint32_t *)0x4000704c = 0x00000429; 
    *(uint32_t *) 0x40007068 = u16PMUTemp;    
    *(uint32_t *)0x4000704c = 0x0000FFFF;
    *(uint32_t *)0x4000704c = 0x0000FFFF;      
  } 
  
  /*end*/
#if(SYSCLK_SOURCE == SYSCLK_SRC_HSI)
  SetSysClockToHSI();     
#elif(SYSCLK_SOURCE == SYSCLK_SRC_LSI)
  SetSysClockToLSI(); 
#elif(SYSCLK_SOURCE == SYSCLK_SCR_EXTCLK_IO)
  SetSysClockToEXTCLK();  
#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock  source (default after reset) */ 
}

#if(SYSCLK_SOURCE == SYSCLK_SRC_HSI)
static void SetSysClockToHSI(void)
{
  __IO uint32_t StartUpCounter = 0, HSIStatus = 0;
  __IO uint32_t ACRreg = 0;  
  __IO uint32_t RCCHCLKReg = 0;  
  __IO uint32_t RCCPCLKReg = 0;
  /* Enable HSI */
    RCC->CR |= RCC_CR_HSION;
    
  RCC->CFGR4 &= ~RCC_CFGR4_FLITFCLK_PRE;
  RCC->CFGR4 |= (((uint32_t)0x0F) << RCC_CFGR4_FLITFCLK_PRE_Pos);

  /* Wait till HSI is ready and if Time out is reached exit */
  do{
    HSIStatus = RCC->CR & RCC_CR_HSIRDY;
    StartUpCounter++;  
  } while((HSIStatus == 0) && (StartUpCounter != STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSIRDY) != RESET)
  {
    HSIStatus = (uint32_t)0x01;
  }
  else
  {
    HSIStatus = (uint32_t)0x00;
  }  
  
  if (HSIStatus == (uint32_t)0x01)
  {
    /* Flash wait state */
    ACRreg = FLASH->ACR;
    ACRreg &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR = (uint32_t)(FLASH_Latency_2|ACRreg);  


    RCCHCLKReg = RCC->CFGR;
    RCCHCLKReg &= (uint32_t)((uint32_t)~RCC_CFGR_HPRE_Mask);
    /* HCLK = SYSCLK */
    RCC->CFGR = (uint32_t)(RCC_SYSCLK_Div1|RCCHCLKReg);

    RCCPCLKReg = RCC->CFGR;
    RCCPCLKReg &= (uint32_t)((uint32_t)~RCC_CFGR_PPRE_Mask);
    /* PCLK = HCLK */
    RCC->CFGR = (uint32_t)(RCC_CFGR_PPRE_DIV1|RCCPCLKReg);

    /* Select HSI as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;    

    /* Wait till HSI is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    {
    }
  }
  else
  { /* If fails to start-up, the application will have wrong clock configuration. User can add here some code to deal with this error */
  }    
};     

#elif(SYSCLK_SOURCE == SYSCLK_SRC_LSI)
static void SetSysClockToLSI(void)
{
  __IO uint32_t StartUpCounter = 0, LSIStatus = 0;
  
  /* Enable LSI */
    RCC->CSR |= RCC_CSR_LSION;

  /* Wait till LSI is ready and if Time out is reached exit */
  do{
    LSIStatus = RCC->CSR & RCC_CSR_LSIRDY;
    StartUpCounter++;  
  } while((LSIStatus == 0) && (StartUpCounter != STARTUP_TIMEOUT));

  if ((RCC->CSR & RCC_CSR_LSIRDY) != RESET)
  {
    LSIStatus = (uint32_t)0x01;
  }
  else
  {
    LSIStatus = (uint32_t)0x00;
  }  
  
  if (LSIStatus == (uint32_t)0x01)
  {

    /* Flash wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_Latency_0;  

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV0;

    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* Select HSI as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_LSI;    

    /* Wait till LSI is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_LSI)
    {
    }

  }
  else
  { /* If fails to start-up, the application will have wrong clock configuration. User can add here some code to deal with this error */
  }    
};   
#elif(SYSCLK_SOURCE == SYSCLK_SCR_EXTCLK_IO)
static void SetSysClockToEXTCLK(void)
{
  __IO uint32_t StartUpCounter = 0, EXTCLKStatus = 0;
  __IO uint32_t ACRreg = 0;
  __IO uint32_t RCCHCLKReg = 0;  
  __IO uint32_t RCCPCLKReg = 0;
  //enable EXTIO PA1/PD7/PB5/PC5

  /* Configure PA1  as CLOCK input */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  // GPIO_InitTypeDef GPIO_InitStructure;
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  // GPIO_Init(GPIOD, &GPIO_InitStructure);


  // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  // GPIO_InitTypeDef GPIO_InitStructure;
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  // GPIO_Init(GPIOB, &GPIO_InitStructure);

  // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  // GPIO_InitTypeDef GPIO_InitStructure;
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  // GPIO_Init(GPIOC, &GPIO_InitStructure);

  /*CLOCK select */
    RCC->CFGR4 &= (uint32_t)~(RCC_CFGR4_EXTCLK_SEL);
    RCC->CFGR4 |= (uint32_t)RCC_CFGR4_EXTCLK_SEL_PA1;  
  // RCC->CFGR4 |= (uint32_t)RCC_CFGR4_EXTCLK_SEL_PB5;  
  // RCC->CFGR4 |= (uint32_t)RCC_CFGR4_EXTCLK_SEL_PC5;  
  // RCC->CFGR4 |= (uint32_t)RCC_CFGR4_EXTCLK_SEL_PD7;  
  /* Enable EXTCLK */
    RCC->CR |= RCC_CR_EXTCLKON;

  /* Wait till LSI is ready and if Time out is reached exit */
  do{
    EXTCLKStatus = RCC->CR & RCC_CR_EXTCLKRDY;
    StartUpCounter++;  
  } while((EXTCLKStatus == 0) && (StartUpCounter != STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_EXTCLKRDY) != RESET)
  {
    EXTCLKStatus = (uint32_t)0x01;
  }
  else
  {
    EXTCLKStatus = (uint32_t)0x00;
  }  
  
  if (EXTCLKStatus == (uint32_t)0x01)
  {

    /* Flash wait state */
    ACRreg=  FLASH->ACR;
    ACRreg &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    
    if (SystemCoreClock <= 16000000)
      FLASH->ACR = (uint32_t)(FLASH_Latency_0|ACRreg);
    else if(SystemCoreClock <= 32000000)
      FLASH->ACR = (uint32_t)(FLASH_Latency_1|ACRreg);
    else
      FLASH->ACR = (uint32_t)(FLASH_Latency_2|ACRreg);


    RCCHCLKReg = RCC->CFGR;
    RCCHCLKReg &= (uint32_t)((uint32_t)~RCC_CFGR_HPRE_Mask);
    /* HCLK = SYSCLK */
    RCC->CFGR = (uint32_t)(RCC_CFGR_HPRE_DIV0|RCCHCLKReg);

    RCCPCLKReg = RCC->CFGR;
    RCCPCLKReg &= (uint32_t)((uint32_t)~RCC_CFGR_PPRE_Mask);
    /* PCLK = HCLK */
    RCC->CFGR = (uint32_t)(RCC_CFGR_PPRE_DIV1|RCCPCLKReg);


    /* Select EXTCLK as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_EXTCLK;    

    /* Wait till EXTCLK is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_EXTCLK)
    {
    }
  }
  else
  { /* If fails to start-up, the application will have wrong clock configuration. User can add here some code to deal with this error */
  }    
};   

#endif


/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0,presc = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case RCC_CFGR_SWS_HSI:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
  case RCC_CFGR_SWS_EXTCLK:  /* EXTCLK used as system clock */
    SystemCoreClock = EXTCLK_VALUE;
    break;
  case RCC_CFGR_SWS_LSI:  /* LSI used as system clock */
    SystemCoreClock = LSI_VALUE;
    break;

    default: /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
  }
  
  /* Compute HCLK clock frequency ----------------*/
  /* Get HCLK prescaler */
  tmp = RCC->CFGR & RCC_CFGR_HPRE;
  tmp = tmp >> 4;
  presc = AHBPrescTable[tmp]; 
  /* HCLK clock frequency */
 SystemCoreClock = SystemCoreClock/presc;
}
