#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H	
#include "sys.h"

#define UltraSonic_1 1
#define UltraSonic_2 2
#define UltraSonic_3 3
#define UltraSonic_4 4

extern u16 Dis_1;
extern u16 Dis_2;
extern u16 Dis_3;
extern u16 Dis_4;

extern u8 UltraSonicNum;

void UltraSonic_Init(void);
u8 ScanDis(void);

#endif
