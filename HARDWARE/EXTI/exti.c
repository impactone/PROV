#include "exti.h"
#include "filter.h"
#include "usart.h"

//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);	
	
   	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		

		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}

 
 void EXTI9_5_IRQHandler(void)
{			
	if(EXTI_GetITStatus(EXTI_Line9) == SET)
	{
	  IMU_Filter();   //IMU������ݵ��˲�
		EXTI_ClearITPendingBit(EXTI_Line9);    //���LINE9�ϵ��жϱ�־λ  
		EXTI_ClearFlag(EXTI_Line9); 
	}
}


