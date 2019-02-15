#include <stm32f4xx.h>
#include "timer.h"
#include "attitude.h"
#include "control.h"

uint16_t SystemCounter = 0;
uint8_t __10ms = 0;
uint8_t __20ms = 0;
uint8_t __30ms = 0;
uint8_t __80ms = 0;
uint8_t __90ms = 0;
uint8_t __100ms = 0;

uint8_t SystemInitReady = 0;

void TIM1_Init(u32 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr;    //��װ��ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //100HZʱ��    
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);

//	TIM_PrescalerConfig(TIM1, 0, TIM_PSCReloadMode_Update);
//	TIM_UpdateDisableConfig(TIM1, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);  							 
}

void TIM2_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr;    //��װ��ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //Ԥ��Ƶֵ    
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	TIM_Cmd(TIM2, ENABLE);						 
}

void TIM4_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr;    //��װ��ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //Ԥ��Ƶֵ    
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 

	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	TIM_Cmd(TIM4, ENABLE);						 
}


//TIM1--ϵͳʱ�� TIM2--ʱ���ж� TIM3--PWM TIM4--UpdataQ TIM5--��ɫ TIM8--���
void TIM_Configuration(void)
{
//	TIM1_Init(0xffff,16799);	 //ֻ�ܸ�6��,�ﲻ��Ҫ��.
	TIM2_Init(99,8399);        //10ms/��  
//	TIM4_Init(199,839);        //2ms/��
}

/***********************************
���ƣ�   ��ȡϵͳʱ�亯��
���ܣ�   �õ�ϵͳʱ��
���룺   ��
�����   ��
************************************/
uint16_t Get_Time_Micros(void)
{
	return TIM1->CNT;
}

//TIM2�жϷ�����
void TIM2_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		if(SystemInitReady)
		{
		  SystemCounter++;
      if(SystemCounter > SYSTEM_COUNT)       SystemCounter = 1;
	    if((SystemCounter % COUNT_10MS) == 0)   __10ms  = 1;
			if((SystemCounter % COUNT_20MS) == 0)   __20ms  = 1;
			if((SystemCounter % COUNT_30MS) == 0)   __30ms  = 1;
			if((SystemCounter % COUNT_80MS) == 0)   __80ms  = 1;
			if((SystemCounter % COUNT_90MS) == 0)   __90ms  = 1;
			if((SystemCounter % COUNT_100MS) == 0)  __100ms = 1;
	  }
	  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);   
	}
}

//TIM4�жϷ�����
//void TIM4_IRQHandler(void)   
//{
//	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
//	{
//		if(SystemInitReady)
//		{
//		  UpdateQ();	 //������Ԫ��������Ƕ� 2ms/��
//		}
//	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);   
//	}
//}
