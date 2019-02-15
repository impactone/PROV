#include "Nokia5110_library.h"
#include "Nokia5110_player.h"
#include "delay.h"
#include "stdio.h"


//硬件电路：开发板上，nokia5110液晶模块与STM32的IO连接情况如下：(F407专用！)
//	RST  res   PB1   PC4 	//复位,0复位
//	CE   sce   PB12  PC3	//片选
//	DC   dc    PB0   PC0	//1写数据，0写指令
//	Din  sdin  PC5   PC1	//数据
//	Clk  sclk  PC4   PC2	//时钟
//	BL   backled  PA8  PC5 	//背光
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

}

void LCD_write_byte(unsigned char dt, unsigned char command)
{
	unsigned char i;  	
    sce0; 	
	if(command)	dc1;
	else   dc0;		
	for(i=0;i<8;i++)
	{ 
		if(dt&0x80)   sdin1;
		else   sdin0;
		dt=dt<<1;	
		sclk0; 			
		sclk1; 		  
		sclk0;		
	}	  
}
//---------------------------------------
//名称: 5110LCD初始化函数
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//----------------------------------------- 
void LCD_init(void)
{
	sce1;
	res1;
	delay_ms(10);
	res0;  	
  delay_ms(100);
  res1;  
	delay_ms(10);
	LCD_write_byte(0x21,0);//LCD功能设置：芯片活动，水平寻址，使用扩展指令
	LCD_write_byte(0xd0,0);//设置VOP值，室温下的编程范围为3.00-10.68
	//Vlcd=3.06+(VOP)*0.06,本例VOP为0B0101 0000为十进制的80，Vlcd=7.86V
	LCD_write_byte(0x20,0);//LCD功能设置：芯片活动，水平寻址，使用基本指令
	LCD_write_byte(0x0C,0);//设定显示配置:普通模式
	sce1;
	LCD_clearall();
}
//---------------------------------------
//名称: 设置坐标函数
//参数：X：0－83 Y：0－5
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//-----------------------------------------   
void LCD_set_XY(unsigned char X, unsigned char Y)//X范围是0~200；Y范围是0~5！！
{
	LCD_write_byte(0x40 | Y, 0);// column
	LCD_write_byte(0x80 | X, 0);// row
	sce1;
} 
//---------------------------------------
//名称: LCD清屏函数
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//----------------------------------------- 
void LCD_clearall(void)
{
	unsigned char t;
	unsigned char k;
	LCD_set_XY(0,0);
	for(t=0;t<6;t++)
	{ 
		for(k=0;k<84;k++)
		{ 
			LCD_write_byte(0x00,1);	 						
		} 
	}
	sce1;
}

void LCD_clear_A2B(u8 X,u8 Y,u8 Num)
{
	u8 k;	
	LCD_set_XY(X,Y);
	if (X+6 < 84)
	{
		for(k=0;k<6;k++)
		{ 
			LCD_write_byte(0x00,1);		
		}
	}
	sce1;
}

void LCD_write_Num(float Num,char *format,u8 X,u8 Y)
{
	char Buff[30];
	sprintf(Buff,format,Num);
	LCD_write_String(X,Y,Buff);
}

void LCD_clear_number(unsigned char X, unsigned char Y)
{
	unsigned char k;
	LCD_set_XY(X,Y);
		for(k=0;k<6;k++)
		{ 
			LCD_write_byte(0x00,1);	 						
		}
	sce1;
}
//---------------------------------------
//名称: 显示英文字符
//参数：c：显示的字符在font6x8表格中的位置
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//-----------------------------------------  	
void LCD_write_char(unsigned char c)
{
	unsigned char line;
	c-= 32;
	for (line=0; line<6; line++)
	LCD_write_byte(font6x8[c][line], 1);
}
//---------------------------------------
//名称: 英文字符串显示函数
//参数：*s：英文字符串指针
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//-----------------------------------------  
void LCD_write_String(unsigned char X,unsigned char Y,uint8_t *s)//显示字符串
{
	uint8_t *s0;
	s0 = s;
	LCD_set_XY(X,Y);
	while (*s) 
	{
		LCD_write_char(*s);
		s++;
	}
	LCD_clear_A2B(X+(s-s0)*6,Y,1);
}

void LCD_write_number(unsigned char X,unsigned char Y,unsigned char c)//显示数字（小字体）
{
	 unsigned char line;
	 LCD_set_XY(X,Y);
	 for (line=0; line<=6; line++)
	 LCD_write_byte(number[c][line], 1);
} 

void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c) //row:列 page:页 dd:字符
{
	unsigned char i;  	
	
	LCD_set_XY(row*8, page);// 列，页 
	for(i=0; i<8;i++) 
	{
		LCD_write_byte(shuzi[c*16+i],1); 
	}
	
    LCD_set_XY(row*8, page+1);// 列，页 
	for(i=8; i<16;i++) 
	{
		LCD_write_byte(shuzi[c*16+i],1);
	}
	sce1;
}

//---------------------------------------
//名称: 写一个汉字到LCD函数
//参数：x,y：写入汉字的地址 address: 写入汉字在han表格中的位置
//作者：cby
//网站：宁波单片机开发网
//网址：www.nbdpj.com
//邮箱：nbdpj@nbdpj.com
//日期：20081111  
//----------------------------------------- 
/*
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c) //row:列 page:页 dd:字符
{
	unsigned char i;  	
	LCD_set_XY(row*10, page);// 列，页 
	for(i=0; i<10;i++) 
	{
		LCD_write_byte(designer[c*10+i],1); 
	}

    LCD_set_XY(row*10, page+1);// 列，页 
	for(i=10; i<8;i--) 
	{
		LCD_write_byte(designer[c*10+i],1);
	}	
	sce1;
}
*/
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c) //row:列 page:页 dd:字符
{
	unsigned char i;  	
	
	LCD_set_XY(row*8, page);// 列，页 
	for(i=0; i<16;i++) 
	{
		LCD_write_byte(designer[c*32+i],1); 
	}

    LCD_set_XY(row*8, page+1);// 列，页 
	for(i=16; i<32;i++) 
	{
		LCD_write_byte(designer[c*32+i],1);
	}	
	sce1;
}

