#ifndef __GLOBAL_H__
#define __GLOBAL_H__
/*******************************************************************************/
#include "ptype.h"

extern	fstack_t g_fstack;
extern	msgq_t  g_msgq;
extern	Timer_t g_timer[TIMER_NUM];	

extern u16 g_tick;
extern	u16 g_flag;

extern u8FIFO_t g_uart1TxQue;
extern u8FIFO_t g_uart1RxQue;

extern u8FIFO_t g_uart2TxQue;
extern u8FIFO_t g_uart2RxQue;

extern actionQueue_t g_promptQueue;
extern u8 g_IT_uart1_tmr;

extern NetInfo_t g_netInfo;
extern ComponentField_t g_componentStatus;

extern rs485transX_t g_rs485transX;

extern char g_buf[U8FIFOSIZE];
extern kv_t g_KVarr[CKVTABSIZE];
/*******************************************************************************/
#endif
