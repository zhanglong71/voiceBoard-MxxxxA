#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"

#include "l_timer.h"
#include "l_arch.h"
#include "hk32f030xmxxa.h"

void TimingDelay_Decrement(void)
{
    msg_t msg;
    /***************************************************/
    for(int i = 0; i < MTABSIZE(g_timer); i++) {
        if(g_timer[i].tick_bak > 0) {
            if(g_timer[i].tick > 0) {
                g_timer[i].tick--;
            } else {
                g_timer[i].tick = g_timer[i].tick_bak;
                
                msg.msgType = g_timer[i].msgType;		/** message type **/
                msg.msgValue = i;						/** message value(Timer index) **/
                msgq_in(&g_msgq, &msg);
            }
        }
    }
}

void TimingDelay_Increment(void)
{
#define CUART1_TIMER_OUT (9)  // ??????¡ê?¡ä???¡ä¦Ì????    
    if (g_IT_uart1_tmr < CUART1_TIMER_OUT) {
        g_IT_uart1_tmr++;
        if (g_IT_uart1_tmr == 3) {
            msg_t msg;
            msg.msgType = CMSG_UART1RTOUT;
            msgq_in(&g_msgq, &msg);
        }
    }
}

/**
 * pressed key, then k11=H; released key, then k11=L
 **/
void key_scan(void)
{
	msg_t msg;
    static u8 tmr_key = 0;
    static u8 key_last = 0xff;
    static u16 tmr_down = 0;
#define CKEYTMR_3SEC (300)
    if (MGet_k11("scan k11") == Bit_SET) {
        if (!Mget_bit(key_last, 1)) {
            tmr_key++;
            if (tmr_key >= 2) { // debounce
                msg.msgType = CMSG_DKEY;
                msgq_in(&g_msgq, &msg);
                tmr_key = 0;
                Mset_bit(key_last, 1);
            }
        } else {
            tmr_key = 0;
        }

        /** Detect the duration of continuous pressing **/
        if (tmr_down++ >= CKEYTMR_3SEC) {
            msg.msgType = CMSG_KEY3S;
            msgq_in(&g_msgq, &msg);
            tmr_down = 0;
        }
    } else { /**KEY1 == Bit_RESET **/
        if (Mget_bit(key_last, 1)) {
            tmr_key++;
            if (tmr_key >= 2) { // debounce
                msg.msgType = CMSG_UKEY;
                msgq_in(&g_msgq, &msg);
                tmr_key = 0;
                Mreset_bit(key_last, 1);
            }
        } else {
            tmr_key = 0;
        }
        
        tmr_down = 0;
    }
}

/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;

  /* TIM2 clock enable */
  RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_TIM2, ENABLE);

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* -----------------------------------------------------------------------
  TIM2 Configuration: Output Compare Timing Mode:

  In this example TIM2 input clock (TIM3CLK) is set to APB1 clock (PCLK1),
    => TIM2CLK = PCLK1 = SystemCoreClock = 48 MHz

  To get TIM2 counter clock at 6 MHz, the prescaler is computed as follows:
     Prescaler = (TIM2CLK / TIM2 counter clock) - 1
     Prescaler = (PCLK1 /6 MHz) - 1

  CC1 update rate = TIM2 counter clock / CCR1_Val = 146.48 Hz
  ==> Toggling frequency = 73.24 Hz

  C2 update rate = TIM2 counter clock / CCR2_Val = 219.7 Hz
  ==> Toggling frequency = 109.8 Hz

  CC3 update rate = TIM2 counter clock / CCR3_Val = 439.4 Hz
  ==> Toggling frequency = 219.7 Hz

  CC4 update rate = TIM2 counter clock / CCR4_Val = 878.9 Hz
  ==> Toggling frequency = 439.4 Hz

  Note:
   SystemCoreClock variable holds HCLK frequency and is defined in system_hk32f0301mxxc.c file.
   Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
   function to update SystemCoreClock variable value. Otherwise, any configuration
   based on this variable will be incorrect.
  ----------------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock  / 6000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
#if 0

  /* Output Compare Timing Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
#endif
  /* TIM Interrupts enable */
  // TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
  // TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

/***************************************************************************/
#define	CVOPTMR_S	(40)
#define CVOPTMR_INIT	(CVOPTMR_S + 1 + (14 * 7 + 10))

volatile u8 g_tmr_sbuf;
int g_tmr_vop = CVOPTMR_INIT + 1;
void vop_sendByPulse(void)
{
/**
 * 0. CVOPTMR_INIT ???¡§??????
 * 1. 0 - CVOPTMR_S ???¡À¡ê???????ÁÙ¦Ì??¡À??5ms)¡ê?CVOPTMR_S????¦×??????¡ä¦Ë?¡¥?(¡¤¨¤??¦Ì» ??¡§????¡¥?)
 * 2. 
 **/
#define VOPRESET_L(x)
#define VOPRESET_H(x)
    //int g_tmr_iVopBusy = 0;    // no use
    //int g_tmr_iVopFault = 0; // no use
    //static int bit_count = 0;
#if	1
    if(g_tmr_vop <= CVOPTMR_INIT) {  //working
        g_tmr_vop++;
        if ((g_tmr_vop < (CVOPTMR_S))) {
            MVopData_L();
        } else  /* ((g_tmr_vop >= (CVOPTMR_S))) */  {
            if(g_tmr_vop - (CVOPTMR_S) == 0) {   //?????
                /** wait for what ?  reset ?**/
            } else if((g_tmr_vop - (CVOPTMR_S) - 5) >= 0) { /** 10ms L -> start send **/
                #define MSENDTIMING(x)	((g_tmr_vop) - (CVOPTMR_S) - 5)
                /*******************************************
                 * bit - 1 - HHH/L - 375us/125us
                 * bit - 0 - H/LLL - 125us/375us
                 *******************************************/
                if(MSENDTIMING("32pulse = 4pulse/bit * 8bit") < 32) {
                    if((MSENDTIMING("Hxxx") & 0x3) == 0x0) {
                        MVopData_H("Hxxx");
                    } else if(((MSENDTIMING("xXxx") & 0x3) == 0x1) || ((MSENDTIMING("xxXx") & 0x3) == 0x2)) {
                        if (Mget_bit(g_tmr_sbuf, (MSENDTIMING("HxxL") >> 2))) {
                           MVopData_H("xHHx");
                        } else {
                           MVopData_L("xLLx");
                        }
                    } else if((MSENDTIMING("xxxL") & 0x3) == 0x3) {
                        MVopData_L("xxxL");
                    }
                } else {   //¨¹¨¢???¡À?                    g_tmr_vop = CVOPTMR_INIT + 1;
                    MVopData_H("xHHHH...");
                    // g_tmr_iVopBusy = 0;
                }
            }
        } 
    } else {
        /**
         * (g_tmr_vop > CVOPTMR_INIT) 
         **/
        //MVopData_H("xHHHH...");        //????? (idle)
    }
#endif

}

// volatile u8 g_tmr_sbuf;
// void setSbuf(u8 __sbuf)
void vp_play(u8 __vpIdx)
{
    IRQ_disable();
    g_tmr_sbuf = __vpIdx;
   	g_tmr_vop = 0;
    IRQ_enable();
}

void vp_init(void)
{
    g_tmr_vop = CVOPTMR_INIT;
}

