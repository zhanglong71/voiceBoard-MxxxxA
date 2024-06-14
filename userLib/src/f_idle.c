
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"
#include "l_gpio.h"
#include "l_u8FIFO.h"
#include "l_rs485.h"
#include "l_jsonTL.h"
#include "f_init.h"
#include "f_idle.h"

#include "stdio.h"
#include "string.h"
#include "main.h"

/*********************************************************************************************/
int f_idle(void *pMsg)
{
//    int len;
//    u8Data_t u8Data;
    
    RetStatus retStatus = POK;
    switch(((msg_t *)pMsg)->msgType) 
    {
    case CMSG_TMR:
        g_tick++;

        if ((g_tick % 10) == 1) {
            if ((g_componentStatus.status != CINDEX_STANDBY) && (g_componentStatus.charge == CINDEX_CHARGING)) {
                /** work & charging, then exit **/
                g_componentStatus.status = CINDEX_STANDBY;
            }
            reportComponentStatus(g_componentStatus.status);
        }
        break;
        
    case CMSG_INIT:
        retStatus = reportVersion();
        if (retStatus != POK) {  // busy! try again later; giveup the 
            SetTimer_irq(&g_timer[0], TIMER_100MS, CSYS_INIT);
        } else {
            g_tick = 0;
            SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
        }
        break;

    case CMSG_TEST:
    case CMSG_2TEST:
        // test only ????????????????????
        #if 0
        if (u8FIFOout_irq(&g_uart1RxQue, &u8Data) == TRUE) {
            rs485_stor_irq(&u8Data);
        }
       
        if (u8FIFOout_irq(&g_uart2RxQue, &u8Data) == TRUE) {
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        }
        #endif
        break;
        
    default:
        break;
    }  

    return  0;
}
/*********************************************************************************************/

/*********************************************************************************************/
