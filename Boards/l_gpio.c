
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "l_arch.h"
#include "l_gpio.h"

#include "main.h"

void GPIO_initVOPPort(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOD Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  
    /* Configure PD02() in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
      
    /* Configure PD03(busy) in input mode for busy status */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
  
    // GPIO_SetBits(GPIOD, GPIO_Pin_2);
    MVopData_H("init status: H");
    // GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    MVop_busy("get voice IC busy status");
}

void watchDog_init(void)
{
#if 1
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
       dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    
    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/32)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128
     */
    IWDG_SetReload(128000 / 128);
    
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
    
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
#endif
}

void GPIO_keyScanInit(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
   
    /* Enable GPIOC clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    
    /* Configure PC7 pin as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**********************
 * for output pin 
 **********************/
void GPIO_init4led(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    /* GPIOC Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
    /* Configure PC03 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
  
    /** wifi enable **/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOC, GPIO_Pin_3);
}

void GPIO_led_blink(void)
{
    GPIO_Toggle(GPIOD, GPIO_Pin_3);
    GPIO_Toggle(GPIOD, GPIO_Pin_4);
}
