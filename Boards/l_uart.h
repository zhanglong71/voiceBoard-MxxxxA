#ifndef __L_UART_H_
#define __L_UART_H_

void RCC_Configuration4uart(void);
void GPIO_Configuration4uart(void);
void NVIC_Configuration4uart(void);

void USART_Config(void);
void GPIO_init485(void);
// void deamon_485uart1_send(void);
void deamon_uart2_send(void);
void GPIO_wifiEnable(void);

#endif
