#ifndef __NOKIA5110_PLAYER_H
#define __NOKIA5110_PLAYER_H			

#include "sys.h" 	 

//#define ADC1_DR_Address    ((u32)0x4001244C)
#define res0   GPIO_ResetBits(GPIOA, GPIO_Pin_1)  //��λ,0��λ
#define sce0   GPIO_ResetBits(GPIOA, GPIO_Pin_0)  //Ƭѡ��Ч��������������
#define dc0    GPIO_ResetBits(GPIOA, GPIO_Pin_4)   //�͵�ƽ0дָ��
#define sdin0  GPIO_ResetBits(GPIOA, GPIO_Pin_5)   //����
#define sclk0  GPIO_ResetBits(GPIOA, GPIO_Pin_8)   //ʱ��

#define res1   GPIO_SetBits(GPIOA, GPIO_Pin_1)  //��λ,0��λ
#define sce1   GPIO_SetBits(GPIOA, GPIO_Pin_0)  //Ƭѡ��Ч��������������
#define dc1    GPIO_SetBits(GPIOA, GPIO_Pin_4)   //�ߵ�ƽ1д����
#define sdin1  GPIO_SetBits(GPIOA, GPIO_Pin_5)   //����
#define sclk1  GPIO_SetBits(GPIOA, GPIO_Pin_8)   //ʱ��

void GPIO_Configuration(void);
void LCD_write_byte(unsigned char dt, unsigned char command);
void LCD_init(void);
void LCD_set_XY(unsigned char X,unsigned char Y);
void LCD_clear_A2B(u8 X,u8 Y,u8 Num);
void LCD_clearall(void);
void LCD_write_char(unsigned char c);
void LCD_write_String(unsigned char X,unsigned char Y,uint8_t *s);//�ַ�����������ʾ�̶�ֵ��С���壩
void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c);//��ʾ���֣������壩
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c);//��ʾ����
void LCD_write_Num(float Num,char *format,u8 X,u8 Y);
void LCD_write_number(unsigned char X,unsigned char Y,unsigned char c);//����ĻX,Y���괦��ʾ�������֣�С���壩	 
void LCD_clear_number(unsigned char X, unsigned char Y);//�����������
#endif


