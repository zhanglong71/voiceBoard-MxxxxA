#ifndef __L_GPIO_H_
#define __L_GPIO_H_

void GPIO_initVOPPort(void);
void watchDog_init(void);
void GPIO_keyScanInit(void);
void GPIO_wifiModuleInit(void);

void GPIO_init4led(void);
void GPIO_led_blink(void);

#endif
