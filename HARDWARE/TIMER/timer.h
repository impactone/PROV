#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define SYSTEM_COUNT  65000 

#define COUNT_10MS    1
#define COUNT_20MS    2
#define COUNT_30MS    3
#define COUNT_80MS    8
#define COUNT_90MS    9
#define COUNT_100MS   10
#define COUNT_1MIN    10
#define COUNT_2MIN    20
#define COUNT_3MIN    30


extern uint8_t __10ms;
extern uint8_t __20ms;
extern uint8_t __30ms;
extern uint8_t __80ms;
extern uint8_t __90ms;
extern uint8_t __100ms;
extern uint8_t SystemInitReady;

void TIM1_Init(u32 arr,u16 psc);
void TIM2_Init(u16 arr,u16 psc);
void TIM4_Init(u16 arr,u16 psc);
uint16_t Get_Time_Micros(void);
void TIM_Configuration(void);
 
#endif
