#include "CONST.h"
#include "ptype.h"
#include "macro.h"

#include "l_arch.h"
#include "l_u8FIFO.h"
/*******************************************************************************
 * check is the FIFO empty
 * note: no change argument, so no need disable interrupt
 *******************************************************************************/
int u8FIFOisEmpty(const u8FIFO_t *q)
{
	if(q->in == q->out)
	{
		return TRUE;
	}
	return	FALSE;
}

/*******************************************************************************
 * get the number of data()
 * note: 
 *******************************************************************************/
int u8FIFOlength(u8FIFO_t *q)
{
	if(q->in < q->out)
	{
		return (U8FIFOSIZE + q->in - q->out);
	}
	return	(q->in - q->out);
}

/*******************************************************************************
 * check is the FIFO full
 *******************************************************************************/
int u8FIFOisFull(const u8FIFO_t *q)
{
	if((q->in + 1 == q->out) || (((q->in + 1) % U8FIFOSIZE) == q->out))
	{
		return TRUE;
	}
	return	FALSE;
}

/*******************************************************************************
 * initial the FIFO
 *******************************************************************************/
void u8FIFOinit(u8FIFO_t *q)
{
	q->in = 0;
	q->out = 0;
	q->out2 = 0;
}

void u8FIFOinit_irq(u8FIFO_t *q)
{
    IRQ_disable();
    u8FIFOinit(q);
    IRQ_enable();
}
/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
int u8FIFOin(u8FIFO_t *q, u8Data_t *u8Data)
{
    if(u8FIFOisFull(q))
    {
        return	FALSE;
    }

    q->buf[q->in] = u8Data->u8Val;
	q->in = (q->in + 1) % U8FIFOSIZE;

    return TRUE;
}

int u8FIFOin_irq(u8FIFO_t *q, u8Data_t *u8Data)
{
	int iRet;

    IRQ_disable();
    iRet = u8FIFOin(q, u8Data);
    IRQ_enable();

	return	iRet;
}

int u8FIFOout(u8FIFO_t *q, u8Data_t *u8Data)
{
    if(u8FIFOisEmpty(q))
    {
        return	FALSE;
    }
    
    u8Data->u8Val = q->buf[q->out];
	q->out = ((q->out + 1) % U8FIFOSIZE);

    return TRUE;
}

int u8FIFOout_irq(u8FIFO_t *q, u8Data_t *u8Data)
{
    int iRet;

    IRQ_disable();
    iRet = u8FIFOout(q, u8Data);
	IRQ_enable();
	    
	return	iRet;
}

/*******************************************************************************
 * get data from FIFO
 * no change the FIFO
 *******************************************************************************/
int u8FIFO_get(const u8FIFO_t *q, u8 offset, u8 *dst)
{
    if(u8FIFOisEmpty(q))
    {
        return	FALSE;
    }

    u8 start = (q->out + offset) % U8FIFOSIZE;
    u8 end = q->in;
    u8 curr = 0;

    for (; start != end; start = ((start + 1) % U8FIFOSIZE), curr++) {
        dst[curr] = q->buf[start];
    }
    dst[curr] = '\0';

    return TRUE;
}

/*******************************************************************************/

/*******************************************************************************
 * the FIFO interface2, have nothing todo with interface1
 *******************************************************************************/
 int u8FIFOisEmpty2(const u8FIFO_t *q)
{
	if(q->in == q->out2)
	{
		return TRUE;
	}
	return	FALSE;
}
int u8FIFOlength2(u8FIFO_t *q)
{
	if(q->in < q->out2)
	{
		return (U8FIFOSIZE + q->in - q->out2);
	}
	return	(q->in - q->out2);
}

int u8FIFOout2(u8FIFO_t *q, u8Data_t *u8Data)
{
    if(u8FIFOisEmpty2(q))
    {
        return  FALSE;
    }
    
    u8Data->u8Val = q->buf[q->out2];
    q->out2 = ((q->out2 + 1) % U8FIFOSIZE);

    return TRUE;
}
int u8FIFOout2_irq(u8FIFO_t *q, u8Data_t *u8Data)
{
    int iRet;

    IRQ_disable();
    iRet = u8FIFOout2(q, u8Data);
	IRQ_enable();
	    
	return	iRet;
}

/*******************************************************************************/

/*******************************************************************************/

/////////////////////////////////////////////////////
