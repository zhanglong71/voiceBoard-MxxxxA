#ifndef __L_TIMER_H
#define __L_TIMER_H
 
void TimingDelay_Decrement(void);
void TimingDelay_Increment(void);
void TIM_Config(void);
void vop_sendByPulse(void);
void vp_init(void);
void vp_play(u8 __vpIdx);
void key_scan(void);

#endif
