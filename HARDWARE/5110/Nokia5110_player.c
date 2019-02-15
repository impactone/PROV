#include "Nokia5110_library.h"
#include "Nokia5110_player.h"
#include "delay.h"
#include "stdio.h"


//Ӳ����·���������ϣ�nokia5110Һ��ģ����STM32��IO����������£�(F407ר�ã�)
//	RST  res   PB1   PC4 	//��λ,0��λ
//	CE   sce   PB12  PC3	//Ƭѡ
//	DC   dc    PB0   PC0	//1д���ݣ�0дָ��
//	Din  sdin  PC5   PC1	//����
//	Clk  sclk  PC4   PC2	//ʱ��
//	BL   backled  PA8  PC5 	//����
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO

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
//����: 5110LCD��ʼ������
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
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
	LCD_write_byte(0x21,0);//LCD�������ã�оƬ���ˮƽѰַ��ʹ����չָ��
	LCD_write_byte(0xd0,0);//����VOPֵ�������µı�̷�ΧΪ3.00-10.68
	//Vlcd=3.06+(VOP)*0.06,����VOPΪ0B0101 0000Ϊʮ���Ƶ�80��Vlcd=7.86V
	LCD_write_byte(0x20,0);//LCD�������ã�оƬ���ˮƽѰַ��ʹ�û���ָ��
	LCD_write_byte(0x0C,0);//�趨��ʾ����:��ͨģʽ
	sce1;
	LCD_clearall();
}
//---------------------------------------
//����: �������꺯��
//������X��0��83 Y��0��5
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
//-----------------------------------------   
void LCD_set_XY(unsigned char X, unsigned char Y)//X��Χ��0~200��Y��Χ��0~5����
{
	LCD_write_byte(0x40 | Y, 0);// column
	LCD_write_byte(0x80 | X, 0);// row
	sce1;
} 
//---------------------------------------
//����: LCD��������
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
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
//����: ��ʾӢ���ַ�
//������c����ʾ���ַ���font6x8����е�λ��
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
//-----------------------------------------  	
void LCD_write_char(unsigned char c)
{
	unsigned char line;
	c-= 32;
	for (line=0; line<6; line++)
	LCD_write_byte(font6x8[c][line], 1);
}
//---------------------------------------
//����: Ӣ���ַ�����ʾ����
//������*s��Ӣ���ַ���ָ��
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
//-----------------------------------------  
void LCD_write_String(unsigned char X,unsigned char Y,uint8_t *s)//��ʾ�ַ���
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

void LCD_write_number(unsigned char X,unsigned char Y,unsigned char c)//��ʾ���֣�С���壩
{
	 unsigned char line;
	 LCD_set_XY(X,Y);
	 for (line=0; line<=6; line++)
	 LCD_write_byte(number[c][line], 1);
} 

void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c) //row:�� page:ҳ dd:�ַ�
{
	unsigned char i;  	
	
	LCD_set_XY(row*8, page);// �У�ҳ 
	for(i=0; i<8;i++) 
	{
		LCD_write_byte(shuzi[c*16+i],1); 
	}
	
    LCD_set_XY(row*8, page+1);// �У�ҳ 
	for(i=8; i<16;i++) 
	{
		LCD_write_byte(shuzi[c*16+i],1);
	}
	sce1;
}

//---------------------------------------
//����: дһ�����ֵ�LCD����
//������x,y��д�뺺�ֵĵ�ַ address: д�뺺����han����е�λ��
//���ߣ�cby
//��վ��������Ƭ��������
//��ַ��www.nbdpj.com
//���䣺nbdpj@nbdpj.com
//���ڣ�20081111  
//----------------------------------------- 
/*
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c) //row:�� page:ҳ dd:�ַ�
{
	unsigned char i;  	
	LCD_set_XY(row*10, page);// �У�ҳ 
	for(i=0; i<10;i++) 
	{
		LCD_write_byte(designer[c*10+i],1); 
	}

    LCD_set_XY(row*10, page+1);// �У�ҳ 
	for(i=10; i<8;i--) 
	{
		LCD_write_byte(designer[c*10+i],1);
	}	
	sce1;
}
*/
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c) //row:�� page:ҳ dd:�ַ�
{
	unsigned char i;  	
	
	LCD_set_XY(row*8, page);// �У�ҳ 
	for(i=0; i<16;i++) 
	{
		LCD_write_byte(designer[c*32+i],1); 
	}

    LCD_set_XY(row*8, page+1);// �У�ҳ 
	for(i=16; i<32;i++) 
	{
		LCD_write_byte(designer[c*32+i],1);
	}	
	sce1;
}

