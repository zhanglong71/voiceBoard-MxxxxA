
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"
#include "l_u8FIFO.h"
#include "l_uart.h"

#include "main.h"

void RCC_Configuration4uart(void)
{
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD, ENABLE);

  /* Enable UART1 clock */
  RCC_APBPeriph2ClockCmd(RCC_APBPeriph2_USART1, ENABLE);

  /* Enable UART2 clock */
  RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_UART2, ENABLE);
}

void GPIO_Configuration4uart(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*
    UART1_TX:PA3  AF1
    UART1_RX:PD6  AF1

    UART2_TX:PC5   AF5
    UART2_RX:PC6   AF5
  */

  /* UART1 Pins configuration ************************************************/
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  // GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_1);

  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  /* RX Pull-Up can filter noise*/
  // GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* UART2 Pins configuration ************************************************/
  #if 1
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_5);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_5);

  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   /* RX Pull-Up can filter noise*/
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  #endif
}

void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;

  /* USARTx configured as follow: */
  /* 
  - USART1 configuration 
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  // HK_EVAL_COMInit(COM1, &USART_InitStructure);
    /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);
 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
  /* 
  - USART2 configuration 
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
    
  /* USART2 configuration */
  USART_Init(UART2, &USART_InitStructure);
 
  USART_ITConfig(UART2, USART_IT_RXNE, ENABLE);   
  /* Enable USART */
  USART_Cmd(UART2, ENABLE);
  // GPIO_Configuration4uart();
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration4uart(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* USART1 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* UART2 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#if 0

  /* UART3 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = UART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* UART4 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
  * @brief Configure the USART Device
  * @param  None
  * @retval None
  */

void GPIO_init485(void)
{
    #if 1
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* Configure PD02() in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  M485TR_R("initialized as input state");
    #endif
}

#if 0
void deamon_485uart1_send(void)
{
    msg_t msg;
    u8Data_t u8Data;
    if(u8FIFOisEmpty(&g_uart1TxQue) == TRUE) {
        if (USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET) {
            M485TR_R("发送完成，进入接收状态");
        }
        return;
    }
    if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
        return;
    }
    
    if(u8FIFOout_irq(&g_uart1TxQue, &u8Data) == TRUE) {
        M485TR_T("发送前置高");
        USART_SendData(USART1, u8Data.u8Val);
        /** 如果队列已空，数据发送完成 **/
        if(u8FIFOisEmpty(&g_uart1TxQue) == TRUE) {
            msg.msgType = CMSG_UART1TX;
            msgq_in_irq(&g_msgq, &msg); 
        }
    }
}
#endif

void deamon_uart2_send(void)
{
    msg_t msg;
    u8Data_t u8Data;
    if(u8FIFOisEmpty(&g_uart2TxQue) == TRUE) {
        if (USART_GetFlagStatus(UART2, USART_FLAG_TC) == SET) {
            // M485TR_R("发送完成，进入接收状态");
        }
        return;
    }
    if(USART_GetFlagStatus(UART2, USART_FLAG_TXE) == RESET) {
        return;
    }
    
    if(u8FIFOout_irq(&g_uart2TxQue, &u8Data) == TRUE) {
        // M485TR_T("发送前置高");
        USART_SendData(UART2, u8Data.u8Val);
        /** 如果队列已空，数据发送完成 **/
        if(u8FIFOisEmpty(&g_uart2TxQue) == TRUE) {
            msg.msgType = CMSG_UART2TX;
            msgq_in_irq(&g_msgq, &msg); 
        }
    }
}
