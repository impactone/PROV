#include <stm32f4xx.h>
#include "select.h"	   
#include "stdio.h"


//外部中断次数
u16 counter1 = 0;
u16 counter2 = 0;

//选择颜色滤波器
//1为红，2为绿，3为蓝
u8 Filter = 1;

//外部中断开关
u8 OpenExti0 = 0;

//三种颜色的脉冲数
u16 red1=0,green1=0,blue1=0;
u16 red2=0,green2=0,blue2=0;

//颜色检测是否完成标志
u8 isOver = 0;

//用于选择参数的IO口初始化
void Select_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	 GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉	
	 GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	 FilterRate(4);
	 EXTIX_INIT();            //中断线初始化
	 TIM5_Int_Init(99,8399);  //定时器初始化  100*8400/84000
}

//开启红色滤波器
void RedFilter(void){
		S2 = 0;
	  S3 = 0;
}

//开启绿色滤波器
void GreenFilter(void)
{
		S2 = 1;
	  S3 = 1;
}

//开启蓝色滤波器
void BlueFilter(void){
		S2 = 0;
	  S3 = 1;
}

//滤波器速率选择 
//1为OFF
//2为2%
//3为20%
//4为100%
void FilterRate(u8 n){
	switch (n) {
		case 1:{S0 = 0;S1 = 0;break;}
		case 2:{S0 = 0;S1 = 1;break;}
		case 3:{S0 = 1;S1 = 0;break;}
		case 4:{S0 = 1;S1 = 1;break;}
	}
}


 
//外部中断初始化函数
void EXTIX_INIT(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    //GPIOB.14,15  中断线以及中断初始化配置
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource1);//PE2 连接到中断线2
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource2);
    //14在上升沿
   	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		
	 //15在下降沿
   	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  
}

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig( 
		TIM5, 
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM5, ENABLE);  //使能TIMx外设打开
							 
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)  
{
	  if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{ 
			if (!isOver){
			if (Filter == 1){
				RedFilter();
				if (!OpenExti0) {OpenExti0 = 1;counter1 = 0;counter2 = 0;} //如果没开，则开外部中断
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
						OpenExti0 = 0;         //关闭外部中断
						isOver = 1;
					}
				}
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		}
}

void EXTI1_IRQHandler(void)
{
     if (OpenExti0) counter1++;
		 EXTI_ClearITPendingBit(EXTI_Line1);
}

//外部中断服务函数 
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		if (OpenExti0) counter1++;
		EXTI_ClearITPendingBit(EXTI_Line14);  //清除EXTI14线路挂起位
	}
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (OpenExti0) counter2++;
		EXTI_ClearITPendingBit(EXTI_Line15);  //清除EXTI15线路挂起位
	}
}

