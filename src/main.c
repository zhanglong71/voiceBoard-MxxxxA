/**
  ******************************************************************************
  * @file    main.c
  * @author  DIISEA
  * @version V1.0.0
  * @date    23-May-2024
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 DIISEA.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "const.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"

#include "l_arch.h"
#include "l_uart.h"
#include "l_sysProcess.h"
#include "f_idle.h"
#include "f_init.h"
#include "main.h"

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
/**********************************************************************************************/
 fstack_t g_fstack;
 msgq_t g_msgq;
 msg_t msg;
 func_t func;
 u16 g_tick;
 u16 g_flag;
 /**
  * g_flag.1 for key double click
  * 
  **/
 Timer_t g_timer[TIMER_NUM];
 /**
  * g_timer[0] for global, 
  * g_timer[1] for uart2 receive
  * g_timer[2] for g_promptQueue
  * g_timer[3] for key/getNetInfo
  **/
u8FIFO_t g_uart1TxQue;
u8FIFO_t g_uart1RxQue;
u8FIFO_t g_uart2TxQue;
u8FIFO_t g_uart2RxQue;

rs485transX_t g_rs485transX;

actionQueue_t g_promptQueue;
u8 g_IT_uart1_tmr = 0; // used uart1 received timeover
/******************************************************************************/
NetInfo_t g_netInfo;

/**
 * record wifi net information
 **/
/******************************************************************************/
ComponentField_t g_componentStatus = {
//    .mop                  = CINDEX_STANDBY,
    .roller               = CINDEX_ROLLERNORMAL,
    .pump                 = CINDEX_PUMPNORMAL,
    .battery              = CINDEX_BATTERYNORMAL,
    .charge               = CINDEX_UNCHARGED,
    .cleanWater           = CINDEX_CLEANWATERNORMAL,
    .status               = CINDEX_STANDBY,
    .voicePrompt          = CINDEX_VOICEPROMPT_ON,
    .commonFaultDetection = CINDEX_NODEFAULT,
    .netConnection        = CINDEX_NETCONNECTION_OFF,
};
/******************************************************************************/
char g_buf[U8FIFOSIZE];    // Temporary variables for uart received data resolve
kv_t g_KVarr[CKVTABSIZE];  // Temporary variables for uart received data resolve
/******************************************************************************/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    fstack_init(&g_fstack);
    func.func = f_init;
    fstack_push(&g_fstack, &func);

    msgq_init(&g_msgq);
    msg.msgType = CMSG_INIT;
    msgq_in_irq(&g_msgq, &msg);
    
    /* Infinite Loop */
    while(1)
    {
        IWDG_ReloadCounter();  /* Reload IWDG counter */
        deamon_task();
        if(msgq_out_irq(&g_msgq, &msg) == FALSE) {
            continue;
        }
        if(sysProcess(&msg) == TRUE) {
            continue;
        }

        if(fstack_top(&g_fstack, &func) == FALSE) {
            /** something wrong happend, Power Down or recover it **/
            fstack_init(&g_fstack);
            func.func = f_idle;
            fstack_push(&g_fstack, &func);
	
            g_tick = 0;
            SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
            continue;
        }
        func.func((unsigned *)&msg);
    }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
