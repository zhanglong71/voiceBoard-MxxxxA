// #include "hk32f0301mxxa_it.h"
#include <string.h>
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "version.h"

#include "l_arch.h"
#include "l_u8FIFO.h"
#include "l_rs485.h"
#include "hk32f030xmxxa.h"
/*******************************************************************************
 * rs485 translate status:
 * SM_RS485_IDLE  --- wait for start transmit
 * SM_RS485_START --- prepare(prefix) before transmit(maybe do nothing)
 * SM_RS485_SEND  --- doing transmit
 * SM_RS485_OVER  --- over(postfix) after transmit(maybe do nothing)
 *******************************************************************************/
RetStatus rs485_stor_irq(u8Data_t *u8Data)
{
    if (g_rs485transX.sm_status != SM_RS485_IDLE) {
        return PERROR;  // busy
    }
    u8FIFOin_irq(g_rs485transX.pfifo, u8Data);
    return POK;
}

void rs485Init(void)
{
	g_rs485transX.pfifo = &g_uart1TxQue;
    g_rs485transX.sm_status = SM_RS485_IDLE;
    g_rs485transX.stepMsgType = CMSG_485STEP;
    g_rs485transX.overMsgType = CMSG_485OVER;
//    ClrTimer_irq(g_rs485transX.ptimer);
}

void rs485TransXDoing(void)
{
    switch(g_rs485transX.sm_status)
    {
    case SM_RS485_IDLE:
        if(u8FIFOisEmpty(g_rs485transX.pfifo) != TRUE) {
            g_rs485transX.sm_status = SM_RS485_START;
            msg_t msg;
            msg.msgType = g_rs485transX.stepMsgType;
            msgq_in_irq(&g_msgq, &msg);
        }
        break;
    case SM_RS485_START:
        /** do nothing(DE=H 50us... only wait timer) **/
        break;
        
    case SM_RS485_SEND:
        if(u8FIFOisEmpty(g_rs485transX.pfifo) == TRUE) { /** 发完成后回发空闲状态 **/
            g_rs485transX.sm_status = SM_RS485_IDLE;
        }
        break;

    default:
        break;
	}
}

void rs485_send_start(void)
{
    /* Enable transfer empty interrupt */
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  

    /* Enable transfer complete interrupt */
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
}

/*****************************************************************************
 * process the "C485_TOUT" only
 *****************************************************************************/
void rs485actProcess(void)
{
    switch(g_rs485transX.sm_status)
    {
    case SM_RS485_IDLE:
        /** do nothing **/
        break;
        
    case SM_RS485_START:
        g_rs485transX.sm_status = SM_RS485_SEND;
        //ClrTimer_irq(rs485transX.ptimer);
        M485TR_T("发送前置高");
        rs485_send_start();
        break;
        
    case SM_RS485_SEND:
        /** do nothing **/
        break;
                    
    default:
        break;
	}
}

/*****************************************************************************
 * process the "C485_OVER"
 *****************************************************************************/
void rs485actOver(void)
{
#if 0
    switch(rs485transX.sm_status)
    {
    case SM_RS485_IDLE:
    case SM_RS485_START:
        /** do nothing **/
        break;
        
    case SM_RS485_SEND:  // send complete
        rs485transX.sm_status = SM_RS485_IDLE;
        ClrTimer_irq(rs485transX.ptimer);
        //M485TR_R("发送完成，进入等待接收状态");
        break;
                    
    default:
        break;
	}
#endif
}

/**********************************************************************************************/
RetStatus reportVersion(void)
{
    u8 i = 0;
    u8Data_t u8Data;
    char buf[32];
    RetStatus retStatus = POK;
    
    //sprintf(buf, "voice softVER:202403201423.%s" CVERSION);
    sprintf(buf, "%s.%s.%s.%s", CBOARD, CWARE, CDATETIME, CVERSION);

    for (i = 0; ((i < strlen(buf)) && (i < MTABSIZE(buf))); i++) {
        u8Data.u8Val = buf[i];
        retStatus = rs485_stor_irq(&u8Data);
        if (retStatus != POK) {
            return retStatus;
        }
    }
    return retStatus;
}

/**********************************************************************************************/

/*********************************************
 * {"voi":xxx,"ver":vxxxx}
 *********************************************/
void generateVoiceAckVer(char* to, u8* ver)
{
    u8 i = 0;
    u8Data_t u8Data;
    char buf[32];
    
    sprintf(buf, "{\"voi\":%s,\"ver\":%s}", to, ver);

    for (i = 0; ((i < strlen(buf)) && (i < MTABSIZE(buf))); i++) {
        u8Data.u8Val = buf[i];
        rs485_stor_irq(&u8Data);
    }
}

/*********************************************
 * {"voi":xxxx,"PLY":OK}
 * {"voi":xxxx,"PLY":err}
 *********************************************/
 void generateVoiceAckOk(char* to, u8 seq)
{
    u8 i = 0;
    u8Data_t u8Data;
    char buf[64];
    
    sprintf(buf, "{\"voi\":%s,\"PLY\":OK,\"SEQ\":%d}", to, seq);

    for (i = 0; ((i < strlen(buf)) && (i < MTABSIZE(buf))); i++) {
        u8Data.u8Val = buf[i];
        //u8FIFOin_irq(&g_uart1TxQue, &u8Data);
        rs485_stor_irq(&u8Data);
    }
}

 void generateVoiceAckErr(char* to, u8 seq)
{
    u8 i = 0;
    u8Data_t u8Data;
    char buf[64];
    
    sprintf(buf, "{\"voi\":%s,\"PLY\":err,\"SEQ\":%d}", to, seq);

    for (i = 0; ((i < strlen(buf)) && (i < MTABSIZE(buf))); i++) {
        u8Data.u8Val = buf[i];
        //u8FIFOin_irq(&g_uart1TxQue, &u8Data);
        rs485_stor_irq(&u8Data);
    }
}

/*********************************************
 * {"voi":user,"ask":sta}
 *********************************************/
 void generateAskstatus(void)
 {
     u8 i = 0;
     u8Data_t u8Data;
     char buf[] = "{\"xxx\":user,\"ask\":sta}";

     for (i = 0; ((i < strlen(buf)) && (i < MTABSIZE(buf))); i++) {
         u8Data.u8Val = buf[i];
         rs485_stor_irq(&u8Data);
     }
 }

