#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"

/*******************************************************************************
 * IRQ_disable()/IRQ_enable() inline function implement in .h file ......
 *******************************************************************************/

/*******************************************************************************
 *
 * function stack operation
 *
 * note: top-1 指向的才是有效数据
 *
 *******************************************************************************/
void fstack_init(fstack_t *s)
{
    s->top = 0;
}

int fstack_pop(fstack_t *s, func_t *f)
{
    if((s->top <= 0) || (s->top > (MTABSIZE(s->func)))) {    /** make sure ... top [1..STACKSIZE - 1]  **/
        return    FALSE;
    }
    f->func = s->func[s->top - 1].func;
    f->arg = s->func[s->top - 1].arg;
    s->top--;
    return TRUE;
}

int fstack_push(fstack_t *s, func_t *f)   /** make sure ... top [0..STACKSIZE-2]  **/
{
    if(s->top >= (MTABSIZE(s->func))) {
        return    FALSE;
    }
    s->func[s->top].func = f->func;
    s->func[s->top].arg = f->arg;
    s->top++;
    
    return TRUE;
}

int fstack_update(fstack_t *s, func_t *f)
{
    /** Same As: stackpop() then stackpush() **/
    s->func[s->top - 1].func = f->func;
    s->func[s->top - 1].arg = f->arg;
    
    return TRUE;
}
/**
 * get function from the top of the stack
 *
 **/
int fstack_top(const fstack_t *s, func_t *f)   /** make sure ... top [1..STACKSIZE - 1]  **/
{
    if((s->top < 1) || (s->top > (MTABSIZE(s->func)))) {
        return    FALSE;
    }
    f->func = s->func[s->top - 1].func;
    f->arg = s->func[s->top - 1].arg;

    return TRUE;
}
/*******************************************************************************
 *
 * msg queue operation
 *
 *******************************************************************************/
int msg_init(msg_t *pMsg)
{
    if((pMsg != 0)) {
        pMsg->msgType = CMSG_NONE;
        return    TRUE;
    } else {
        return FALSE;
    }
}

void msgq_init(msgq_t *q)
{
    q->tail = q->head = 0;
}

static int isempty(msgq_t *q)
{
    if(q->tail == q->head) {
        return TRUE;
    }
    return FALSE;
}

static int isfull(msgq_t *q)
{
    if((q->tail + 1 == q->head) || (((q->tail + 1) % (MTABSIZE(q->msg))) == (q->head))) {
        return TRUE;
    }
    return FALSE;
}

int msgq_in(msgq_t *q, msg_t *val)            //Note: check queue full is necessary before invoke this routine
{
    if(isfull(q)) {
        return FALSE;
    }
    q->msg[q->tail].msgType = val->msgType;    
    q->msg[q->tail].msgValue = val->msgValue;
    
    q->tail = (q->tail + 1) % (MTABSIZE(q->msg));

    return TRUE;
}

int msgq_in_irq(msgq_t *q, msg_t *val)        //Note: check queue full is necessary before invoke this routine
{
    int iRet;
    
    IRQ_disable();
    iRet = msgq_in(q, val);
    IRQ_enable();

    return iRet;
}

int msgq_out(msgq_t *q, msg_t *val)        //Note: check queue empty is necessary before invoke this routine
{
    if(isempty(q)) {
        return  FALSE;
    }
    
    val->msgType = q->msg[q->head].msgType;
    val->msgValue = q->msg[q->head].msgValue;
    q->head = (q->head + 1) % (MTABSIZE(q->msg));
        
    return    TRUE;
}

int msgq_out_irq(msgq_t *q, msg_t *val)        //Note: check queue empty is necessary before invoke this routine
{
    int iRet;
    
    IRQ_disable();
    iRet = msgq_out(q, val);
    IRQ_enable();
        
    return    iRet;
}

/*******************************************************************************
 * Timer operation
 *******************************************************************************/
void SetTimer(Timer_t * timer, int tick, msgType_t msgType)
{
    if (timer == NULL) {
        return;
    }

    timer->tick = tick;
    timer->tick_bak = tick;
    timer->msgType = msgType;
}
void SetTimer_irq(Timer_t * timer, int tick, msgType_t msgType)
{
    IRQ_disable();
    SetTimer(timer, tick, msgType);
    IRQ_enable();
}

void ClrTimer(Timer_t * timer)
{
    if (timer == NULL) {
        return;
    }
    timer->tick = 0;
    timer->tick_bak = 0;
}
void ClrTimer_irq(Timer_t * timer)
{
    IRQ_disable();
    ClrTimer(timer);
    IRQ_enable();
}

/////////////////////////////////////////////////////
