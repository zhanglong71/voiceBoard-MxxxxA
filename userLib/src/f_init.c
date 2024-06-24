
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"
#include "l_gpio.h"
#include "l_uart.h"
#include "l_timer.h"
#include "l_u8FIFO.h"
#include "l_rs485.h"
#include "l_actionFIFO.h"
#include "l_sysProcess.h"

#include "f_init.h"
#include "f_idle.h"
#include "main.h"

/*********************************************************************************************/
int f_init(void *pMsg)
{
    func_t func;
    msg_t msg;
    
    switch(((msg_t *)pMsg)->msgType) 
    {
    case CMSG_TMR:
        g_tick++;
        break;
 
    case CMSG_INIT:
        /*** step1 Periph_Driver ******************************************************************/
        /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (KEIL_startup_hk32f030xmxxa.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_hk32f030xmxxa.c file
        */
        RCC_Configuration4uart(); 
        GPIO_Configuration4uart();
        NVIC_Configuration4uart();
        /* USART configuration */
        GPIO_init485();
        USART_Config();
        TIM_Config();
        SysTick_Config(SystemCoreClock / 100);  /** 10ms **/
        GPIO_initVOPPort();
        GPIO_keyScanInit();
        GPIO_init4led();
        GPIO_wifiModuleInit();
        watchDog_init();
        /*** step2 varitual about Periph_Driver ***************************************************/
        vp_init();
        promptInit();
        rs485Init();
        netInfoData_init();
        
        u8FIFOinit(&g_uart1TxQue);
        u8FIFOinit(&g_uart1RxQue);
        u8FIFOinit(&g_uart2TxQue);
        u8FIFOinit(&g_uart2RxQue);
        /*** step3 ********************************************************************************/
	    for(int i = 0; i < MTABSIZE(g_timer); i++) {
            ClrTimer(&g_timer[i]);
        }
        g_tick = 0;
        SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
        
        fstack_init(&g_fstack);
        func.func = f_idle;
        fstack_push(&g_fstack, &func);
        
        msgq_init(&g_msgq);
        msg.msgType = CMSG_INIT;
        msgq_in_irq(&g_msgq, &msg);
        break;

    default:
        break;
    }  

    return  0;
}

/*********************************************************************************************/

/*********************************************************************************************/
