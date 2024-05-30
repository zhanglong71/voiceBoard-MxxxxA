#ifndef __L_U8FIFO_H__
#define __L_U8FIFO_H__


int u8FIFOisEmpty(const u8FIFO_t *q);
int u8FIFOlength(u8FIFO_t *q);
int u8FIFOisFull(const u8FIFO_t *q);
void u8FIFOinit(u8FIFO_t *q);
void u8FIFOinit_irq(u8FIFO_t *q);
int u8FIFOin(u8FIFO_t *q, u8Data_t *u8Data);
int u8FIFOin_irq(u8FIFO_t *q, u8Data_t *u8Data);
int u8FIFOout(u8FIFO_t *q, u8Data_t *u8Data);
int u8FIFOout_irq(u8FIFO_t *q, u8Data_t *u8Data);
int u8FIFO_get(const u8FIFO_t *q, u8 offset, u8 *dst);
//int u8FIFO_last(const u8FIFO_t *q, u8* ch);

int u8FIFOout2_irq(u8FIFO_t *q, u8Data_t *u8Data);
int u8FIFOlength2(u8FIFO_t *q);

#endif
