#include "hk32f030xmxxa_it.h"

#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"

#include "l_timer.h"
#include "l_arch.h"
#include "l_gpio.h"
#include "l_actionFIFO.h"

/*******************************************************************************
 * check is the queue empty
 *******************************************************************************/
static int isActionQueEmpty(actionQueue_t *q)
{
    if(q->tail == q->head) {
        return TRUE;
    }
    return    FALSE;
}

/*******************************************************************************
 * check is the queue full
 *******************************************************************************/
static int isActionQueFull(actionQueue_t *q)
{
    // if((q->tail + 1 == q->head) || (((q->tail + 1) % (ACTIONQUEUESIZE)) == q->head)) {
    if(((q->tail + 1) == q->head) || (((q->tail + 1) % (MTABSIZE(q->buf))) == q->head)) {
        return TRUE;
    }
    return    FALSE;
}

static void actionQueueClear(actionQueue_t *q)
{
    q->tail = 0;
    q->head = 0;
    q->flag = 0;
}

/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t stepMsgType, msgType_t overMsgType)
{
    q->tail = 0;
    q->head = 0;
    // q->flag = 0;
    
    q->stepMsgType = stepMsgType;  
    q->overMsgType = overMsgType;
    q->timer = timer;
    ClrTimer_irq(q->timer);
}

/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
int actionQueueIn(actionQueue_t *q, action_t *action)
{
    if(isActionQueFull(q) == TRUE) {  
        return FALSE;
    }

    q->buf[q->tail].actionType = action->actionType;
    q->buf[q->tail].actionTime = action->actionTime;
    q->buf[q->tail].actionPara = action->actionPara;
    //q->buf[q->tail].func       = action->func;
    q->tail = (q->tail + 1) % (MTABSIZE(q->buf));
 
    return TRUE;
}

int actionQueueOut(actionQueue_t *q, action_t *action)
{
    if(isActionQueEmpty(q) == TRUE) {
        return FALSE;
    }
    
    action->actionType = q->buf[q->head].actionType;
    action->actionTime = q->buf[q->head].actionTime;
    action->actionPara = q->buf[q->head].actionPara;
    //action->func       = q->buf[q->head].func;
    
    q->head = (q->head + 1) % (MTABSIZE(q->buf));
       
    return TRUE;
}

/*********************************************************************************/
void vp_stor(unsigned char __vpIdx)
{
	action_t action;
/** step 1 preparation **/
    action.actionType = CVOP_PWRON;
    action.actionTime = TIMER_200MS;
    actionQueueIn(&g_promptQueue, &action);
/** step 2 doing **/
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_6SEC;
    action.actionPara = __vpIdx;
    actionQueueIn(&g_promptQueue, &action);
/** step 3 end **/
    action.actionType = CVOP_PWROFF;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_promptQueue, &action);
}

#if 0
void vp_setVolume(u8 __vol)
{
	action_t action;

    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_100MS;
    action.actionPara = ((__vol & 0x0f) | 0xe0);
    actionQueueIn(&g_promptQueue, &action);
}
#endif

#if 0
void vp_setDefaultVolume(void)
{
	action_t action;
    
    action.actionType = CVOP_PWRON;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_promptQueue, &action);

    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_100MS;
    action.actionPara = 0xe3;
    actionQueueIn(&g_promptQueue, &action);

}
#endif

/**
 * similer as timer delay
 **/
void actionDelay(int _delay)
{
	action_t action;
    action.actionType = CACT_DELAY;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

/*******************************************************************************
 * start action and set action flag(and clear the last action flag)
 *******************************************************************************/
static void startAction(actionQueue_t *q, action_t * pAction)
{
	switch(pAction->actionType) 
    {
	case CACT_DELAY:
		q->flag = (1<<3);
        /** nothing **/
		break;
	
	case CACT_VOPON:
		q->flag = (1<<2);
		vp_play((u8)pAction->actionPara);
		break;
    case CVOP_PWRON:
        q->flag = (1<<1);
        MVopPower_on("VOP power on");
        break;
    case CVOP_PWROFF:
        // GPIO_VOPPWR_off();
        MVopPower_off("VOP power off");
        break;
        
	default:
		/** don't goto here **/
		q->flag = 0;
		break;
	}
}

 /*******************************************************************************
  * stop the action; no flag modify
  *
  * 1. play VOP --- stop VOP
  * 2. VOP circuit on --- do nothing
  * 3. VOP circuit off --- do nothing
  * 4. delay --- do nothing
  *******************************************************************************/
static void stopAction(actionQueue_t *q)
{
  	/** stop the timeout action  **/
    if(q->flag & (1<<3)) {
        /** delay... do Nothing **/
    } else if(q->flag & (1<<2)) {
	    vp_play((u8)CVOP_STOP);
    } else if(q->flag & (1<<1)) {
        // GPIO_VOPPWR_off();   /** do nothing **/
        //Mreset_bit(q->flag, 1);
    } else if(q->flag & (1<<0)) {
        // GPIO_VOPPWR_off();   /** do nothing **/
        //Mreset_bit(q->flag, 0);
  	} else {
	}
}

/** 清空动作队列，并且停止当前动作 **/
void promptInit(void)
{
	actionQueueInit(&g_promptQueue, &(g_timer[2]), CPMT_TOUT, CPMT_OVER);
	stopAction(&g_promptQueue);
}

void vp_next(void)
{
#if 0
    msg_t msg;
    msg.msgType = g_promptQueue.stepMsgType;
    msgq_in_irq(&g_msgq, &msg);
#endif
}

/**
 * the stop is used to end VOP play!
 **/
void vp_stop(void)
{
    /** stop last and  **/
    ClrTimer_irq(g_promptQueue.timer);
    stopAction(&g_promptQueue);
    actionQueueClear(&g_promptQueue);
    // GPIO_VOPPWR_off();
    MVopPower_off("VOP power off");
}

/**
 * the stop ACTION is used to start another VOP(no need stop last VOP)
 * 
 **/
void vp_stop1(void)
{
    ClrTimer_irq(g_promptQueue.timer);
    actionQueueClear(&g_promptQueue);
}

/** 
 * event queue process
 * check the queue and start action when idle(idle ==> doing)
 **/
void actionDoing(actionQueue_t * q)
{
	action_t action;
	
	if(q->flag == 0) {
		if(actionQueueOut(q, &action) == TRUE) {
            /** step1 start action and set flag **/
			startAction(q, &action);
            /** step2 timer ... **/
	        SetTimer_irq(q->timer, action.actionTime, q->stepMsgType);
		}
	} else {

	}
}

/*************************************************
 * one section over, then start another section
 * otherwise action over
 *************************************************/
void actProcess(actionQueue_t * q)
{
//     msg_t msg;
	action_t action;
	
	/** stop the timeout action first  **/
	stopAction(q);
  	ClrTimer_irq(q->timer);
    
	/** start the next action **/
	if(actionQueueOut(q, &action) == TRUE) {
        /** step1 start action and set new flag **/
		startAction(q, &action);
        /** step2 start new timer **/
	    SetTimer_irq(q->timer, action.actionTime, q->stepMsgType);
	} else {
		if(q->flag != 0) {
			q->flag = 0;
			SetTimer_irq(q->timer, TIMER_10MS, q->overMsgType);
            
            //msg.msgType = q->overMsgType;
            //msgq_in_irq(&g_msgq, &msg);
		}
	}
}

