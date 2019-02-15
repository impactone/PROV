 #ifndef __5110_H
 #define __5110_H


 #define sce0   GPIO_ResetBits(GPIOA, GPIO_Pin_0)  //片选
 #define res0   GPIO_ResetBits(GPIOA, GPIO_Pin_1)  //复位,0复位
 #define dc0    GPIO_ResetBits(GPIOA, GPIO_Pin_4)   //1写数据，0写指令
 #define sdin0  GPIO_ResetBits(GPIOA, GPIO_Pin_5)   //数据
 #define sclk0  GPIO_ResetBits(GPIOA, GPIO_Pin_8)   //时钟


 #define sce1   GPIO_SetBits(GPIOA, GPIO_Pin_0)  //片选
 #define res1   GPIO_SetBits(GPIOA, GPIO_Pin_1)  //复位,0复位
 #define dc1    GPIO_SetBits(GPIOA, GPIO_Pin_4)   //1写数据，0写指令
 #define sdin1  GPIO_SetBits(GPIOA, GPIO_Pin_5)   //数据
 #define sclk1  GPIO_SetBits(GPIOA, GPIO_Pin_8)   //时钟
	    
void GPIO_Configuration_LCD(void);
void LCD_WriteByte(unsigned char dt, unsigned char command);
void LCD_Init(void);
void LCD_Set_XY(unsigned char X, unsigned char Y);
void LCD_Clear(void);
void LCD_WriteChar(unsigned char X,unsigned char Y,unsigned char c);
void LCD_WriteString_en(unsigned char X,unsigned char Y,unsigned char *s);
void LCD_WriteNumber(unsigned char x,unsigned char y, unsigned long number);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                              unsigned char ch_with,unsigned char num,
							  unsigned char line,unsigned char row);
void LCD_Write_cn(unsigned char row, unsigned char page,unsigned char c); //row:列 page:页 dd:字符
void LCD_clear_number(unsigned char X, unsigned char Y);
void LCD_write_Number(unsigned char X,unsigned char Y,double num);//显示连续数字XXXXX.XXXX（小字体）

#endif
