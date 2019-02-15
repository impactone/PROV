#include "laser.h"

void Laser_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); 

  //�ȳ�ʼ��ADC3ͨ��4 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PF6 ͨ��4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��    
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
		
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);	//��λ����	 
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);	  //ADC1��λ

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC3, &ADC_InitStructure);//ADC��ʼ��
	
 
	ADC_Cmd(ADC3, ENABLE);//����ADת����	
}

u16 Get_Adc(u8 ch)   
{
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_480Cycles );	//ADC3,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			     
	ADC_SoftwareStartConv(ADC3);			
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));
	return ADC_GetConversionValue(ADC3);	
}

u8 FirstFlag = 1;
//����ADC�ɼ��ľ��� ��λ��cm
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
//������ֵ
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
//����
  for (i = 0;i < filterNum;i++)
    sum += ADC_Buffer[i];
  return sum/filterNum;		
}
