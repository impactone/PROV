#include <stm32f4xx.h>
#include "select.h"	   
#include "stdio.h"


//�ⲿ�жϴ���
u16 counter1 = 0;
u16 counter2 = 0;

//ѡ����ɫ�˲���
//1Ϊ�죬2Ϊ�̣�3Ϊ��
u8 Filter = 1;

//�ⲿ�жϿ���
u8 OpenExti0 = 0;

//������ɫ��������
u16 red1=0,green1=0,blue1=0;
u16 red2=0,green2=0,blue2=0;

//��ɫ����Ƿ���ɱ�־
u8 isOver = 0;

//����ѡ�������IO�ڳ�ʼ��
void Select_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	 GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����	
	 GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	 FilterRate(4);
	 EXTIX_INIT();            //�ж��߳�ʼ��
	 TIM5_Int_Init(99,8399);  //��ʱ����ʼ��  100*8400/84000
}

//������ɫ�˲���
void RedFilter(void){
		S2 = 0;
	  S3 = 0;
}

//������ɫ�˲���
void GreenFilter(void)
{
		S2 = 1;
	  S3 = 1;
}

//������ɫ�˲���
void BlueFilter(void){
		S2 = 0;
	  S3 = 1;
}

//�˲�������ѡ�� 
//1ΪOFF
//2Ϊ2%
//3Ϊ20%
//4Ϊ100%
void FilterRate(u8 n){
	switch (n) {
		case 1:{S0 = 0;S1 = 0;break;}
		case 2:{S0 = 0;S1 = 1;break;}
		case 3:{S0 = 1;S1 = 0;break;}
		case 4:{S0 = 1;S1 = 1;break;}
	}
}


 
//�ⲿ�жϳ�ʼ������
void EXTIX_INIT(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    //GPIOB.14,15  �ж����Լ��жϳ�ʼ������
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource1);//PE2 ���ӵ��ж���2
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource2);
    //14��������
   	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		
	 //15���½���
   	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  
}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( 
		TIM5, 
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx�����
							 
}

//��ʱ��5�жϷ�����
void TIM5_IRQHandler(void)  
{
	  if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{ 
			if (!isOver){
			if (Filter == 1){
				RedFilter();
				if (!OpenExti0) {OpenExti0 = 1;counter1 = 0;counter2 = 0;} //���û�������ⲿ�ж�
				else{
					red1 = counter1;
					red2 = counter2;
					counter1 = 0;
					counter2 = 0;
					Filter = 2;
				  GreenFilter();					
				}
			}
			else
				if (Filter == 2){
			   	green1 = counter1;
					green2 = counter2;
		  		counter1 = 0;
					counter2 = 0;
		  		Filter = 3;
					BlueFilter();
			   }
				else
					if (Filter == 3){
						blue1 = counter1;
						blue2 = counter2;
						counter1 = 0;
						counter2 = 0;
						Filter = 1;
						OpenExti0 = 0;         //�ر��ⲿ�ж�
						isOver = 1;
					}
				}
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		}
}

void EXTI1_IRQHandler(void)
{
     if (OpenExti0) counter1++;
		 EXTI_ClearITPendingBit(EXTI_Line1);
}

//�ⲿ�жϷ����� 
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		if (OpenExti0) counter1++;
		EXTI_ClearITPendingBit(EXTI_Line14);  //���EXTI14��·����λ
	}
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (OpenExti0) counter2++;
		EXTI_ClearITPendingBit(EXTI_Line15);  //���EXTI15��·����λ
	}
}

