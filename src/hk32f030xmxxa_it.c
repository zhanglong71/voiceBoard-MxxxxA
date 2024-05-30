/**
  ******************************************************************************
  * @file    hk32f030xmxxa_it.c 
  * @author  Alexander
  * @version V1.0.0
  * @date    04-January-2023
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 HKMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hk32f030xmxxa_it.h"
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"

#include "l_arch.h"
#include "l_timer.h"
#include "l_u8FIFO.h"
/** @addtogroup HK32F030xMxxA_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
    TimingDelay_Increment();
}

/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    u8Data_t u8data;
	if( USART_GetITStatus(USART1, USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        g_IT_uart1_tmr = 0;

        u8data.u8Val = USART_ReceiveData(USART1);
        u8FIFOin(&g_uart1RxQue, &u8data);

        msg_t msg;
        msg.msgType = CMSG_UART1RX;
        msgq_in(&g_msgq, &msg);
	}
    
	/* USART1 transfer empty interrupt handle */
	if( USART_GetITStatus(USART1, USART_IT_TXE) != RESET )
	{
		USART_ClearITPendingBit(USART1, USART_IT_TXE);

        if(u8FIFOout(g_rs485transX.pfifo, &u8data) == TRUE) {
           USART_SendData(USART1, u8data.u8Val);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART1, USART_IT_TC, ENABLE);
        }
    }
    
    /* USART1 transfer complete interrupt handle */
    if( USART_GetITStatus(USART1, USART_IT_TC) != RESET )
    {
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        
        M485TR_R("....");
	}
}

void UART2_IRQHandler(void)
{
    u8Data_t u8data;
	if( USART_GetITStatus(UART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART2, USART_IT_RXNE);
		
        /* receive data */
        u8data.u8Val = USART_ReceiveData(UART2);
        u8FIFOin(&g_uart2RxQue, &u8data);

        msg_t msg;
        msg.msgType = CMSG_UART2RX;
        msgq_in(&g_msgq, &msg);
    }
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
 uint16_t capture = 0;
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

        /*vop control frequency = 4k Hz */
        vop_sendByPulse();
        // GPIO_led_blink();  // ????????????????????????????????????????
        capture = TIM_GetCapture1(TIM2);
        TIM_SetCompare1(TIM2, capture + CCR1_Val);
        #if 0
    } else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

        capture = TIM_GetCapture2(TIM2);
        TIM_SetCompare2(TIM2, capture + CCR2_Val);
    } else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

        capture = TIM_GetCapture3(TIM2);
        TIM_SetCompare3(TIM2, capture + CCR3_Val);
    } else {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

        capture = TIM_GetCapture4(TIM2);
        TIM_SetCompare4(TIM2, capture + CCR4_Val);
        #endif
    }
}

/******************************************************************************/
/*             HK32F030xMxxA Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (KEIL_startup_hk32f030xmxxa.s).                                      */
/******************************************************************************/
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */
