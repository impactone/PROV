#ifndef __MAGNET_H
#define __MAGNET_H
#include "sys.h"

#define MAG_A PCout(3)
#define MAG_B PCout(4)
#define MAG_C PCout(5)
#define MAG_D PDout(4)

void Mag_Init(void);
void RaiseUp(void);
void PutDown(void);
u8 PushWhiteOnce(u16 SysCnt);
u8 PushBlackOnce(u16 SysCnt);
u8 PushWhite(u8 times,u16 SysCnt);
u8 PushBlack(u8 times,u16 SysCnt);

#endif
