#ifndef __NOKIA5110_PLAYER_H
#define __NOKIA5110_PLAYER_H			

#include "sys.h" 	 

//#define ADC1_DR_Address    ((u32)0x4001244C)
#define res0   GPIO_ResetBits(GPIOA, GPIO_Pin_1)  //复位,0复位
#define sce0   GPIO_ResetBits(GPIOA, GPIO_Pin_0)  //片选有效，可以输入数据
#define dc0    GPIO_ResetBits(GPIOA, GPIO_Pin_4)   //低电平0写指令
#define sdin0  GPIO_ResetBits(GPIOA, GPIO_Pin_5)   //数据
#define sclk0  GPIO_ResetBits(GPIOA, GPIO_Pin_8)   //时钟

#define res1   GPIO_SetBits(GPIOA, GPIO_Pin_1)  //复位,0复位
#define sce1   GPIO_SetBits(GPIOA, GPIO_Pin_0)  //片选无效，不能输入数据
#define dc1    GPIO_SetBits(GPIOA, GPIO_Pin_4)   //高电平1写数据
#define sdin1  GPIO_SetBits(GPIOA, GPIO_Pin_5)   //数据
#define sclk1  GPIO_SetBits(GPIOA, GPIO_Pin_8)   //时钟

void GPIO_Configuration(void);
void LCD_write_byte(unsigned char dt, unsigned char command);
void LCD_init(void);
void LCD_set_XY(unsigned char X,unsigned char Y);
void LCD_clear_A2B(u8 X,u8 Y,u8 Num);
void LCD_clearall(void);
void LCD_write_char(unsigned char c);
void LCD_write_String(unsigned char X,unsigned char Y,uint8_t *s);//字符串函数，显示固定值（小字体）
void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c);//显示数字（大字体）
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c);//显示汉字
void LCD_write_Num(float Num,char *format,u8 X,u8 Y);
void LCD_write_number(unsigned char X,unsigned char Y,unsigned char c);//在屏幕X,Y坐标处显示单个数字（小字体）	 
void LCD_clear_number(unsigned char X, unsigned char Y);//清除单个数字
#endif


