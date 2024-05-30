#ifndef __L_ACTIONFIFO_H__
#define __L_ACTIONFIFO_H__

//void actionQueueInit(actionQueue_t *q);
//void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t msgType);

void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t stepMsgType, msgType_t overMsgType);
int actionQueueIn(actionQueue_t *q, action_t *action);
int actionQueueOut(actionQueue_t *q, action_t *action);

void vp_stor(unsigned char __vpIdx);
void actionDelay(int _delay);

//static void startAction(actionQueue_t *q, action_t * pAction);
//static void stopAction(actionQueue_t *q)
void promptInit(void);
void vp_next(void);
void vp_stop1(void);
// void vp_setVolume(u8 __vol);
// void vp_setDefaultVolume(void);

void actionDoing(actionQueue_t * q);
void actProcess(actionQueue_t * q);

#endif
