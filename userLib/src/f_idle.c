
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"
#include "l_gpio.h"
#include "l_u8FIFO.h"
#include "l_rs485.h"
#include "l_jsonTL.h"
#include "l_flash.h"
#include "l_actionFIFO.h"

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
    // char *ptr;
#define CREPORT_PERIOD (11)
    RetStatus retStatus = POK;
    switch(((msg_t *)pMsg)->msgType) 
    {
    case CMSG_TMR:
        g_tick++;

        if ((g_tick % CREPORT_PERIOD) == 1) {
            if (((g_componentStatus.status == CINDEX_STANDARD) || (g_componentStatus.status == CINDEX_HIGHPOWER)) &&
                (g_componentStatus.charge == CINDEX_CHARGING)) {
                /** work & charging, then exit **/
                g_componentStatus.status = CINDEX_STANDBY;
            }
            reportComponentStatus(g_componentStatus.status);
        }
        if (g_componentStatus.netConnection == CINDEX_NETCONNECTION_ON) {
            reportStatusOneByOne();
        }
        break;
        
    case CMSG_INIT:
        retStatus = reportVersion();
        if (retStatus != POK) {  // busy! try again later; giveup the 
            SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
        } else {
            g_tick = 0;
            SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
        }
        break;

     /** check double click block start **/
     case CMSG_DKEY:
        if (Mget_bit(g_flag, 1)) {   /** (g_flag & (1 << 1)) double click...  ok **/
            Mreset_bit(g_flag, 1);
            ClrTimer_irq(&g_timer[3]);
   
            // reportResetNet(); // !!!!!!!!!! double click ok!!! giveup !!!
        } else  { /**  **/
            Mset_bit(g_flag,1);
            SetTimer_irq(&g_timer[3], TIMER_400MS, CMSG_DCLK);
        }
        break;
    
    case CMSG_DCLK:
        Mreset_bit(g_flag, 1);
        ClrTimer_irq(&g_timer[3]);
        break;
    /** check double click block end **/

    case CMSG_KEY3S:
        /** charging and press key 3s, then reset wifi module **/
        if ((g_componentStatus.charge == CINDEX_CHARGING) || 
            (g_componentStatus.charge == CINDEX_CHARGECOMPLETE) || 
            (g_componentStatus.charge == CINDEX_CHARGEREPAIR) ||
            (g_componentStatus.charge == CINDEX_CHARGEFAULT)) {
            reportResetNet();     // !!!!!!!!!!
            vp_stop1();
            vp_stor(vopIdx_WifiReset);
        }
        break;

    case CMSG_TEST:
        break;
        
    default:
        break;
    }  

    return  0;
}
/*********************************************************************************************/

/*********************************************************************************************/
