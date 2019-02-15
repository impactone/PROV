#ifndef __SELECT_H
#define __SELECT_H	 
#include "sys.h"

#define S0 PBout(3)	
#define S1 PBout(4)	
#define S2 PCout(4)	
#define S3 PCout(5)	

#define isBall1 GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)
#define isBall2 GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)

extern u8 isOver;
extern u16 red1,green1,blue1;
extern u16 red2,green2,blue2;

void Select_Init(void);//≥ı ºªØ
void RedFilter(void);
void GreenFilter(void);
void BlueFilter(void);
void FilterRate(u8 n);
void EXTIX_INIT(void);
void TIM5_Int_Init(u16 arr,u16 psc);
		 				    
#endif
