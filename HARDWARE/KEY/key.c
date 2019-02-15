#include "key.h"
#include "delay.h"


void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
 	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
} 

u8 KEY_Scan(void)
{	 	  
	if(KEY0==0||KEY1==0||KEY2==0)
	{
		delay_ms(10);//ȥ���� 
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==1)return KEY2_PRES; 
	}     
	return 0;// �ް�������
}


