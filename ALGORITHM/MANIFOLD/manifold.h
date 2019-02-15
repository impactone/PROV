#ifndef __MANIFOLD_H
#define __MANIFOLD_H

#include "usart.h"

#define MAX_MANIFOLD_DATA 40000   //妙算接收数据的最大值
#define MANIFOLD_DATA_LENTH 6

u8 MANIFOLD_Decode(RecStru *data,u8 Lenth);
u8 isInAInterval(float data,float a,float b);
u8 hasEnemy(RecStru *data,float xReal,float yReal);
u8 ProcessMANIFOLD(RecStru *data,
										 float *angExpect,
                     float xReal,s32 *xExpect,
                     float yReal,s32 *yExpect,
                     u16 SystemCounter);

#endif

