#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#include "stm32f4xx.h"


/** 
  * @brief  Bmp structures definition
  */
typedef __packed struct __BmpInfo
{
  uint16_t width;
  uint16_t height;
  const uint8_t* pBmp;
}BmpInfoStructTypedDef;


/** 
  * @brief  翻转开关
  */
typedef enum
{
	INV_OFF = 0,
	INV_ON  = 1,
}InvChoose;


/** 
  * @brief  是 或 否
  */
typedef enum
{
	NOT = 0,
	IS  = 1,
}ISorNOT;


void OLED_SetPositionInPage(uint8_t x, uint8_t y);
void OLED_Init(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, ISorNOT isShow, ISorNOT isRefresh);
void OLED_RefreshGram(void);
//void OLED_DrawBmp(uint16_t x, uint16_t y, BmpInfoStructTypedDef* bmpInfo, InvChoose InvSW);
void OLED_DrawSomeBits(int16_t x, int16_t y, uint8_t wrtData, uint8_t effBits, \
        InvChoose InvSW, ISorNOT isRefresh);
void OLED_DrawBmp(int16_t x, int16_t y, uint16_t showW, uint16_t showH, \
        BmpInfoStructTypedDef* bmpInfo, InvChoose InvSW);
void OLED_PrintNum(float Num,char *format,
        int16_t x,int16_t y,uint16_t showW,uint16_t showH);
void OLED_Print6x8Str(int16_t x,int16_t y,uint16_t showW,uint16_t showH,
        uint8_t* pWrtData,InvChoose InvSW,ISorNOT isRefresh);
uint8_t* OLED_TextPrint(void* fmt, ...);
void OLED_Clear(void);


#endif


