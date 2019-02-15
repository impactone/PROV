#ifndef __LASER_H
#define __LASET_H

#include "sys.h"

#define filterNum 10
#define isTooLong GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)

void Laser_Init(void);
u16 Get_Adc(u8 ch);
float Get_Dis(void);
float ave_Filter(u32 temp_val);

#endif

