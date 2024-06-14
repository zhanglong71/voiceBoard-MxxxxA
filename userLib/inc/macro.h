#ifndef __MACRO_H__
#define __MACRO_H__
/*******************************************************************************/
/***************************** 数组大小 ****************************************/
#define	MTABSIZE(x)	(sizeof(x)/sizeof(x[0]))
/*******************************************************************************/
#define	MIRQ_enable()	__enable_irq()
#define	MIRQ_disable()	__disable_irq()
/*********************************************************************************/
#if 1
#define MVopData_L(x)  do{GPIO_ResetBits(GPIOD, GPIO_Pin_2);}while(0)
#define MVopData_H(x) do{GPIO_SetBits(GPIOD, GPIO_Pin_2);}while(0)

#define MVopPower_on(x)  /** do{GPIO_SetBits(GPIOD, GPIO_Pin_3);}while(0) **/
#define MVopPower_off(x) /** do{GPIO_ResetBits(GPIOD, GPIO_Pin_3);}while(0) **/

#define MVop_busy(x) GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_3)
/*********************************************************************************/
// #define MWifi_enable(x) do{GPIO_SetBits(GPIOC, GPIO_Pin_5);}while(0)
// #define MWifi_disable(x) do{GPIO_ResetBits(GPIOC, GPIO_Pin_5);}while(0)
#endif
/*********************************************************************************/
#define M485TR_T(x) do{GPIO_SetBits(GPIOB, GPIO_Pin_4);}while(0)
#define M485TR_R(x) do{GPIO_ResetBits(GPIOB, GPIO_Pin_4);}while(0)
/*********************************************************************************/
#define MWifi_Enable(x) do{GPIO_SetBits(GPIOD, GPIO_Pin_1);}while(0)
/*********************************************************************************/
#define Mset_bit(var, offset) do{	\
        ((var) |= (1 << (offset))); \
    }while(0)
    
#define Mreset_bit(var, offset) do{	\
    ((var) &= ~(1 << (offset))); \
    }while(0)

#define Mget_bit(var, offset) \
    !(!((var) & (1 << (offset))))
/*********************************************************************************/

/*******************************************************************************/
#endif
