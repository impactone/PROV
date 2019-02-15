#include "laser.h"

void Laser_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); 

  //先初始化ADC3通道4 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PF6 通道4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化    
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
		
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);	//复位结束	 
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);	  //ADC1复位

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC3, &ADC_InitStructure);//ADC初始化
	
 
	ADC_Cmd(ADC3, ENABLE);//开启AD转换器	
}

u16 Get_Adc(u8 ch)   
{
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_480Cycles );	//ADC3,ADC通道,480个周期,提高采样时间可以提高精确度			     
	ADC_SoftwareStartConv(ADC3);			
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));
	return ADC_GetConversionValue(ADC3);	
}

u8 FirstFlag = 1;
//返回ADC采集的距离 单位：cm
float Get_Dis(void)
{
		u32 temp_val = 0;
	  if (isTooLong)
		{			
	    temp_val = Get_Adc(ADC_Channel_4);
		  //return ave_Filter(temp_val)*0.22524f + 100.0f;
			return temp_val*1.3088f;
			//return temp_val;
		}
		else
		{
			FirstFlag = 1;
			return 65535.0f;
		}
}

float ave_Filter(u32 temp_val)
{
	static u32 ADC_Buffer[filterNum] = {0};
  u8 i;
  float sum = 0.0f;
//滑动均值
  if (FirstFlag)
	{
		for (i = 0;i < filterNum;i++)
		ADC_Buffer[i] = temp_val;
		FirstFlag = 0;
	}
	else
	{
		for (i = 1;i < filterNum;i++)
			ADC_Buffer[i-1] = ADC_Buffer[i];
		ADC_Buffer[filterNum-1] = temp_val;
	}
//返回
  for (i = 0;i < filterNum;i++)
    sum += ADC_Buffer[i];
  return sum/filterNum;		
}
