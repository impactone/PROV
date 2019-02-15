#include "pwm.h"
#include "delay.h"
#include "task.h"

/***********************************
���ƣ�   PWM��ʼ������
���ܣ�   ��������Ͷ��PWM���
���룺   ARR��PSC,���ڿ���Ƶ��
�����   ��
************************************/
void PWM_Configuration(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
 	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//C6--TIM8_CH1  
	//C7--TIM8_CH2 C8--TIM8_CH3 C9--TIM8_CH4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler =16800-1;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);

//  TIM_CtrlPWMOutputs(TIM3,ENABLE);	
	TIM_CtrlPWMOutputs(TIM8,ENABLE);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
	TIM_ARRPreloadConfig(TIM8, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	

	
	//�����ԭ
	RotatorControl1(MID1);
	RotatorControl2(MID2);
	WhiteDoor = CLOSEW;
	BlackDoor = CLOSEB;
	
	//�����ʼ����ʼ
  Wind1 = 0;             
  Wind2 = 0;
}

/***********************************
���ƣ�   ���Ʒ������
���ܣ�   ���PWM���Ʒ������
���룺   ������� STARTΪ�� STOPΪ��
�����   ��
************************************/
void WindMotor1(u8 mode)
{
   if (mode)
	 {
		 Wind1 = 16;
	 }
	 else
	 {
		 Wind1 = 10;
	 }
}

/***********************************
���ƣ�   ���Ʒ������
���ܣ�   ���PWM���Ʒ������
���룺   ������� STARTΪ�� STOPΪ��
�����   ��
************************************/
void WindMotor2(u8 mode)
{
   if (mode)
	 {
		 Wind2 = 16;
	 }
	 else
	 {
		 Wind2 = 10;
	 }
}

/***********************************
���ܣ�   ���ƶ��
���룺   ����Ƕ�
				 MID1Ϊ��ԭ LEFT1Ϊ����� RIGHT1Ϊ���Ҵ�
�����   ��
************************************/
void RotatorControl1(u8 mode)
{
	Rotator1 = mode;
}

/***********************************
���ܣ�   ���ƶ��
���룺   ����Ƕ�
				 MID2Ϊ��ԭ LEFT2Ϊ����� RIGHT2Ϊ���Ҵ�
�����   ��
************************************/
void RotatorControl2(u8 mode)
{
	Rotator2 = mode;
}

u8 OpenWhiteDoor(u8 isOpen,u16 SysCnt)
{
	static u8 last_cnt = 0;
	static u8 ste_status = 1;  //1Ϊ�ر� 2Ϊ��ing 3Ϊ�Ѿ���
	
	if (!isOpen)   //�ز���
	{
		WhiteDoor = CLOSEW;
		ste_status = 1;
		return 0;
	}
	
  switch (ste_status)
  {
		case 1:
			WhiteDoor = OPENW;
		  ste_status = 2;
		  last_cnt = SysCnt;
			break;
		case 2:
	    if (SysCnt - last_cnt >= OPEN_DOOR_TIME)
	    {
				ste_status = 3;
		    return 1;
    	}			
			break;
		case 3:
			return 1;
		default:
			break;
	}
	return 0;
}

u8 OpenBlackDoor(u8 isOpen,u16 SysCnt)
{
	static u8 last_cnt = 0;
	static u8 ste_status = 1;  //1Ϊ�ر� 2Ϊ��ing 3Ϊ�Ѿ���

	if (!isOpen)   //�ز���
	{
		BlackDoor = CLOSEB;
		ste_status = 1;
		return 0;
	}
	
  switch (ste_status)
  {
		case 1:
			BlackDoor = OPENB;
		  ste_status = 2;
		  last_cnt = SysCnt;
			break;
		case 2:
	    if (SysCnt - last_cnt >= OPEN_DOOR_TIME)
	    {
				ste_status = 3;
		    return 1;
    	}			
			break;
		case 3:
			return 1;
			break;
		default:
			break;
	}
	return 0;
}
